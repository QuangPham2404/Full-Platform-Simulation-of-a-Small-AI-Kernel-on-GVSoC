import gvsoc.runner
import gvsoc.systree
import vp.clock_domain
from gvrun.parameter import TargetParameter

import cpu.iss.riscv
import interco.router
import memory.memory
import utils.loader.loader
import gdbserver.gdbserver
import dot_product_component

GAPY_TARGET = True


class Target(gvsoc.runner.Target):
    def __init__(self, parser, options, name=None):
        super().__init__(
            parser, options, model=Rv64,
            description="RV64 virtual board", name=name
        )


class Rv64(gvsoc.systree.Component):
    def __init__(self, parent, name, parser, options):
        super().__init__(parent, name, options=options)

        # Independent, fixed CPU and peripheral/DPC clock domains.
        cpu_clock = vp.clock_domain.Clock_domain(
            self, "cpu_clock", frequency=100000000
        )
        dpc_clock = vp.clock_domain.Clock_domain(
            self, "dpc_clock", frequency=50000000
        )

        binary = TargetParameter(
            self, name="binary", value=None,
            description="Binary to be simulated"
        ).get_value()
        soc = Soc(self, "soc", binary)

        # Bind to Soc's exposed ports; its children are bound inside Soc.
        cpu_clock.o_CLOCK(soc.i_CPU_CLOCK())
        dpc_clock.o_CLOCK(soc.i_DPC_CLOCK())


class Soc(gvsoc.systree.Component):
    def i_CPU_CLOCK(self) -> gvsoc.systree.SlaveItf:
        return gvsoc.systree.SlaveItf(
            self, "cpu_clock", signature="clock"
        )

    def i_DPC_CLOCK(self) -> gvsoc.systree.SlaveItf:
        return gvsoc.systree.SlaveItf(
            self, "dpc_clock", signature="clock"
        )

    def __init__(self, parent, name, binary):
        super().__init__(parent, name)

        host = cpu.iss.riscv.Riscv(
            self, "cpu", isa="rv64imafdc", binaries=[binary]
        )
        ico = interco.router.Router(self, "ico")
        mem = memory.memory.Memory(self, "mem", size=0x00100000)
        loader = utils.loader.loader.ElfLoader(
            self, "loader", binary=binary
        )
        gdbserver.gdbserver.Gdbserver(self, "gdbserver")
        dcp = dot_product_component.DotProductComponent(self, "dcp")

        host.o_DATA(ico.i_INPUT())
        host.o_FETCH(ico.i_INPUT())
        host.o_DATA_DEBUG(ico.i_INPUT())

        ico.o_MAP(
            mem.i_INPUT(), "mem",
            base=0x00000000, rm_base=True, size=0x00100000
        )

        loader.o_OUT(ico.i_INPUT())
        loader.o_ENTRY(host.i_ENTRY())
        loader.o_START(host.i_FETCHEN())

        # Preserve the existing 4 KiB MMIO window and V2.2 regmap.
        ico.o_MAP(
            dcp.i_INPUT(), "dcp",
            base=0x20000000, size=0x00001000, rm_base=True
        )

        # Tutorial 9: forward boundary clock ports at this hierarchy level.
        self.bind(
            self, "cpu_clock", host, "clock",
            master_signature="clock", slave_signature="clock"
        )
        for component in (ico, mem, loader, dcp):
            self.bind(
                self, "dpc_clock", component, "clock",
                master_signature="clock", slave_signature="clock"
            )

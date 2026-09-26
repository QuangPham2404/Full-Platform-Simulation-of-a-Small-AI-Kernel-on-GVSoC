import gvsoc.runner

# Add target marker
GAPY_TARGET = True

# Create target class
class Target(gvsoc.runner.Target):
    def __init__(self, parser, options, name=None):
        super(Target, self).__init__(
            parser,
            options,
            model=Rv64,
            description="RV64 virtual board",
            name=name
        )


# Create the virtual board
import gvsoc.systree
import vp.clock_domain
from gvrun.parameter import TargetParameter
import memory.memory
import interco.router
import cpu.iss.riscv
import utils.loader.loader
import gdbserver.gdbserver
import dot_product_component

'''
This project's component tree is as follows:

Rv64                      ← board/top level
│
├── clock
│
└── soc                   ← actual chip
    │
    ├── CPU (host)
    ├── interconnect (ico)
    ├── Memory (mem)
    ├── Loader (loader)
    ├── gdbserver
    ├── Dot Product Component (dcp)
    ├──── trace
'''

# Create top component of the component tree
class Rv64(gvsoc.systree.Component):
    def __init__(self, parent, name, parser, options):
        super().__init__(parent, name, options=options)

        # Define the clock for the boards
        clock = vp.clock_domain.Clock_domain(self, 'clock', frequency=100000000)

        # Handle the binary paramter passed through gvrun
        binary = TargetParameter(
            self, name='binary', value=None,
            description='Binary to be simulated'
        ).get_value()
        soc = Soc(self, 'soc', binary)
        
        # Bind output_CLOCK of clock to input_CLOCK of soc    
        clock.o_CLOCK(soc.i_CLOCK())

# Create the SoC component and define its sub-components
class Soc(gvsoc.systree.Component):
    def __init__(self, parent, name, binary):
        super().__init__(parent, name)

        # Create the CPU component
        host = cpu.iss.riscv.Riscv(self, "cpu", isa="rv64imafdc", binaries=[binary])

        # Create the interconnect for the SoC 
        ico = interco.router.Router(self, "ico")

        # Create the memory (register) for the SoC
        mem = memory.memory.Memory(self, "mem", size=0x00100000)

        # Create the EFL binary loader for the SoC
        loader = utils.loader.loader.ElfLoader(self, "loader", binary=binary)

        # Create the GDB server for the SoC
        gdbserver = gdbserver.gdbserver.Gdbserver(self, "gdbserver")

        # Create the Dot Product Component for the SoC
        dcp = dot_product_component.DotProductComponent(self, "dcp")

        # Connect CPU OUTPUT to interconnect INPUT
        host.o_DATA(ico.i_INPUT())
        host.o_FETCH(ico.i_INPUT())
        host.o_DATA_DEBUG(ico.i_INPUT())

        # Connect interconnect OUTPUT to memory INPUT
        ico.o_MAP(mem.i_INPUT(), 'mem', base=0x00000000, rm_base=True, size=0x00100000)

        # Connect loader OUTPUT to ico INPUT to send the binary to memory
        # AND: Connect loader OUTPUT to CPU ENTRY and START INPUTS to specify (1) the entry point of the binary and (2) the start signal
        loader.o_OUT(ico.i_INPUT())
        loader.o_ENTRY(host.i_ENTRY())
        loader.o_START(host.i_FETCHEN())

        # Connect ICO OUTPUT to DCP INPUT to send operands from CPU to DCP using register mapping
        ico.o_MAP(
            dcp.i_INPUT(),
            "dcp",
            base=0x20000000,
            size=0x00001000,
            rm_base=True
        )


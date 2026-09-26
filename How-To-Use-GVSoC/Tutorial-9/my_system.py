import gvsoc.runner


# The marker so that gvsoc knows that this file contains our defintion of our target.
GAPY_TARGET = True


# Create a "Target" class that inherits the built in gvsoc.runner.Target class. 
'''
This class will be used to define the target architecture and its properties.
The target for this project is a Rv64 virtual board, 
as we defined when we are instantiating the children class in the constructor.
'''
class Target(gvsoc.runner.Target):
    def __init__(self, parser, options, name=None):
        super(Target, self).__init__(
            parser, 
            options,
            model=Rv64, 
            description="RV64 virtual board",
            name=name)


# Create the actual virtual board
import gvsoc.systree
import vp.clock_domain

'''
This project's component tree is as follows:

Rv64                      ← board/top level
│
├── clock1 (for host)
├── clock2 (for the rest)
│
└── soc                   ← actual chip
    │
    ├── CPU (host)
    ├── interconnect (ico)
    ├── Memory (mem)
    ├── Loader (loader)
    ├── gdbserver
    ├── My_comp (comp)
    ├──── trace
    └── My_comp2 (comp2)
'''

# We create the top component of our component tree of our virtual board.
# The top component must always inherit from gvsoc.systree.Component - this is the base class for all components in the system tree.
# This is also where we define the clock zone for our virtual board.
class Rv64(gvsoc.systree.Component):
    def __init__(self, parent, name, parser, options):
        super().__init__(parent, name, options=options)

        # We define 2 clock domains
        clock1 = vp.clock_domain.Clock_domain(self, 'clock1', frequency=100000000)
        clock2 = vp.clock_domain.Clock_domain(self, 'clock2', frequency=100000000)

        # gvrun supplies the ELF through --parameter binary=.... Resolve that
        # parameter here and pass the resulting path to the SoC.
        binary = TargetParameter(
            self, name='binary', value=None,
            description='Binary to be simulated'
        ).get_value()

        soc = Soc(self, 'soc', binary)

        # PORT BINDING: Bind output_CLOCK of clock to input_CLOCK of the components in each domain
        # Here the host is one clock domain, the rest belong to domain 2

        # FIX:
        # Rv64 must only bind to its direct child "soc", rather than reaching
        # directly into soc.host, soc.mem, etc.
        # Soc will route these clocks to its internal components.
        clock1.o_CLOCK(soc.i_HOST_CLOCK())
        clock2.o_CLOCK(soc.i_SYSTEM_CLOCK())

        # Since we allow comp to control host's clock, we need to do port binding of the CLK_CTRL ports

        # FIX:
        # The clock-control signal also passes through the Soc boundary.
        # From Rv64's perspective, Soc is the master controlling clock1.
        soc.o_CLK_CTRL(clock1.i_CTRL())


from gvrun.parameter import TargetParameter
import memory.memory
import interco.router
import cpu.iss.riscv
import utils.loader.loader
import gdbserver.gdbserver

# Import our new component from my_comp.py
import my_comp


# Define the SoC component of our virtual board.
class Soc(gvsoc.systree.Component):

    # FIX:
    # Expose a Soc-level input port for the host clock.
    # Rv64 connects clock1 to this port.
    def i_HOST_CLOCK(self) -> gvsoc.systree.SlaveItf:
        return gvsoc.systree.SlaveItf(
            self,
            'host_clock',
            signature='clock'
        )

    # FIX:
    # Expose a second Soc-level input port for the clock used by
    # the rest of the SoC.
    # Rv64 connects clock2 to this port.
    def i_SYSTEM_CLOCK(self) -> gvsoc.systree.SlaveItf:
        return gvsoc.systree.SlaveItf(
            self,
            'system_clock',
            signature='clock'
        )

    # FIX:
    # Expose the clock-control output of Soc.
    # Internally this will be driven by my_comp.
    def o_CLK_CTRL(self, itf: gvsoc.systree.SlaveItf):
        self.itf_bind(
            'clk_ctrl',
            itf,
            signature='clock_ctrl'
        )

    def __init__(self, parent, name, binary):
        super().__init__(parent, name)

        # The binary was resolved by Rv64 using gvrun's parameter mechanism;
        # it is passed directly to the SoC instead of read from argparse.

        # Define the memory for the SoC
        self.mem = memory.memory.Memory(
            self,
            'memory',
            size=0x00100000
        ) # 1MB memory

        # Define the interconnect for the SoC
        self.ico = interco.router.Router(self, 'ico')

        # Connect the SoC interconnect output to the SoC memory input
        '''
        Syntax for binding memory to interconnect (creating a memory mapping):
        - Here the memory "space" range is from "base" to "base + size". i.e. the range of the mem [base, base + size))
        - "remove_offset" is used to convert the global address to the local address that the memory sees. 
        For example, if the global address is 0x00000000 and the memory sees it as 0x00000000, then remove_offset is 0x00000000. 
        If the global address is 0x10000000 and the memory sees it as 0x00000000, then remove_offset is 0x10000000.
        '''
        # Use the current mapping API for a zero base offset.
        self.ico.o_MAP(
            self.mem.i_INPUT(),
            'mem',
            base=0x00000000,
            rm_base=True,
            size=0x00100000
        )

        # Define the CPU for the SoC
        # Associate the ELF with the CPU using the current RISC-V model API.
        self.host = cpu.iss.riscv.Riscv(
            self,
            'host',
            isa='rv64imafdc',
            binaries=[binary]
        )

        # Connect the SoC interconnect output to the SoC CPU input, 
        # and thus now there is a connection between the CPU and the memory through the interconnect.
        # Note that FETCH is for instruction, DATA is for computation data, and DATA_DEBUG is for debug data.
        self.host.o_DATA(self.ico.i_INPUT())
        self.host.o_FETCH(self.ico.i_INPUT())
        self.host.o_DATA_DEBUG(self.ico.i_INPUT())

        # Define the loader for the SoC - this is NOT hardware, 
        # but a software component to load the EFL binary for the board to run during the simulation.
        # Note the binary is parsed from the command line arguments above.
        self.loader = utils.loader.loader.ElfLoader(
            self,
            'loader',
            binary=binary
        )

        # Connect the loader output to the interconnect input, so that the loader can load into the memory
        # Connect the loader output to the CPU input, 
        # giving the input the entry point of the memory and the instruction to start executing the binary.
        '''
        Note about entry point: Here at the low level, the user must define the entry point correctly based on the defined range during the linking mem with ico
        For higher level, like normal computers, the OS will take care of this for the user, but here we are simulating a bare metal system, so the user must define the entry point correctly.
        '''
        self.loader.o_OUT(self.ico.i_INPUT())
        self.loader.o_START(self.host.i_FETCHEN())
        self.loader.o_ENTRY(self.host.i_ENTRY())
        
        # Define the GDB debug server for the SoC (optional) - this is NOT hardware, 
        # but a software component to allow the user to debug the virtual board using GDB.
        gdbserver.gdbserver.Gdbserver(self, 'gdbserver')

        # Create a child component for "my_comp" and connect it to the ico.
        # The base is chosen arbitrarily outside of the "mem" component range to ensure does clash. It's just used to simulate how hardware works.
        self.comp = my_comp.MyComp(
            self,
            "my_comp",
            value=0x12345678
        )

        self.ico.o_MAP(
            self.comp.i_INPUT(),
            "comp",
            base=0x20000000,
            size=0x00001000,
            rm_base=True
        )

        # Create a child component and connect its ports to my_comp
        # Note that you connect o --> i
        comp2 = my_comp.MyComp2(self, "my_comp2")
        self.comp.o_NOTIF(comp2.i_NOTIF())
        comp2.o_RESULT(self.comp.i_RESULT())


        # ============================================================
        # FIX: Clock-domain bindings are now done INSIDE Soc.
        # ============================================================

        # The external "host_clock" port is forwarded only to the CPU.
        self.bind(
            self, 'host_clock',
            self.host, 'clock',
            master_signature='clock',
            slave_signature='clock'
        )

        # The external "system_clock" port is forwarded to the rest
        # of the clocked components inside the SoC.
        self.bind(
            self, 'system_clock',
            self.mem, 'clock',
            master_signature='clock',
            slave_signature='clock'
        )

        self.bind(
            self, 'system_clock',
            self.ico, 'clock',
            master_signature='clock',
            slave_signature='clock'
        )

        self.bind(
            self, 'system_clock',
            self.loader, 'clock',
            master_signature='clock',
            slave_signature='clock'
        )

        self.bind(
            self, 'system_clock',
            self.comp, 'clock',
            master_signature='clock',
            slave_signature='clock'
        )

        # MyComp2 also uses clock events from the previous tutorials,
        # so keep it in the second clock domain as well.
        self.bind(
            self, 'system_clock',
            comp2, 'clock',
            master_signature='clock',
            slave_signature='clock'
        )

        # FIX:
        # Forward my_comp's clk_ctrl master port to Soc's external
        # clk_ctrl port. Rv64 then connects this Soc port to clock1.
        self.comp.o_CLK_CTRL(
            gvsoc.systree.SlaveItf(
                self,
                'clk_ctrl',
                signature='clock_ctrl'
            )
        )
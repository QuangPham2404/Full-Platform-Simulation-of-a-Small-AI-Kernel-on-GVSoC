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
├── clock
│
└── soc                   ← actual chip
    │
    ├── CPU (host)
    ├── interconnect (ico)
    ├── Memory (mem)
    ├── Loader (loader)
    ├── gdbserver
    └── My_comp (comp)
'''

# We create the top component of our component tree of our virtual board.
# The top component must always inherit from gvsoc.systree.Component - this is the base class for all components in the system tree.
# This is also where we define the clock zone for our virtual board.
class Rv64(gvsoc.systree.Component):
    def __init__(self, parent, name, parser, options):
        super().__init__(parent, name, options=options)

        # Define the clock for the board
        clock = vp.clock_domain.Clock_domain(self, 'clock', frequency=100000000)

        # gvrun supplies the ELF through --parameter binary=.... Resolve that
        # parameter here and pass the resulting path to the SoC.
        binary = TargetParameter(
            self, name='binary', value=None,
            description='Binary to be simulated'
        ).get_value()
        soc = Soc(self, 'soc', binary)

        # PORT BINDING: Bind output_CLOCK of clock to input_CLOCK of soc    
        clock.o_CLOCK(soc.i_CLOCK())

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
    def __init__(self, parent, name, binary):
        super().__init__(parent, name)

        # The binary was resolved by Rv64 using gvrun's parameter mechanism;
        # it is passed directly to the SoC instead of read from argparse.

        # Define the memory for the SoC
        mem = memory.memory.Memory(self, 'memory', size=0x00100000) # 1MB memory

        # Define the interconnect for the SoC
        ico = interco.router.Router(self, 'ico')

        # Connect the SoC interconnect output to the SoC memory input
        '''
        Syntax for binding memory to interconnect (creating a memory mapping):
        - Here the memory "space" range is from "base" to "base + size". i.e. the range of the mem [base, base + size))
        - "remove_offset" is used to convert the global address to the local address that the memory sees. 
        For example, if the global address is 0x00000000 and the memory sees it as 0x00000000, then remove_offset is 0x00000000. 
        If the global address is 0x10000000 and the memory sees it as 0x00000000, then remove_offset is 0x10000000.
        '''
        # Use the current mapping API for a zero base offset.
        ico.o_MAP(mem.i_INPUT(), 'mem', base=0x00000000, rm_base=True, size=0x00100000)

        # Define the CPU for the SoC
        # Associate the ELF with the CPU using the current RISC-V model API.
        host = cpu.iss.riscv.Riscv(self, 'host', isa='rv64imafdc', binaries=[binary])

        # Connect the SoC interconnect output to the SoC CPU input, 
        # and thus now there is a connection between the CPU and the memory through the interconnect.
        # Note that FETCH is for instruction, DATA is for computation data, and DATA_DEBUG is for debug data.
        host.o_DATA(ico.i_INPUT())
        host.o_FETCH(ico.i_INPUT())
        host.o_DATA_DEBUG(ico.i_INPUT())

        # Define the loader for the SoC - this is NOT hardware, 
        # but a software component to load the EFL binary for the board to run during the simulation.
        # Note the binary is parsed from the command line arguments above.
        loader = utils.loader.loader.ElfLoader(self, 'loader', binary=binary)

        # Connect the loader output to the interconnect input, so that the loader can load into the memory
        # Connect the loader output to the CPU input, 
        # giving the input the entry point of the memory and the instruction to start executing the binary.
        '''
        Note about entry point: Here at the low level, the user must define the entry point correctly based on the defined range during the linking mem with ico
        For higher level, like normal computers, the OS will take care of this for the user, but here we are simulating a bare metal system, so the user must define the entry point correctly.
        '''
        loader.o_OUT(ico.i_INPUT())
        loader.o_START(host.i_FETCHEN())
        loader.o_ENTRY(host.i_ENTRY())
        
        # Define the GDB debug server for the SoC (optional) - this is NOT hardware, 
        # but a software component to allow the user to debug the virtual board using GDB.
        gdbserver.gdbserver.Gdbserver(self, 'gdbserver')

        # Create a child component for "my_comp" and connect it to the ico.
        # The base is chosen arbitrarily outside of the "mem" component range to ensure does clash. It's just used to simulate how hardware works.
        comp = my_comp.MyComp(self, "my_comp", value = 0x12345678)
        ico.o_MAP(comp.i_INPUT(), "comp", base=0x20000000, size=0x00001000, rm_base=True)
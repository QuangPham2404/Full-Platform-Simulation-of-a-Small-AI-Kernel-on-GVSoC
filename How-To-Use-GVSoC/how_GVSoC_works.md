# GVSoC Tutorials

Link: https://gvsoc-developer.readthedocs.io/en/latest/tutorials.html#how-to-build-a-system-from-scratch

Note that the some of the code in the guides in this link are outdated. To solve related errors with the scripts, check the `solution/` directory in the tutorials directories.

In current repo, tutorials are availabe at: ``/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/`

## HOW TO USE GVSOC

**0. A GENERAL STRUCTURE OF A GVSOC PROJECT**

In a directory of a GVSoC, we will have:


```bash
include ../utils/rt.mk

GVSOC_ROOT ?= ../../../../..
GVSOC_WORKDIR ?= $(CURDIR)/build
ifdef BUILDDIR
FULL_BUILDDIR := $(GVSOC_WORKDIR)/$(BUILDDIR)
else
FULL_BUILDDIR := $(GVSOC_WORKDIR)
endif

clean:
        rm -rf $(FULL_BUILDDIR)/test
        make -C $(GVSOC_ROOT) TARGETS=my_system MODULES=$(CURDIR) BUILDDIR=$(FULL_BUILDDIR)/build INSTALLDIR=$(FULL_BUILDDIR)/install $(OPTIONS) clean

all:
        mkdir -p $(FULL_BUILDDIR)/test
        riscv64-unknown-elf-gcc -g -o $(FULL_BUILDDIR)/test/test main.c $(RT_SRCS) $(RT_FLAGS)

gvsoc:
        make -C $(GVSOC_ROOT) TARGETS=my_system MODULES=$(CURDIR) BUILDDIR=$(FULL_BUILDDIR)/build INSTALLDIR=$(FULL_BUILDDIR)/install $(OPTIONS) build

prepare:
        cp solution/* .

run:
        $(FULL_BUILDDIR)/install/bin/gvrun --target-dir=$(CURDIR) --target=my_system --work-dir=$(FULL_BUILDDIR)/work --parameter binary=$(FULL_BUILDDIR)/test/test run $(runner_args)
```

**1. DESIGN THE VIRTUAL CHIP:**

- Python: Design the system (your code)
- C++: Describe hardware (GVSoC plattform provide, you code your self if you create a new component)
- `make gvsoc`: compile and build the "digital version of the chip" under the form of a executable binary of the simulator specifically for this simulated chip case. Read the `Makefile` for details of what is run during the command, and where the build ouputs will be stored (usually this will be in `build/build` and `build/install`)

**2. BUILD THE SOFTWARE:**

- You need something for your simulator to run. So you write a program in C++/C.
- Compile it for GVSoC CPU (compiler provided by plattform) and obtain an EFL executable. The command used is usually `make all` (check the `Makefile` to make sure). The EFL binary will usually be stored in `build/test`.

**3. RUN THE SIMULATION**

- GVSoC is the program that will run the simulation. GVSoC take: simulator binary + EFL executable and runs the simulation on your computer's CPU
- Result: the output of the EFL executable and also traces if you specify

## HOW GVSOC WORKS

### The component tree

GVSoC models a machine as a tree of components, there for when creating the model to simulate the concept of `parent` is very important, since it defines where that component is stored in the to-be-simulated plattform's component tree.

For example, let's say our simulated plattform have this simple component tree:

```txt
Rv64
│
├── Clock
│
└── SoC
    │
    ├── CPU
    ├── Interconnect
    └── RAM
```

If we are to code the clock component, we will have something like:

```python
def __init__(self, parent, name, parser, options):
    ...

clock = vp.clock_domain.Clock_domain(
    self,
    'clock',
    ...
)
```

This means:

```txt
self    → parent
'clock' → name
```

### Connecting components

In GVSoC we connect components (e.g. memory with interconnect or clock to SoC) by binding the output/input ports of the components together

## HOW WE CODE UP A BOARD FOR SIMULATION

- In the Python file: We first define the "target" (Basically its a "sign" that basically points our to gvsoc that hey this is what we'll simulate), then we start to work on the component tree. Starting of with the board, which contains the clock, and other "big" components, for instance in this exmaple with have the "soc". Below are important concepts when coding the system.py:
  - `parent` and `name` to define component in the component tree
  - binding `<component>.o_XXXX` to `<component>.i_XXXX`. We only need to define one direction during binding and connect the component to the interconnect (for instance CPU --> interconnect, and interconnect --> memory). The interconnect will automatically handle the 2-way data transfer during the simulation. 
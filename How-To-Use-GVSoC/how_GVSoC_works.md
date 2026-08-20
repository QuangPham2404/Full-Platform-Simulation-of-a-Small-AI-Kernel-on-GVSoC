# GVSoC Tutorials

Link: https://gvsoc-developer.readthedocs.io/en/latest/tutorials.html#how-to-build-a-system-from-scratch

## How GVSoC works

1. DESIGN THE VIRTUAL CHIP:
- Python: Design the system (you code)
- C++: Describe hardware (GVSoC plattform provide, you code your self if you create a new component)
- `make gvsoc`: compile and build the "digital version of the chip" under the form of a executable binary of the simulator specifically for this simulated chip case

2. BUILD THE HARDWARE:
- You need something for your simulator to run. So you write a program in C++
- Compile it for GVSoC CPU (compiler provided by plattform) and obtain an EFL executable

3. RUN THE SIMULATION
- GVSoC is the program that will run the simulation. GVSoC take: simulator binary + EFL executable and runs the simulation on your computer's CPU
- Result: the output of the EFL executable and also traces if you specify

## Tutorials notes

### Tutorial 1

**Key words and Concepts**
- Interconnect: collection of paths and switches that allow components in a chip (e.g. core, memeory, I/O, etc) to communcate with each other.
- Target: is a plattform. A plattform includes: clock + SoC (CPU with defined ISA, RAM, I/O, etc)
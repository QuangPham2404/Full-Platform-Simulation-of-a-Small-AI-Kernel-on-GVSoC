# Tutorial 5 Notes

## Summary

Previously, the component mainly responded at offset `0x0`. Tutorial 5 expands it in three stages:

### 1. Manually add more registers

- Offset `0x0`: original behavior.
- Offset `0x4`: returns twice the configured value.
- The request handler checks the address to decide what to do.

### 2. Use GVSoC's `vp::Register`

- Adds a register at offset `0x8`.
- GVSoC handles full and partial reads/writes and automatically provides traces.

### 3. Generate a register map

- Describe registers and their bit fields in `regmap.md`.
- Run `regmap-gen` to generate the C++ register-handling code.
- This avoids manually writing many address checks.

## Technical notes

### 1. Why we need to add register map for components

Basically its like defining little "boxes" in the components so that it can check to see if it has recieved any data and thus able to respond accurately. This not only give the component a way to properly stores its own internal data, its also faciliates a methodogical way of how components can communication with the cpu and other components by writing/reading values from specific internal registers.

A special case is for boolean signals. In this case, we need not overcomplicate with register mapping - instead, we can program the component to handle it directly at the port.

For a compute component such as a dot-product accelerator, the register map could be:

| Offset | Purpose                          |
|--------|-----------------------------------|
| 0x00   | Operand A                        |
| 0x04   | Operand B                        |
| 0x08   | Control register, such as START  |
| 0x0C   | Status register, such as DONE    |
| 0x10   | Computation result               |

The CPU writes operands and commands to the appropriate addresses. The component checks the address of each request and determines what the received value means. After computation, the CPU can read the result and status registers.

Therefore, registers in a compute component are not only storage boxes. They form the component’s software-visible interface the component whether a value is input data, a command, or something else.

RAM is different: its many addresses mainly store ordinary data. A compute component normally exposes only a small number of special-purpose registers, and reading or writing them may cause hardware behaviour such as starting a computation or reporting its completion.

Not every component requires a register map. It is mainly required when the CPU needs to control or access the component using addresses.

### 1.5. Register mapping and internal register

Register mapping is like a phone book for the cpu to know what to do. Internal registers also have register mapping for its address. Its usage is to provide a more organized way to store internal data for the component instead of continuing to use register mapping (you can, but not recommended for larger scale projects).

**Important**: 

1. Since internal registers are basically a part of a component instead of a stand-alone component in the wider system with its on ports, interacting with cpu, etc (intuitively it is controlled "internally" by the component), it is not needed to be defined as a sub-component in my_system.py.
2. Also, an interal register only needs to be initializes in .cpp files, unlike ports and traces, since again, it is an internal component without cross-component boundary. So, no need to define it in the .py script. (Intuition: .py is to describe the "shell", not the internal components.)

### This project allow us to understand deeper how gvsoc works:

- my_system.py describes the system, target, and its component tree. It also defines the interconnection between the components via the interconnects or direct.
- .py files describe the "shell" of the components, including its input/ouput port and linking it to the .cpp file
- .cpp files does the main job: it defines internal sub-components of the components (ports, internal registers, etc), write initialization function for that component, as well as write handling functions to handle requests.
- main.c: this is where we write the main code for the program to be simulated. The rest of the files is strictly for describing the "shape" and "behaviour" of the system. This file is where we specific what the system executes (for instance, printf functions are actually read, assigning varialbes are actually write, etc)

### 2. Adding register map manually

The manual way to register-map the internal components of a componet is via its request handling function, after we have define and "register" those sub-components in the component class's function.

### 3. Adding register map automatically using regmap

## Success messages

Normal run with `make run`:

```txt
Hit value
Hello, got 0x11227744 at 0x20000008
```

Tracing run with `make run runner_args="--trace=my_comp/my_reg --trace-level=trace"`

```txt
/test/test run --trace=my_comp/my_reg --trace-level=trace
1620000: 162: [/soc/my_comp/my_reg/trace     ] Modified register (value: 0x11223344)
1650000: 165: [/soc/my_comp/my_reg/trace     ] Modified register (value: 0x11227744)
Hit value
Hello, got 0x11227744 at 0x20000008
```

## Error logs
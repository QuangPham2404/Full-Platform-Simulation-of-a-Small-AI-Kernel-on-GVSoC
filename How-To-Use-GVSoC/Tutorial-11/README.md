# Tutorial 11 Notes

# Summary

## Continuation

This tutorial picks up from tutorial 4.

## What it does

This tutorial focuses on adding a ISA (instruction) to the core ISS (basically the simulation of the cpu core). We are going add our own custom instruction to the ISA (called `my_instr`) where

```txt
my_instr x1, x2, x3 means x3 = x1 + 2*x2
```

This tutorial shifts from the previous ones: Previously we focuses on the system revolving around the core (registers mapping, ico lantency, component tree - SoC development, etc). Now we focus on the cpu cores themselves.

# Technical Notes

## Some technical terms

| Term | Meaning in simple wording |
| --- | --- |
| ISA (Instruction Set Architecture) | Basically the "cookbook" for the cpu. It defines the instructions that the cpu can carry out. If an instruction is not available in the ISA, then the cpu just can't "understand" it and cannot execute it. Vice versa, if an instruction is available, but the hardware does not support it, the instruction also cannot be executed. A note that each ISA have its own syntax of how it defines the instruction. |
| ISS (Instruction Set Simulator) | Basically the simluated software of the cpu. It mimics how a cpu responds to the instructions sent to it and allow edit of the ISA |
| Instruction encoding | Insturctions in binary (machine language) |
| Instruction decoding | When the CPU recieves the instructions in binary, it must translate it and refer to the ISA |

How the cpu works: (1) Instruction encoded and stored in register --> (2) Fetch instruction from registers (machine language) --> (3) ISS - decode --> (4) ISS - execution --> (5) Move on to next state (either the same instruction or the next one(s))

## How to add an ISA instruction to ISS in GVSoC

In order to add an ISA instuction we need to do 2 thing:

| Step | Explanation |
| --- | ---|
Instruction definition | Adding the `my_instr` definition in to the ISA itself (similarly adding a receipe to the cpu's cookbook). The aim is that ISS can recognize and decode `my_instr` from machine language |
| Instruction implementation | After adding the instruction so that it can be recognized, our next step is to tell the ISS what to actually do after it decodes `my_instr` |

## How instructions are defined in RISC-V

**General knowledge on how the instructions are defined by ISAs**

Each ISA have their own way of defining instructions, below is some key points:

| No. | Mechanism | Explanation - example |
| --- | --- | --- |
| 1 | Fixed-size vs variable-size intructions | ARM, RISC-V, etc instructions all have the same size. This makes it easier to encode, but more code is required so it takes more mem. Meanwhile, x86, etc instructions can have variable size. This makes it harder to decode but less code so save mem. Some ISA also allow a mix of both |
| 2 | Load/store architecture vs Register-mem architecture | ARM, RISC-V instructions cannot read directly from RAM, it must use instructinos `load` and `store` to move data from RAM to cpu register, do artihemtic, then write back to RAM. Meanwhile x86 instructinos are able to access RAM directly |
| 3 | Instrucion format, encoding | Different ISA have different syntax (e.g. with part in the machine code means what) | 
| 4 | Number of operands allowed in each instruction | |
| 5 | Insutrctino complexity | RISC is single step instructions, while others allow each instructino to have multiple steps |

**How instructions are defined in RISC-V**

In RISC-V, one of the format for an ISA insturction is "R-type" (denoted as `Format R` when we are declaring a new instruction). Each instructino in RISC-V concerns about 3 main things:

1. Which input registers
2. Which operation
3. Which output registers

| Critrea |  Details | 
| --- | --- |
| Insturction size| fixed sized, 32 bits | 
| Instruction (machine language - binary) breakdown | `func7-rs2-rs1-func3-rd-opcode` Where `func7` and `func3` are the more detailed insturctnios on what to do; `rs1`. `rs2`, and `rd` are the address or at least referece to the register for x1, x2, and x3 (destination register); `opcode` is the category of the instruction (labelling) |

More about `funct` and `opcode`: imagine the ISA as a cookbook. In that cook book there are sections, then subsections, and sub-subsections. `opcode` and `funct` are the ones that narrows down to the specific instruction in which sections/sub-sections.

For instance:

```txt
opcode = 0110011
Register-register arithmetic/logic


funct3 = 000
Narrows down to ADD/SUB family

funct7 = 0000000
ADD
rs1 + rs2

funct7 = 0100000
SUB
rs1 − rs2
```

More example:

```txt
Instruction	opcode	funct3	funct7	Operation
ADD	0110011	000	0000000	\(rd=rs1+rs2\)
SUB	0110011	000	0100000	\(rd=rs1-rs2\)
AND	0110011	111	0000000	Bitwise AND
OR	0110011	110	0000000	Bitwise OR
```

**Instruction definition in RISC-V**

We will need to edit the source code in gvsoc repo to add the new instuction. Go to `/gvsoc/core/models/cpu/iss/isa_gen/isa_riscv_gen.py` (this is basically the RISC-V ISA cookbook for GVSoC) and add the following:

```py
Instr('my_instr', Format_R,
      '0000010 ----- ----- 110 ----- 0110011'),
```

Where:

| Field | entry | What it means |
| --- | --- | --- |
|funct7	| 0000010 | Custom instruction identifier
| rs2 | ----- | Any input register
| rs1 | ----- | Any input register
| funct3 | 110 | Custom instruction identifier
| rd | ----- |Any output register
| opcode | 0110011 | Register-register arithmetic/logic

More about the structure of RISC-V ISA, which is like a cookbook with different chapters, and each of these chapters are 2 base ISA and the extensions that can be implemented for a cpu design. They are as follows:

| Base ISA / extension | What it does |
| --- | --- |
| Base ISA: RV32I (integer instructions) | Base 32-bit instructions set |
| Base ISA: RV64I (integer instructions) | Base 64-bit insturctinos (more mem but more capablilty per instruction) |
| The extensions | M (multiplication), A (atomic), F & D (for precision), C (squeezed) |

For instance, previous examples uses ISA `rv64imafdc` which an ISA that uses the 64-bit base with addtional extensions. We can combine `base ISA + extensions` based on our design.

In this example we focus on the 32-bit base ISA with integer extension (`rv32i`). Note that in the ISA file, you may see `rv32m`, etc. Note that this is gvsoc's logic of how it catgorize and consturct its ISA (which is mentioned above). Don't be confused betweent the two.

**Instruction implementation**

Now we specifically define how the instruction are imlpemented after we have defined it. We are going to do it in the header file for the `rv32i` base ISA: `/gvsoc/core/models/cpu/iss/include/isa/rv32i.hpp`

Add a new function (corresponding to a new instruction defined in the .py file above):

```cpp
static inline iss_reg_t my_instr_exec(Iss *iss, iss_insn_t *insn, iss_reg_t pc)
{
    REG_SET(0, REG_GET(0) + 2*REG_GET(1));
    return iss_insn_next(iss, insn, pc);
}
```

| Parameter | Simple explanation | Technical explanation |
| --- | --- | --- |
| Iss *iss	| Which simulated CPU?	| Pointer to the ISS instance, containing or providing access to CPU simulation state |
iss_insn_t *insn	| Which instruction is being executed?	| Pointer to the decoded instruction representation, including operand information |
iss_reg_t pc	| Where are we in the program?	| Current program counter (instruction address)

Here note that we can intuitively see the implementation of our custom instruction: `r3 = r1 + 2*r2`. We can use `REG` directly. How the program find the register, etc are internal gvsoc workings, as desginers we need not care about that for now.

## The assembly to test the new instruction

### What is assembly?

The assembly language is a low-level programming language that allows programmers to talk directly to to the CPU. It sits below other programming languages (like Python, C, C++, etc), closer to the machine itself. Therefore, it offers more direct control over the cpu (e.g. talk to registers, call direct instructions, etc)

```md
# For C
c = a + b; // "+" operand needs a compiler to translate that into an add instruction for the CPU
 
# For Assembly
add x3, x1, x2 // Direct request for CPU add instruction
```

In C/C++, you describe a computation and the *compiler* is in charge of doing a bunch of stuff to turn that to assembly (in most cases) and then to machine language. For Assembly, an *assembler* do a *direct* translation from the code to machine language.  

### Why do we need an assembly file here?

We have defined the definition and implementation of the custom instruction `my_instr` in the ISS's ISA file and rv32i.hpp file respectively then rebuild the board. That means our board's CPU now have our new instruction in its cookbook. The remaining problem is that C doesn't know what function that is used to call that instruction (like "+" calls `add`). So, we need a custom assembly file to define a C function that calls this instruction to test - later we can call in `main.c` directly: 

```c
uint32_t c = my_instr(a, b);
```

# Success Message

Run with normal `make run runner_args="--trace=insn"`, and note the line where it shows our custom instruction `my_instr` is being executed by the CPU:

```txt

1630000: 163: [/soc/host/insn] _stext:0 M 0000000000000b20 my_instr a0, a0, a1 a0=0000000000000019 a0:0000000000000005 a1:000000000000000a
5 + 2 * 10 -> 25
```

# Error Logs
# Tutorial 13 Notes

# Summary

## Continuation

The idea continue from tutorial 11-12, but system-wise its from tutorial 4.

## What it does

Tutorial 11 adds a custom instruction to an existing ISA, this tutorial create a whole new cutsom ISA extension. They achieve the same objective of adding a new custom instruction, but this way of creating a new ISA extension is cleaner since it refrains from touching the already working ISAs in gvsoc. Intuitively, its also more "modular" and better engineering practice.

# Technical Notes

## How do we create a custom ISA in GVSoC

The ISA are defined in `core/models/cpu/iss/isa_gen/isa_riscv_gen.py`. If we want to create a new custom ISA, we just add a new class, similar to the existing ones `rv32i`, `rv32a`, etc. 

Note that by "creating a custom IRA" it means either creating a whole new IRA (not possible) or creating a new ISA extension to add on to the base ISA (what we are doing instead of adding the new instruction directly to the base ISA)

Afer we create a new ISA, we update our custom CPU to use add the new ISA extension with the custom instruction `my_instr`.

# Success Message

No trace runs for this tutorial. 

# Error Logs
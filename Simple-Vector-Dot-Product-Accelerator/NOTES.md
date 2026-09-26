# Milestone Project 1 Notes

## Technical notes

### 1. The overall structure of a gvsoc project

- my_system.py: 
  - Define target (target = clock + SoC)
  - Define component tree for project: instantiating components and connect them
- dot_product_component.py
  - Describe the "shell" of the custome component: defining the ports
- dot_product_component.cpp
  - Describe the behaviour of the custom component: registering the ports, define call-back functions for ports, etc
- dot_product_data_types.hpp
  - Describe any custom data type used for the projects
- main.c
  - The main program to be compiled and simulate using the SoC

### 2. How to create ports and connect to interconnect (Tutorial 2)

### 3. Register mapping for components (Tutorial 5)

## Implementation notes

### 1. Software side vs Hardware side

- Software side: main.c and the header files
- Hardware side: the remaining of the project

### 2. Writing the main.c

- Create the vectors A and B
- Write A and B to the DCP's mapped register interface
- Tell DCP to compute
- Read result from DCP
- printf(result) & write result to register address

This follows the flow of the project where cpu organizes and the dcp calculates

### How to implement

- Key 1: we will use register mapping for DCP to store the inputs outputs while handling the I/O request.

```
                        DCP
                         │
        ┌────────────────┼─────────────────┐
        ▼                ▼                 ▼
      A regs           B regs          CTRL / RESULT
```

  In register mapping format:

```
DPC base address = 0x20000000

Offset     Register
--------------------------------
0x00       A[0]
0x04       A[1]
0x08       A[2]
0x0C       A[3]

0x10       B[0]
0x14       B[1]
0x18       B[2]
0x1C       B[3]

0x20       CONTROL
0x24       STATUS
0x28       RESULT
```
  Here the CONTROL register is to store the "control" signal. We will set this to 1 in our `main.c` and when DCP read this it will execute the computation.

- Key 2: A and B in `main.c` can be defined as unsigne integer list

```c
uint32_t A[4] = {1, 2, 3, 4};
uint32_t B[4] = {5, 6, 7, 8};
```

- Key 3: Combining Key 1 and Key 2 we can figure a way to correctly write the input data into the DCP in `main.c` by doing many write request, each one calling the callback function once:

```c
*(uint32_t *)0x20000000 = A[0];
*(uint32_t *)0x20000004 = A[1];
*(uint32_t *)0x20000008 = A[2];
*(uint32_t *)0x2000000C = A[3];

*(uint32_t *)0x20000010 = B[0];
*(uint32_t *)0x20000014 = B[1];
*(uint32_t *)0x20000018 = B[2];
*(uint32_t *)0x2000001C = B[3];
```

- Key 4: With this, we only need 1 I/O port and 1 corresponding call-back function to handle this.

- Key 5: The cpu class is already defined by the Component library in gvsoc. And note that when we write `*(uint32_t *)0x20000000 = A[0];` thats a WRITE I/O request and `printf()` thats a READ I/O request. Therfore, 1 callback function can handle this.

## Error loggings

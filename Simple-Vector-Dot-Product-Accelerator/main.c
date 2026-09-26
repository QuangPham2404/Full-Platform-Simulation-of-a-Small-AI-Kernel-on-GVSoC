#include <stdio.h>
#include <stdint.h>

/*
The register mapping for DCP is:

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

*/


// DPC base address
#define DPC_BASE_ADDR 0x20000000

// DPC register addresses
#define DPC_A0      (DPC_BASE_ADDR + 0x00)
#define DPC_A1      (DPC_BASE_ADDR + 0x04)
#define DPC_A2      (DPC_BASE_ADDR + 0x08)
#define DPC_A3      (DPC_BASE_ADDR + 0x0C)

#define DPC_B0      (DPC_BASE_ADDR + 0x10)
#define DPC_B1      (DPC_BASE_ADDR + 0x14)
#define DPC_B2      (DPC_BASE_ADDR + 0x18)
#define DPC_B3      (DPC_BASE_ADDR + 0x1C)

#define DPC_CONTROL (DPC_BASE_ADDR + 0x20)
#define DPC_STATUS  (DPC_BASE_ADDR + 0x24)
#define DPC_RESULT  (DPC_BASE_ADDR + 0x28)


// Helper macros for accessing memory-mapped DPC registers
#define REG_WRITE(addr, value) \
    (*(volatile uint32_t *)(addr) = (value))

#define REG_READ(addr) \
    (*(volatile uint32_t *)(addr))


int main()
{
    // Define input vectors
    uint32_t A[4] = {1, 2, 3, 4};
    uint32_t B[4] = {5, 6, 7, 8};

    // Write vector A into DPC registers
    REG_WRITE(DPC_A0, A[0]);
    REG_WRITE(DPC_A1, A[1]);
    REG_WRITE(DPC_A2, A[2]);
    REG_WRITE(DPC_A3, A[3]);

    // Write vector B into DPC registers
    REG_WRITE(DPC_B0, B[0]);
    REG_WRITE(DPC_B1, B[1]);
    REG_WRITE(DPC_B2, B[2]);
    REG_WRITE(DPC_B3, B[3]);

    // Tell DPC to start computation.
    // CONTROL = 1 means "start".
    REG_WRITE(DPC_CONTROL, 1);

    // Wait until the DPC reports that computation is complete.
    // STATUS = 1 means "done".
    while (REG_READ(DPC_STATUS) != 1)
    {
        // Wait for computation to complete
    }

    // Read final dot-product result
    uint32_t result = REG_READ(DPC_RESULT);

    // Print result
    printf("[CPU] Dot-product completed successfully.\n");
    printf("[CPU] Result = %u\n", result);

    return 0;
}
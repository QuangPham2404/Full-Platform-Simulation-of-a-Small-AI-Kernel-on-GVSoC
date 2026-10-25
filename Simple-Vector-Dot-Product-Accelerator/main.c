#include <stdio.h>
#include <stdint.h>

#define VECTOR_SIZE 16
#define DPC_BASE_ADDR 0x20000000U

// 16 contiguous 32-bit input registers for each vector.
#define DPC_A(i)    (DPC_BASE_ADDR + 0x00U + 4U * (i))
#define DPC_B(i)    (DPC_BASE_ADDR + 0x40U + 4U * (i))
#define DPC_CONTROL (DPC_BASE_ADDR + 0x80U)
#define DPC_STATUS  (DPC_BASE_ADDR + 0x84U)
#define DPC_RESULT  (DPC_BASE_ADDR + 0x88U)

#define REG_WRITE(addr, value) (*(volatile uint32_t *)(uintptr_t)(addr) = (value))
#define REG_READ(addr)         (*(volatile uint32_t *)(uintptr_t)(addr))

int main(void)
{
    uint32_t A[VECTOR_SIZE] = {
        1, 2, 3, 4, 5, 6, 7, 8,
        9, 10, 11, 12, 13, 14, 15, 16
    };
    uint32_t B[VECTOR_SIZE] = {
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1
    };

    for (uint32_t i = 0; i < VECTOR_SIZE; i++)
    {
        REG_WRITE(DPC_A(i), A[i]);
        REG_WRITE(DPC_B(i), B[i]);
    }

    REG_WRITE(DPC_CONTROL, 1);

    while (REG_READ(DPC_STATUS) != 1)
    {
        // Wait for DPC completion.
    }

    uint32_t result = REG_READ(DPC_RESULT);
    printf("[CPU] Dot-product completed successfully.\n");
    printf("[CPU] Result = %u\n", result);
    return 0;
}

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

// GNU assembler .insn avoids requiring a compiler with a custom mnemonic.
static inline uint32_t mac(uint32_t acc, uint32_t a, uint32_t b)
{
    uintptr_t rd = acc;
    __asm__ volatile (".insn r 0x0b, 0, 0, %0, %1, %2"
                      : "+r"(rd) : "r"((uintptr_t)a), "r"((uintptr_t)b));
    return (uint32_t)rd;
}

// Consecutive dependent MACs, then an independent instruction before a use.
// Keep this sequence together so compiler scheduling cannot hide the timing.
static uint32_t mac_timing_probe(void)
{
    uintptr_t acc = 0, independent;
    const uintptr_t one = 1;
    __asm__ volatile (
        ".balign 4\n"
        ".insn r 0x0b, 0, 0, %0, %2, %2\n"
        ".insn r 0x0b, 0, 0, %0, %2, %2\n"
        "addi %1, zero, 7\n"
        ".insn r 0x0b, 0, 0, %0, %2, %2\n"
        : "+&r"(acc), "=&r"(independent) : "r"(one));
    return (uint32_t)acc + (uint32_t)independent;
}

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
    uint32_t cpu_result = 0;
    for (unsigned i = 0; i < VECTOR_SIZE; ++i)
        cpu_result = mac(cpu_result, A[i], B[i]);
    printf("[CPU] Custom MAC result = %u\n", cpu_result);
    if (result != 136 || cpu_result != result)
    {
        printf("[CPU] FAIL: CPU/DPC result mismatch\n");
        return 1;
    }
    if (mac_timing_probe() != 10 || mac(UINT32_MAX, 2, 3) != 5)
    {
        printf("[CPU] FAIL: MAC timing probe / wraparound check\n");
        return 1;
    }
    printf("[CPU] MAC dependency probe and wraparound check passed.\n");
    printf("[CPU] Dot-product completed successfully.\n");
    printf("[CPU] Result = %u\n", result);
    return 0;
}

#include <stdio.h>
#include <stdint.h>

int main()
{
    // These are write request to the internal register
    *(uint32_t *)0x20000008 = 0x11223344;
    *(uint8_t *)0x20000009 = 0x77;

    // Debug print message - basically read request
    printf("Hello, got 0x%x at 0x20000008\n", *(uint32_t *)0x20000008);
}
// Recall the "definition" of our custom ISA instruction
// Instr('my_instr', Format_R, '0000010 ----- ----- 110 ----- 0110011')
// in hex its: 0x04b56533 (after we choose the specific register address to fill in for the "---...---" parts)
// Which are bit varaible for the registers address in our instruction. We can choose any of the cpu registers to fill in

.global my_instr // Make the file accessible by other files

my_instr: // Define the function name
    .word 0x04b56533 // Point to our ISA instruction
    jr ra // Intuition: after this, go back to our c program
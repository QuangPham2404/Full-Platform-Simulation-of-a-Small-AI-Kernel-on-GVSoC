# Tutorial 0 Notes

## Notes

**General Notes**

Directory path: `/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/0_how_to_build_a_system_from_scratch`

**Key words and Concepts**

- Interconnect: collection of paths and switches that allow components in a chip (e.g. core, memeory, I/O, etc) to communcate with each other.
- Target: is a plattform. A plattform includes: clock + SoC (CPU with defined ISA, RAM, I/O, etc)

**Python concepts**

- Class inheritance: Use `__init__` for children class if you need to add parameters to the children class that the parents might not have. In this case, this overides the parent's class `__init__`, so since we still need to inherit stuff from parent, we need the line `super()__init__`. Otherwise we don't need to use an extra `__init__` for the children class at all. Below is an example:

```python
# YOU DON'T NEED SUPER()
class Animal:
    def __init__(self, name):
        self.name = name
    def speak(self):
        print(f"{self.name} makes a sound")

class Cat(Animal):
    pass   # <-- no __init__ defined at all!

c = Cat("Whiskers")
c.speak()   # "Whiskers makes a sound"


# YOU NEED SUPER()
class Animal:
    def __init__(self, name):
        self.name = name
    def speak(self):
        print(f"{self.name} makes a sound")

class Dog(Animal):
    def __init__(self, name, breed):
        super().__init__(name)   # let Animal set self.name
        self.breed = breed       # Dog adds its own extra setup

d = Dog("Rex", "Labrador")
d.speak()          # "Rex makes a sound"  <- came from Animal's logic
print(d.breed)     # "Labrador"           <- came from Dog's own logic
```

## Success message

Normal run with `make run`

```txt
/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/0_how_to_build_a_system_from_scratch/build/install/bin/gvrun --target-dir=/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/0_how_to_build_a_system_from_scratch --target-dir=/home/stvn/UREKA/gvsoc/install/generators --target=my_system --work-dir=/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/0_how_to_build_a_system_from_scratch/build/work --parameter binary=/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/0_how_to_build_a_system_from_scratch/build/test/test run
Hello
```

Tracing run with `make run runner_args="--trace=insn"`

```txt
30000: 3: [/soc/host/insn                ] _start:5                         M 0000000000000c04 auipc               sp, 0x0           sp=0000000000000c04
40000: 4: [/soc/host/insn                ] _start:5                         M 0000000000000c08 addi                sp, sp, fffffffffffffe7c  sp=0000000000000a80  sp:0000000000000c04
50000: 5: [/soc/host/insn                ] _start:8                         M 0000000000000c0c auipc               t0, 0x0                   t0=0000000000000c0c
60000: 6: [/soc/host/insn                ] _start:8                         M 0000000000000c10 addi                t0, t0, 1a                t0=0000000000000c26  t0:0000000000000c0c
70000: 7: [/soc/host/insn                ] _start:9                         M 0000000000000c14 csrrw               0, t0, mtvec              t0:0000000000000c26
80000: 8: [/soc/host/insn                ] _start:12                        M 0000000000000c18 auipc               t0, 0x0                   t0=0000000000000c18
90000: 9: [/soc/host/insn                ] _start:12                        M 0000000000000c1c addi                t0, t0, ffffffffffffffc4  t0=0000000000000bdc  t0:0000000000000c18
100000: 10: [/soc/host/insn                ] _start:13                        M 0000000000000c20 c.li                a0, 0, 0                  a0=0000000000000000
110000: 11: [/soc/host/insn                ] _start:14                        M 0000000000000c22 jalr                ra, t0, 0                 ra=0000000000000c26  t0:0000000000000bdc
130000: 13: [/soc/host/insn                ] __init_start:55                  M 0000000000000bdc c.addi              sp, sp, fffffffffffffff0  sp=0000000000000a70  sp:0000000000000a80
140000: 14: [/soc/host/insn                ] __init_start:55                  M 0000000000000bde c.sdsp              s0, 0(sp)                 s0:0000000057575757  sp:0000000000000a70  PA:0000000000000a70
150000: 15: [/soc/host/insn                ] __init_start:55                  M 0000000000000be0 c.sdsp              ra, 8(sp)                 ra:0000000000000c26  sp:0000000000000a70  PA:0000000000000a78
160000: 16: [/soc/host/insn                ] __init_start:16                  M 0000000000000be2 addi                s0, 0, 10                 s0=0000000000000010
170000: 17: [/soc/host/insn                ] __init_start:56                  M 0000000000000be6 jal                 ra, ffffffffffffff2a      ra=0000000000000bea
190000: 19: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b10 c.lui               a2, 0x1000                a2=0000000000001000
200000: 20: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b12 c.lui               a1, 0x1000                a1=0000000000001000
210000: 21: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b14 addi                a5, a2, fffffffffffffa80  a5=0000000000000a80  a2:0000000000001000
220000: 22: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b18 addi                a1, a1, fffffffffffffb10  a1=0000000000000b10  a1:0000000000001000
230000: 23: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b1c beq                 a5, a1, 96                a5:0000000000000a80  a1:0000000000000b10
240000: 24: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b20 addi                a4, a1, fffffffffffffffc  a4=0000000000000b0c  a1:0000000000000b10
250000: 25: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b24 c.sub               a4, a4, a5                a4=000000000000008c  a4:0000000000000b0c  a5:0000000000000a80
260000: 26: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b26 srli                a3, a5, 0x2               a3=00000000000002a0  a5:0000000000000a80
270000: 27: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b2a c.li                a0, 0, 14                 a0=0000000000000014
280000: 28: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b2c srli                a6, a4, 0x2               a6=0000000000000023  a4:000000000000008c
290000: 29: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b30 c.andi              a3, a3, 1                 a3=0000000000000000  a3:00000000000002a0
300000: 30: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b32 bgeu                a0, a4, 82                a0:0000000000000014  a4:000000000000008c
310000: 31: [/soc/host/insn                ] __init_bss:39                    M 0000000000000b36 addi                a4, a2, fffffffffffffa80  a4=0000000000000a80  a2:0000000000001000
320000: 32: [/soc/host/insn                ] __init_bss:39                    M 0000000000000b3a c.beqz              a3, 0, c                  a3:0000000000000000
350000: 35: [/soc/host/insn                ] __init_bss:39                    M 0000000000000b46 xori                a0, a3, 1                 a0=0000000000000001  a3:0000000000000000
360000: 36: [/soc/host/insn                ] __init_bss:39                    M 0000000000000b4a c.add               a0, a0, a6                a0=0000000000000024  a0:0000000000000001  a6:0000000000000023
370000: 37: [/soc/host/insn                ] __init_bss:39                    M 0000000000000b4c slli                a2, a3, 0x2               a2=0000000000000000  a3:0000000000000000
380000: 38: [/soc/host/insn                ] __init_bss:39                    M 0000000000000b50 srli                a3, a0, 0x1               a3=0000000000000012  a0:0000000000000024
390000: 39: [/soc/host/insn                ] __init_bss:39                    M 0000000000000b54 c.add               a5, a5, a2                a5=0000000000000a80  a5:0000000000000a80  a2:0000000000000000
400000: 40: [/soc/host/insn                ] __init_bss:39                    M 0000000000000b56 c.slli              a3, a3, 0x3               a3=0000000000000090  a3:0000000000000012
410000: 41: [/soc/host/insn                ] __init_bss:39                    M 0000000000000b58 c.add               a3, a3, a5                a3=0000000000000b10  a3:0000000000000090  a5:0000000000000a80
420000: 42: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000a80  PA:0000000000000a80
430000: 43: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000a88  a5:0000000000000a80
440000: 44: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000a88  a3:0000000000000b10
470000: 47: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000a88  PA:0000000000000a88
480000: 48: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000a90  a5:0000000000000a88
490000: 49: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000a90  a3:0000000000000b10
520000: 52: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000a90  PA:0000000000000a90
530000: 53: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000a98  a5:0000000000000a90
540000: 54: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000a98  a3:0000000000000b10
570000: 57: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000a98  PA:0000000000000a98
580000: 58: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000aa0  a5:0000000000000a98
590000: 59: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000aa0  a3:0000000000000b10
620000: 62: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000aa0  PA:0000000000000aa0
630000: 63: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000aa8  a5:0000000000000aa0
640000: 64: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000aa8  a3:0000000000000b10
670000: 67: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000aa8  PA:0000000000000aa8
680000: 68: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000ab0  a5:0000000000000aa8
690000: 69: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000ab0  a3:0000000000000b10
720000: 72: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000ab0  PA:0000000000000ab0
730000: 73: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000ab8  a5:0000000000000ab0
740000: 74: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000ab8  a3:0000000000000b10
770000: 77: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000ab8  PA:0000000000000ab8
780000: 78: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000ac0  a5:0000000000000ab8
790000: 79: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000ac0  a3:0000000000000b10
820000: 82: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000ac0  PA:0000000000000ac0
830000: 83: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000ac8  a5:0000000000000ac0
840000: 84: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000ac8  a3:0000000000000b10
870000: 87: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000ac8  PA:0000000000000ac8
880000: 88: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000ad0  a5:0000000000000ac8
890000: 89: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000ad0  a3:0000000000000b10
920000: 92: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000ad0  PA:0000000000000ad0
930000: 93: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000ad8  a5:0000000000000ad0
940000: 94: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000ad8  a3:0000000000000b10
970000: 97: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000ad8  PA:0000000000000ad8
980000: 98: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000ae0  a5:0000000000000ad8
990000: 99: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000ae0  a3:0000000000000b10
1020000: 102: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000ae0  PA:0000000000000ae0
1030000: 103: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000ae8  a5:0000000000000ae0
1040000: 104: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000ae8  a3:0000000000000b10
1070000: 107: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000ae8  PA:0000000000000ae8
1080000: 108: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000af0  a5:0000000000000ae8
1090000: 109: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000af0  a3:0000000000000b10
1120000: 112: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000af0  PA:0000000000000af0
1130000: 113: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000af8  a5:0000000000000af0
1140000: 114: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000af8  a3:0000000000000b10
1170000: 117: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000af8  PA:0000000000000af8
1180000: 118: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000b00  a5:0000000000000af8
1190000: 119: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000b00  a3:0000000000000b10
1220000: 122: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000b00  PA:0000000000000b00
1230000: 123: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000b08  a5:0000000000000b00
1240000: 124: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000b08  a3:0000000000000b10
1270000: 127: [/soc/host/insn                ] __init_bss:44                    M 0000000000000b5a sd                  0, 0(a5)                  a5:0000000000000b08  PA:0000000000000b08
1280000: 128: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b5e c.addi              a5, a5, 8                 a5=0000000000000b10  a5:0000000000000b08
1290000: 129: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b60 bne                 a5, a3, fffffffffffffffa  a5:0000000000000b10  a3:0000000000000b10
1300000: 130: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b64 andi                a5, a0, 1                 a5=0000000000000000  a0:0000000000000024
1310000: 131: [/soc/host/insn                ] __init_bss:42                    M 0000000000000b68 c.beqz              a5, 0, 4a                 a5:0000000000000000
1340000: 134: [/soc/host/insn                ] __init_bss:46                    M 0000000000000bb2 c.jr                0, ra, 0, 0               ra:0000000000000bea
1360000: 136: [/soc/host/insn                ] __init_start:58                  M 0000000000000bea jal                 ra, 23a                   ra=0000000000000bee
1380000: 138: [/soc/host/insn                ] __mem_alloc_init_all:81          M 0000000000000e24 c.lui               a5, 0x3000                a5=0000000000003000
1390000: 139: [/soc/host/insn                ] __mem_alloc_init_all:81          M 0000000000000e26 addi                a5, a5, fffffffffffffb97  a5=0000000000002b97  a5:0000000000003000
1400000: 140: [/soc/host/insn                ] __mem_alloc_init_all:81          M 0000000000000e2a c.andi              a5, a5, fffffffffffffff8  a5=0000000000002b90  a5:0000000000002b97
1410000: 141: [/soc/host/insn                ] __mem_alloc_init_all:82          M 0000000000000e2c c.lui               a3, 0x1000                a3=0000000000001000
1420000: 142: [/soc/host/insn                ] __mem_alloc_init_all:83          M 0000000000000e2e lui                 a4, 0x100000              a4=0000000000100000
1430000: 143: [/soc/host/insn                ] __mem_alloc_init_all:82          M 0000000000000e32 sd                  a5, fffffffffffffb00(a3)  a5:0000000000002b90  a3:0000000000001000  PA:0000000000000b00
1440000: 144: [/soc/host/insn                ] __mem_alloc_init_all:83          M 0000000000000e36 sub                 a3, a4, a5                a3=00000000000fd470  a4:0000000000100000  a5:0000000000002b90
1450000: 145: [/soc/host/insn                ] __mem_alloc_init_all:84          M 0000000000000e3a beq                 a4, a5, a                 a4:0000000000100000  a5:0000000000002b90
1460000: 146: [/soc/host/insn                ] __mem_alloc_init_all:86          M 0000000000000e3e c.sw                a3, 0(a5)                 a3:00000000000fd470  a5:0000000000002b90  PA:0000000000002b90
1470000: 147: [/soc/host/insn                ] __mem_alloc_init_all:87          M 0000000000000e40 sd                  0, 8(a5)                  a5:0000000000002b90  PA:0000000000002b98
1480000: 148: [/soc/host/insn                ] __mem_alloc_init_all:232         M 0000000000000e44 c.jr                0, ra, 0, 0               ra:0000000000000bee
1500000: 150: [/soc/host/insn                ] __init_start:16                  M 0000000000000bee c.ld                a5, 8(s0)                 a5=0000000000000000  s0:0000000000000010  PA:0000000000000018
1510000: 151: [/soc/host/insn                ] __init_start:16                  M 0000000000000bf0 c.beqz              a5, 0, c                  a5:0000000000000000
1550000: 155: [/soc/host/insn                ] __init_start:62                  M 0000000000000bfc jal                 ra, 1f74                  ra=0000000000000c00
1570000: 157: [/soc/host/insn                ] main:4                           M 0000000000002b70 c.addi              sp, sp, fffffffffffffff0  sp=0000000000000a60  sp:0000000000000a70
1580000: 158: [/soc/host/insn                ] main:5                           M 0000000000002b72 addi                a0, 0, 260                a0=0000000000000260
1590000: 159: [/soc/host/insn                ] main:4                           M 0000000000002b76 c.sdsp              ra, 8(sp)                 ra:0000000000000c00  sp:0000000000000a60  PA:0000000000000a68
1600000: 160: [/soc/host/insn                ] main:5                           M 0000000000002b78 jal                 ra, ffffffffffffe32a      ra=0000000000002b7c
1620000: 162: [/soc/host/insn                ] puts:61                          M 0000000000000ea2 lbu                 a3, 0(a0)                 a3=0000000000000048  a0:0000000000000260  PA:0000000000000260
1630000: 163: [/soc/host/insn                ] puts:39                          M 0000000000000ea6 c.lui               a7, 0x1000                a7=0000000000001000
1640000: 164: [/soc/host/insn                ] puts:130                         M 0000000000000ea8 c.lui               a2, 0x1000                a2=0000000000001000
1650000: 165: [/soc/host/insn                ] puts:39                          M 0000000000000eaa lw                  a5, fffffffffffffb08(a7)  a5=0000000000000000  a7:0000000000001000  PA:0000000000000b08
1660000: 166: [/soc/host/insn                ] puts:57                          M 0000000000000eae c.addi              sp, sp, ffffffffffffffe0  sp=0000000000000a40  sp:0000000000000a60
1670000: 167: [/soc/host/insn                ] puts:130                         M 0000000000000eb0 addi                a2, a2, fffffffffffffa80  a2=0000000000000a80  a2:0000000000001000
1680000: 168: [/soc/host/insn                ] puts:62                          M 0000000000000eb4 c.beqz              a3, 0, 9c                 a3:0000000000000048
1690000: 169: [/soc/host/insn                ] puts:62                          M 0000000000000eb6 c.mv                a6, 0, a0                 a6=0000000000000260  a0:0000000000000260
1700000: 170: [/soc/host/insn                ] puts:62                          M 0000000000000eb8 addi                t3, 0, 80                 t3=0000000000000080
1710000: 171: [/soc/host/insn                ] puts:62                          M 0000000000000ebc c.li                t5, 0, 1                  t5=0000000000000001
1720000: 172: [/soc/host/insn                ] puts:62                          M 0000000000000ebe c.li                t4, 0, a                  t4=000000000000000a
1730000: 173: [/soc/host/insn                ] puts:62                          M 0000000000000ec0 c.j                 0, 12
1750000: 175: [/soc/host/insn                ] puts:40                          M 0000000000000ed2 addiw               a4, a5, 1                 a4=0000000000000001  a5:0000000000000000
1760000: 176: [/soc/host/insn                ] puts:39                          M 0000000000000ed6 add                 a0, a2, a5                a0=0000000000000a80  a2:0000000000000a80  a5:0000000000000000
1770000: 177: [/soc/host/insn                ] puts:39                          M 0000000000000eda sb                  a3, 0(a0)                 a3:0000000000000048  a0:0000000000000a80  PA:0000000000000a80
1780000: 178: [/soc/host/insn                ] puts:40                          M 0000000000000ede sw                  a4, fffffffffffffb08(a7)  a4:0000000000000001  a7:0000000000001000  PA:0000000000000b08
1790000: 179: [/soc/host/insn                ] puts:40                          M 0000000000000ee2 addiw               a0, a5, 2                 a0=0000000000000002  a5:0000000000000000
1800000: 180: [/soc/host/insn                ] puts:44                          M 0000000000000ee6 add                 a1, a2, a4                a1=0000000000000a81  a2:0000000000000a80  a4:0000000000000001
1810000: 181: [/soc/host/insn                ] puts:42                          M 0000000000000eea bne                 a4, t3, ffffffffffffffd8  a4:0000000000000001  t3:0000000000000080
1840000: 184: [/soc/host/insn                ] puts:42                          M 0000000000000ec2 beq                 a3, t4, 2c                a3:0000000000000048  t4:000000000000000a
1850000: 185: [/soc/host/insn                ] puts:61                          M 0000000000000ec6 lbu                 a3, 1(a6)                 a3=0000000000000065  a6:0000000000000260  PA:0000000000000261
1860000: 186: [/soc/host/insn                ] puts:44                          M 0000000000000eca c.mv                t1, 0, a0                 t1=0000000000000002  a0:0000000000000002
1870000: 187: [/soc/host/insn                ] puts:44                          M 0000000000000ecc c.mv                a5, 0, a4                 a5=0000000000000001  a4:0000000000000001
1880000: 188: [/soc/host/insn                ] puts:68                          M 0000000000000ece c.addi              a6, a6, 1                 a6=0000000000000261  a6:0000000000000260
1890000: 189: [/soc/host/insn                ] puts:62                          M 0000000000000ed0 c.beqz              a3, 0, 4c                 a3:0000000000000065
1900000: 190: [/soc/host/insn                ] puts:40                          M 0000000000000ed2 addiw               a4, a5, 1                 a4=0000000000000002  a5:0000000000000001
1910000: 191: [/soc/host/insn                ] puts:39                          M 0000000000000ed6 add                 a0, a2, a5                a0=0000000000000a81  a2:0000000000000a80  a5:0000000000000001
1920000: 192: [/soc/host/insn                ] puts:39                          M 0000000000000eda sb                  a3, 0(a0)                 a3:0000000000000065  a0:0000000000000a81  PA:0000000000000a81
1930000: 193: [/soc/host/insn                ] puts:40                          M 0000000000000ede sw                  a4, fffffffffffffb08(a7)  a4:0000000000000002  a7:0000000000001000  PA:0000000000000b08
1940000: 194: [/soc/host/insn                ] puts:40                          M 0000000000000ee2 addiw               a0, a5, 2                 a0=0000000000000003  a5:0000000000000001
1950000: 195: [/soc/host/insn                ] puts:44                          M 0000000000000ee6 add                 a1, a2, a4                a1=0000000000000a82  a2:0000000000000a80  a4:0000000000000002
1960000: 196: [/soc/host/insn                ] puts:42                          M 0000000000000eea bne                 a4, t3, ffffffffffffffd8  a4:0000000000000002  t3:0000000000000080
1990000: 199: [/soc/host/insn                ] puts:42                          M 0000000000000ec2 beq                 a3, t4, 2c                a3:0000000000000065  t4:000000000000000a
2000000: 200: [/soc/host/insn                ] puts:61                          M 0000000000000ec6 lbu                 a3, 1(a6)                 a3=000000000000006c  a6:0000000000000261  PA:0000000000000262
2010000: 201: [/soc/host/insn                ] puts:44                          M 0000000000000eca c.mv                t1, 0, a0                 t1=0000000000000003  a0:0000000000000003
2020000: 202: [/soc/host/insn                ] puts:44                          M 0000000000000ecc c.mv                a5, 0, a4                 a5=0000000000000002  a4:0000000000000002
2030000: 203: [/soc/host/insn                ] puts:68                          M 0000000000000ece c.addi              a6, a6, 1                 a6=0000000000000262  a6:0000000000000261
2040000: 204: [/soc/host/insn                ] puts:62                          M 0000000000000ed0 c.beqz              a3, 0, 4c                 a3:000000000000006c
2050000: 205: [/soc/host/insn                ] puts:40                          M 0000000000000ed2 addiw               a4, a5, 1                 a4=0000000000000003  a5:0000000000000002
2060000: 206: [/soc/host/insn                ] puts:39                          M 0000000000000ed6 add                 a0, a2, a5                a0=0000000000000a82  a2:0000000000000a80  a5:0000000000000002
2070000: 207: [/soc/host/insn                ] puts:39                          M 0000000000000eda sb                  a3, 0(a0)                 a3:000000000000006c  a0:0000000000000a82  PA:0000000000000a82
2080000: 208: [/soc/host/insn                ] puts:40                          M 0000000000000ede sw                  a4, fffffffffffffb08(a7)  a4:0000000000000003  a7:0000000000001000  PA:0000000000000b08
2090000: 209: [/soc/host/insn                ] puts:40                          M 0000000000000ee2 addiw               a0, a5, 2                 a0=0000000000000004  a5:0000000000000002
2100000: 210: [/soc/host/insn                ] puts:44                          M 0000000000000ee6 add                 a1, a2, a4                a1=0000000000000a83  a2:0000000000000a80  a4:0000000000000003
2110000: 211: [/soc/host/insn                ] puts:42                          M 0000000000000eea bne                 a4, t3, ffffffffffffffd8  a4:0000000000000003  t3:0000000000000080
2140000: 214: [/soc/host/insn                ] puts:42                          M 0000000000000ec2 beq                 a3, t4, 2c                a3:000000000000006c  t4:000000000000000a
2150000: 215: [/soc/host/insn                ] puts:61                          M 0000000000000ec6 lbu                 a3, 1(a6)                 a3=000000000000006c  a6:0000000000000262  PA:0000000000000263
2160000: 216: [/soc/host/insn                ] puts:44                          M 0000000000000eca c.mv                t1, 0, a0                 t1=0000000000000004  a0:0000000000000004
2170000: 217: [/soc/host/insn                ] puts:44                          M 0000000000000ecc c.mv                a5, 0, a4                 a5=0000000000000003  a4:0000000000000003
2180000: 218: [/soc/host/insn                ] puts:68                          M 0000000000000ece c.addi              a6, a6, 1                 a6=0000000000000263  a6:0000000000000262
2190000: 219: [/soc/host/insn                ] puts:62                          M 0000000000000ed0 c.beqz              a3, 0, 4c                 a3:000000000000006c
2200000: 220: [/soc/host/insn                ] puts:40                          M 0000000000000ed2 addiw               a4, a5, 1                 a4=0000000000000004  a5:0000000000000003
2210000: 221: [/soc/host/insn                ] puts:39                          M 0000000000000ed6 add                 a0, a2, a5                a0=0000000000000a83  a2:0000000000000a80  a5:0000000000000003
2220000: 222: [/soc/host/insn                ] puts:39                          M 0000000000000eda sb                  a3, 0(a0)                 a3:000000000000006c  a0:0000000000000a83  PA:0000000000000a83
2230000: 223: [/soc/host/insn                ] puts:40                          M 0000000000000ede sw                  a4, fffffffffffffb08(a7)  a4:0000000000000004  a7:0000000000001000  PA:0000000000000b08
2240000: 224: [/soc/host/insn                ] puts:40                          M 0000000000000ee2 addiw               a0, a5, 2                 a0=0000000000000005  a5:0000000000000003
2250000: 225: [/soc/host/insn                ] puts:44                          M 0000000000000ee6 add                 a1, a2, a4                a1=0000000000000a84  a2:0000000000000a80  a4:0000000000000004
2260000: 226: [/soc/host/insn                ] puts:42                          M 0000000000000eea bne                 a4, t3, ffffffffffffffd8  a4:0000000000000004  t3:0000000000000080
2290000: 229: [/soc/host/insn                ] puts:42                          M 0000000000000ec2 beq                 a3, t4, 2c                a3:000000000000006c  t4:000000000000000a
2300000: 230: [/soc/host/insn                ] puts:61                          M 0000000000000ec6 lbu                 a3, 1(a6)                 a3=000000000000006f  a6:0000000000000263  PA:0000000000000264
2310000: 231: [/soc/host/insn                ] puts:44                          M 0000000000000eca c.mv                t1, 0, a0                 t1=0000000000000005  a0:0000000000000005
2320000: 232: [/soc/host/insn                ] puts:44                          M 0000000000000ecc c.mv                a5, 0, a4                 a5=0000000000000004  a4:0000000000000004
2330000: 233: [/soc/host/insn                ] puts:68                          M 0000000000000ece c.addi              a6, a6, 1                 a6=0000000000000264  a6:0000000000000263
2340000: 234: [/soc/host/insn                ] puts:62                          M 0000000000000ed0 c.beqz              a3, 0, 4c                 a3:000000000000006f
2350000: 235: [/soc/host/insn                ] puts:40                          M 0000000000000ed2 addiw               a4, a5, 1                 a4=0000000000000005  a5:0000000000000004
2360000: 236: [/soc/host/insn                ] puts:39                          M 0000000000000ed6 add                 a0, a2, a5                a0=0000000000000a84  a2:0000000000000a80  a5:0000000000000004
2370000: 237: [/soc/host/insn                ] puts:39                          M 0000000000000eda sb                  a3, 0(a0)                 a3:000000000000006f  a0:0000000000000a84  PA:0000000000000a84
2380000: 238: [/soc/host/insn                ] puts:40                          M 0000000000000ede sw                  a4, fffffffffffffb08(a7)  a4:0000000000000005  a7:0000000000001000  PA:0000000000000b08
2390000: 239: [/soc/host/insn                ] puts:40                          M 0000000000000ee2 addiw               a0, a5, 2                 a0=0000000000000006  a5:0000000000000004
2400000: 240: [/soc/host/insn                ] puts:44                          M 0000000000000ee6 add                 a1, a2, a4                a1=0000000000000a85  a2:0000000000000a80  a4:0000000000000005
2410000: 241: [/soc/host/insn                ] puts:42                          M 0000000000000eea bne                 a4, t3, ffffffffffffffd8  a4:0000000000000005  t3:0000000000000080
2440000: 244: [/soc/host/insn                ] puts:42                          M 0000000000000ec2 beq                 a3, t4, 2c                a3:000000000000006f  t4:000000000000000a
2450000: 245: [/soc/host/insn                ] puts:61                          M 0000000000000ec6 lbu                 a3, 1(a6)                 a3=0000000000000000  a6:0000000000000264  PA:0000000000000265
2460000: 246: [/soc/host/insn                ] puts:44                          M 0000000000000eca c.mv                t1, 0, a0                 t1=0000000000000006  a0:0000000000000006
2470000: 247: [/soc/host/insn                ] puts:44                          M 0000000000000ecc c.mv                a5, 0, a4                 a5=0000000000000005  a4:0000000000000005
2480000: 248: [/soc/host/insn                ] puts:68                          M 0000000000000ece c.addi              a6, a6, 1                 a6=0000000000000265  a6:0000000000000264
2490000: 249: [/soc/host/insn                ] puts:62                          M 0000000000000ed0 c.beqz              a3, 0, 4c                 a3:0000000000000000
2520000: 252: [/soc/host/insn                ] puts:39                          M 0000000000000f1c c.li                a5, 0, a                  a5=000000000000000a
2530000: 253: [/soc/host/insn                ] puts:39                          M 0000000000000f1e sb                  a5, 0(a1)                 a5:000000000000000a  a1:0000000000000a85  PA:0000000000000a85
2540000: 254: [/soc/host/insn                ] puts:44                          M 0000000000000f22 add                 a5, a2, t1                a5=0000000000000a86  a2:0000000000000a80  t1:0000000000000006
2550000: 255: [/soc/host/insn                ] puts:44                          M 0000000000000f26 sb                  0, 0(a5)                  a5:0000000000000a86  PA:0000000000000a86
2560000: 256: [/soc/host/insn                ] puts:130                         M 0000000000000f2a c.li                a5, 0, 1                  a5=0000000000000001
2570000: 257: [/soc/host/insn                ] puts:40                          M 0000000000000f2c sw                  a0, fffffffffffffb08(a7)  a0:0000000000000006  a7:0000000000001000  PA:0000000000000b08
2580000: 258: [/soc/host/insn                ] puts:130                         M 0000000000000f30 c.sdsp              a5, 8(sp)                 a5:0000000000000001  sp:0000000000000a40  PA:0000000000000a48
2590000: 259: [/soc/host/insn                ] puts:130                         M 0000000000000f32 c.sdsp              a2, 10(sp)                a2:0000000000000a80  sp:0000000000000a40  PA:0000000000000a50
2600000: 260: [/soc/host/insn                ] puts:130                         M 0000000000000f34 c.sdsp              t1, 18(sp)                t1:0000000000000006  sp:0000000000000a40  PA:0000000000000a58
2610000: 261: [/soc/host/insn                ] puts:77                          M 0000000000000f36 c.li                a0, 0, 5                  a0=0000000000000005
2620000: 262: [/soc/host/insn                ] puts:78                          M 0000000000000f38 c.addi4spn          a1, sp, 8                 a1=0000000000000a48  sp:0000000000000a40
2630000: 263: [/soc/host/insn                ] puts:81                          M 0000000000000f3a slli                0, 0, 0x1f
Hello
2640000: 264: [/soc/host/insn                ] puts:81                          M 0000000000000f3e ebreak              0, 0, 1
2650000: 265: [/soc/host/insn                ] puts:81                          M 0000000000000f42 srai                0, 0, 0x7
2660000: 266: [/soc/host/insn                ] puts:47                          M 0000000000000f46 sw                  0, fffffffffffffb08(a7)   a7:0000000000001000  PA:0000000000000b08
2670000: 267: [/soc/host/insn                ] puts:72                          M 0000000000000f4a c.li                a0, 0, 0                  a0=0000000000000000
2680000: 268: [/soc/host/insn                ] puts:72                          M 0000000000000f4c c.addi16sp          sp, sp, 20                sp=0000000000000a60  sp:0000000000000a40
2690000: 269: [/soc/host/insn                ] puts:72                          M 0000000000000f4e c.jr                0, ra, 0, 0               ra:0000000000002b7c
2710000: 271: [/soc/host/insn                ] main:7                           M 0000000000002b7c c.ldsp              ra, 8(sp)                 ra=0000000000000c00  sp:0000000000000a60  PA:0000000000000a68
2720000: 272: [/soc/host/insn                ] main:7                           M 0000000000002b7e c.li                a0, 0, 0                  a0=0000000000000000
2730000: 273: [/soc/host/insn                ] main:7                           M 0000000000002b80 c.addi              sp, sp, 10                sp=0000000000000a70  sp:0000000000000a60
2740000: 274: [/soc/host/insn                ] main:7                           M 0000000000002b82 c.jr                0, ra, 0, 0               ra:0000000000000c00
2760000: 276: [/soc/host/insn                ] __init_start:64                  M 0000000000000c00 jal                 ra, ffffffffffffffba      ra=0000000000000c04
2780000: 278: [/soc/host/insn                ] __init_stop:49                   M 0000000000000bba c.addi              sp, sp, ffffffffffffffe0  sp=0000000000000a50  sp:0000000000000a70
2790000: 279: [/soc/host/insn                ] __init_stop:49                   M 0000000000000bbc c.sdsp              s0, 10(sp)                s0:0000000000000010  sp:0000000000000a50  PA:0000000000000a60
2800000: 280: [/soc/host/insn                ] __init_stop:25                   M 0000000000000bbe addi                s0, 0, 28                 s0=0000000000000028
2810000: 281: [/soc/host/insn                ] __init_stop:25                   M 0000000000000bc2 c.ld                a5, 8(s0)                 a5=0000000000000000  s0:0000000000000028  PA:0000000000000030
2820000: 282: [/soc/host/insn                ] __init_stop:49                   M 0000000000000bc4 c.sdsp              s1, 8(sp)                 s1:0000000057575757  sp:0000000000000a50  PA:0000000000000a58
2830000: 283: [/soc/host/insn                ] __init_stop:49                   M 0000000000000bc6 c.sdsp              ra, 18(sp)                ra:0000000000000c04  sp:0000000000000a50  PA:0000000000000a68
2840000: 284: [/soc/host/insn                ] __init_stop:49                   M 0000000000000bc8 c.mv                s1, 0, a0                 s1=0000000000000000  a0:0000000000000000
2850000: 285: [/soc/host/insn                ] __init_stop:25                   M 0000000000000bca c.beqz              a5, 0, c                  a5:0000000000000000
2880000: 288: [/soc/host/insn                ] __init_stop:52                   M 0000000000000bd6 c.mv                a0, 0, s1                 a0=0000000000000000  s1:0000000000000000
2890000: 289: [/soc/host/insn                ] __init_stop:52                   M 0000000000000bd8 jal                 ra, 43e                   ra=0000000000000bdc
2910000: 291: [/soc/host/insn                ] exit:101                         M 0000000000001016 lui                 a1, 0x20000               a1=0000000000020000
2920000: 292: [/soc/host/insn                ] exit:101                         M 000000000000101a addi                a1, a1, 23                a1=0000000000020023  a1:0000000000020000
2930000: 293: [/soc/host/insn                ] exit:101                         M 000000000000101e c.bnez              a0, 0, a                  a0:0000000000000000
2940000: 294: [/soc/host/insn                ] exit:101                         M 0000000000001020 lui                 a1, 0x20000               a1=0000000000020000
2950000: 295: [/soc/host/insn                ] exit:101                         M 0000000000001024 addi                a1, a1, 26                a1=0000000000020026  a1:0000000000020000
2960000: 296: [/soc/host/insn                ] exit:77                          M 0000000000001028 c.li                a0, 0, 18                 a0=0000000000000018
2970000: 297: [/soc/host/insn                ] exit:81                          M 000000000000102a slli                0, 0, 0x1f
2980000: 298: [/soc/host/insn                ] exit:81                          M 000000000000102e ebreak              0, 0, 1
```

## Error logs

Error logs are recorded in `ERROR_LOGS.md`

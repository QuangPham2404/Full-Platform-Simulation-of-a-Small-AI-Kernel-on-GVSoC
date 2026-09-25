# Tutorial 8 Notes

## Summary

This tutorial picks up from Tutorial 4. It simple add a new cpu core to the existing system. Both cpu cores execute the same binary (meaning that the binary is executed twice). Since there is no parrallel computing yet, so the results is independent but output will be interleaving.

## Technical Notes

A simple tutorial, not much to note. However, the concept of introducing more cores into on SoC is exiting, since it opens the door to future developement.

## Success Message

Normal run with `make run`:

```txt
Received value 1
Received results 11111111 22222222
Received value 1
Received results 11111111 22222222
Hello, got 0x12345678 from my comp
Hello, got 0x12345678 from my comp
```

We can see that the output is repeated, meaning 2 cores executes the same binary. And the results is interleaving, meaning the execution of the 2 cores are independent and overlapping.

Running with trace with `make run runner_args="--trace=insn"`

```txt
16600000: 1660: [/soc/host/insn                 ] main:10                          M 0000000000002c14 addiw               a4, a5, 0                  a4=000000000000000e  a5:000000000000000e
16600000: 1660: [/soc/host2/insn                ] main:10                          M 0000000000002c14 addiw               a4, a5, 0                  a4=000000000000000e  a5:000000000000000e
16610000: 1661: [/soc/host/insn                 ] main:10                          M 0000000000002c18 c.sw                a4, 0(a2)                  a4:000000000000000e  a2:0000000020000000  PA:0000000020000000
16610000: 1661: [/soc/host2/insn                ] main:10                          M 0000000000002c18 c.sw                a4, 0(a2)                  a4:000000000000000e  a2:0000000020000000  PA:0000000020000000
16620000: 1662: [/soc/host/insn                 ] main:8                           M 0000000000002c1a c.addiw             a5, a5, 1                  a5=000000000000000f  a5:000000000000000e
16620000: 1662: [/soc/host2/insn                ] main:8                           M 0000000000002c1a c.addiw             a5, a5, 1                  a5=000000000000000f  a5:000000000000000e
```

This is the concrete artifact that both cores are executing.

## Error Log
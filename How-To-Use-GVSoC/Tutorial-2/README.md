# Tutorial 2 Notes

## Tutorial 2 summary

This tutorial create MyComp2, and its function is as follows:
- It is connected to MyComp through a port, and when MyComp1 recieves a read request from the CPU, MyComp send a notification to MyComp2
- MyComp2 sends back to MyComp 2 values
- MyComp recieves and print those values
- MyComp finishes handling CPU read and return the value for the I/O read command from the CPU

## Coding Notes

## Technical Notes

1. There are 2 ways to communicate in hardware (as of right now):
- Using I/O request: Here we don't need an output port, we only need to define an input port to receive request, and we write the response directly onto the request's buffer memory (tutorial 1).
- Using wire notification: Here we don't have any I/O request, thus we don't have any response buffer to write the response directly to. That's why in this case we need both input and output ports (tutorial 2).

2. Why for the .py script the input and output port are registered in a different syntax:
- Input ports creates an object so that other ouput ports/ico can have an "object" (so to speak) to connect to. Hence, its a function that returns an object.

```python
def i_INPUT(self):
    return gvsoc.systree.SlaveItf(slef, "input", signature="io")
```

- Output ports on the other hand basically recieves a desitation (denoted `itf`) and connect (bind) to it. No object needs to be created, only a function needs to be called.

```python
def o_OUPUT(self, itf):
    self.itf_bind("notif", itf, signature="wire<bool>")
```

- This is purely a choice of the GVSoC Python's API.

## Success Message

Normal run with `make run`

```txt
Received request at offset 0x0, size 0x4, is_write 0
Received value 1
Received results 11111111 22222222
Hello, got 0x12345678 from my comp
```

Run with tracing with `make run runner_args="--trace=insn"`

```txt
(.venv) stvn@DESKTOP-PU3LLP9:~/UREKA/gvsoc/engine/docs/developer_manual/tutorials/2_how_to_make_components_communicate_together$ make run runner_args="--trace=insn"
/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/2_how_to_make_components_communicate_together/build/install/bin/gvrun --target-dir=/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/2_how_to_make_components_communicate_together --target=my_system --work-dir=/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/2_how_to_make_components_communicate_together/build/work --parameter binary=/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/2_how_to_make_components_communicate_together/build/test/test run --trace=insn
30000: 3: [/soc/host/insn                ] _start:5                         M 0000000000000c14 auipc               sp, 0x0           sp=0000000000000c14
40000: 4: [/soc/host/insn                ] _start:5                         M 0000000000000c18 addi                sp, sp, fffffffffffffe7c  sp=0000000000000a90  sp:0000000000000c14
50000: 5: [/soc/host/insn                ] _start:8                         M 0000000000000c1c auipc               t0, 0x0                   t0=0000000000000c1c
60000: 6: [/soc/host/insn                ] _start:8                         M 0000000000000c20 addi                t0, t0, 1a                t0=0000000000000c36  t0:0000000000000c1c
70000: 7: [/soc/host/insn                ] _start:9                         M 0000000000000c24 csrrw               0, t0, mtvec              t0:0000000000000c36
80000: 8: [/soc/host/insn                ] _start:12                        M 0000000000000c28 auipc               t0, 0x0                   t0=0000000000000c28
90000: 9: [/soc/host/insn                ] _start:12                        M 0000000000000c2c addi                t0, t0, ffffffffffffffc4  t0=0000000000000bec  t0:0000000000000c28
100000: 10: [/soc/host/insn                ] _start:13                        M 0000000000000c30 c.li                a0, 0, 0                  a0=0000000000000000
```

## Error Logging

Available in ERROR_LOG.md
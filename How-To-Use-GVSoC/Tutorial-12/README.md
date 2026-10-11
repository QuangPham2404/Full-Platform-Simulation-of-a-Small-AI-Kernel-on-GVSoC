# Tutorial 12 Notes

## Summary

### Continuation

This tutorial picks up from Tutorial 11.

### What it does

This tutorial focuses on adding latency to the CPU carrying out instructions by simulating 2 real-world lantency factors: (1) Latency due to dependency (more software-based) and (2) Latency in the cpu execution itself (hardware-based).

The reason why we need to model this because the simulation doesn't handle timing using real world timing - think of it like almost an ideal execution of hardware. So in order to accurate model our devices, developer must add specific latency to mimic the real world behaviour.

## Technical Notes

### The types of lantency we added so far

| Tutorial | What it models in real life | What it uses in GVSoC |
|---|---|---|
| **6 — Component timing** | A hardware component takes time to perform an operation before sending a result; other components can continue working meanwhile. | `vp::ClockEvent` + `event.enqueue(10)` schedules a callback 10 cycles later. |
| **7 — I/O request timing** | A memory-mapped peripheral takes time to respond to a CPU read/write request, potentially stalling the CPU. | **Synchronous:** `req->inc_latency(1000)` adds request latency. **Asynchronous:** `event.enqueue(2000)` + `vp::IO_REQ_PENDING`, then `req->get_resp_port()->resp(req)` sends the response later. |
| **10 — Interconnect timing** | Data transfers through a bus/interconnect take time due to propagation/routing overhead and limited bandwidth. | **Path latency:** `ico.o_MAP(..., latency=100)` adds 100 cycles per access on that mapping. **Bandwidth:** `Router(..., bandwidth=1)` limits transfer capacity to 1 byte/cycle. |
| **12 — ISS instruction timing (CPU)** | CPU instructions have execution/result latencies; dependent instructions may need to wait for results. | **Static result latency:** `insn.get_out_reg(0).set_latency(100)`. **Dynamic stall:** `iss->timing.stall_cycles_account(REG_GET(1))`. |

## Simulating depedency-based delay for CPU

We do this by (1) creating our own custom CPU, (2) add a fixed latency to the instruction's output register. This is basically saying "after the output is generated (usually instantenous by simulator), add a N cycles latency before making it available. This affects the next instructions that depends on the instruction's output. For instance, in this tutorial:

```txt
my_instr x3, x1, x2   # Produces x3 - we add latency here
add      x4, x3, x5   # Needs x3
```

Then the `add` instruction is delayed by 100 cycles. HOWEVER: other instructions that are NOT depdendent on `my_instr` can still be carried out by the CPU and not affected (this is also true in the real world).

## Simulating execution-based delay for CPU

We do this by explicitly changing the isa `.hpp` file (the file where we define the instruction's implementation) and add a latency in the instruction. So this means the latency is not OUTSIDE the instruction (method 1) its now INSIDE the instruction.

## Success Message

### Dependency-based delay

We should see something like this (note the 100 cycles delay):

```txt
32470000: 3247: [/soc/host/insn                 ] main:0                           M 0000000000002c3e jal                 ra, ffffffffffffdee2      ra=0000000000002c42
32650000: 3265: [/soc/host/insn                 ] $d:0                             M 0000000000000b20 my_instr            a0, a0, a1                a0=0000000000000019  a0:0000000000000005  a1:000000000000000a
32800000: 3280: [/soc/host/insn                 ] $x:0                             M 0000000000000b24 c.jr                0, ra, 0, 0               ra:0000000000002c42
33800000: 3380: [/soc/host/insn                 ] main:0                           M 0000000000002c42 c.mv                a2, 0, s0                 a2=000000000000000a  s0:000000000000000a
33810000: 3381: [/soc/host/insn                 ] main:0                           M 0000000000002c44 addiw               a3, a0, 0                 a3=0000000000000019  a0:0000000000000019
```

### Latency in CPU execution

We should see something like this (note the 100 cycles delay):

```txt
cycle 168  my_instr ... a0=0x19
cycle 269  c.jr ...
cycle 271  main:15 c.mv ... a0=0x19
```

## Error Logs

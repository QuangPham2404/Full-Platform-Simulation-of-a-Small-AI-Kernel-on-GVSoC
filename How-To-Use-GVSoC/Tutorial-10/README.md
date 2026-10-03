# Tutorial 10 Notes

## Summary

### Continuation

This tutorial picks up from tutorial 4 (the baseline)

### What it does

It introduces the concept of simulating ico latency due to 2 reasons: "physical" latency and latency due to bandwith.

### Compared to Tutorial 6 and Tutorial 7

| Tutorial | What it does | What it tries to simulate |
| --- | --- | --- | 
| Tutorial 6 - How to add timing | Add delay using ClkEvents and enqueue() | Organizational timing (syncronization) |
| Tutorial 7 - How to use the IO request interface | Add delay using req()->inc_latency() | Delay to *complete* a request |
| Tutorial 10 - How to customize ico timing | Add delay in ico.o_MAP() and changing bandwith of the ico itself | Delay due to physical wire and/or bandwidth |

## Technical Notes

### Two new concepts of latency

1. Latency from physical wire: the time it literally take for the physical signal to go from one place to another through the ico
2. Latency from bandwidth: as the bandwith becomes smaller, the longer it will take for an instruction to be carried out. This is also counted as latency.

### Note for gvsoc

In gvsoc, timing is counted by **cycles**, not seconds. Also the timing is also the simulation timing, so no need look at that. We care about the cycle counter more.

## Success Message

### ico physical latency

Running with `make run runner_args="--trace=insn"` shows us the latency of 100 added to the ico.

```txt
12570000: 1257: [/soc/host/insn                ] main:0                           M 0000000000002bda c.li                a5, 0, 14                 a5=0000000000000014
13580000: 1358: [/soc/host/insn                ] main:0                           M 00000
```

Note the jump of cycle numbering from `1257` to `1358`. That's the latency when executing main().

### ico bandwith latency

The **router trace**, `make run runner_args="--trace=insn --trace=ico --trace-level=trace`, shows the bandwidth reservation. From the run:

```
cycle 12077: read request, size 0x4
cycle 12077: bandwidth: 1, next_burst: 12294
cycle 12078: read request, size 0x4
cycle 12078: bandwidth: 1, next_burst: 12298
```

Note the the instruction tracing `insn` doesn't show because since the value are not used, the cpu can shoot 4 instructions (check `main.c` - we read the value 4 times per iteration) one after another without delay because there is no need for sync wait.

## Error Logs
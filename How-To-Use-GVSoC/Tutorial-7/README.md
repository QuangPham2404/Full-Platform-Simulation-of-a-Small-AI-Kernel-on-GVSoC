# Tutorial 7 Notes

## Summary

This tutorial picks of after tutorial 4 (tracing). The aim is to showcase how we can code *syncronous* and *asyncronous* response during I/O exchange.

- Sync means the request is imediately initiated, but with added latency in the transfer of the response (added to simulate latency in real system)
- Async means the component actively waits (usually for something to complete first) then proceed to transfer the response

We see this a lot in our HPC applications.

## Technical Notes

1. **How to do syncronous response**

We can simply add a latency before the respond command.

```cpp
req->inc_latency(1000); // Latency of 1000 cycles
```

2. **How to do asyncronous response**

To do this, we first need to use a `vp::ClockEvent` object and a corresponding `handle_event()` function, creating a 2-step response structure like in Tutorial 6. To handle pending request correctly, we also need `vp::ioReq` object that allows us to store the pending request and return a proper pending message `return vp::IO_REQ_PENDING;`. This is crucial because asyncronous repsonse must be handled correctly or else the order of response will be messed up.

In this tutorial we add another register-mapping to separate syncronous and asyncronous response - 0x00 and 0x04 respectively (global address).

Differentiate: async wait time is DIFFERENT from sync latency.

3. **Note about initializing `ClockEvent` objects**

After we create an `ClockEvent` `event` object, we need to add it into the intitializer. This is the same as the tracing event ni previous tutorials. Think of them as requirements needed to initialize the component.

## Success Message

1. Syncronous run with `make run --runner_args="--trace=my_comp"`:

```txt
0: -1: [/soc/my_comp/trace            ] New slave port (name: clock, port: 0x60fef4fc1778)
0: -1: [/soc/my_comp/trace            ] New slave port (name: reset, port: 0x60fef4fc1580)
0: -1: [/soc/my_comp/comp             ] New slave port (name: power_supply, port: 0x60fef4fc1628)
0: -1: [/soc/my_comp/comp             ] New slave port (name: voltage, port: 0x60fef4fc16d0)
0: -1: [/soc/my_comp/comp             ] New slave port (name: input, port: 0x60fef4fc1818)
0: -1: [/soc/my_comp/comp             ] New slave port (name: result, port: 0x60fef4fc19c0)
0: -1: [/soc/my_comp/comp             ] New master port (name: notif, port: 0x60fef4fc18d8)
0: -1: [/soc/my_comp2/trace           ] New slave port (name: clock, port: 0x60fef4fc64b8)
0: -1: [/soc/my_comp2/trace           ] New slave port (name: reset, port: 0x60fef4fc62c0)
0: -1: [/soc/my_comp2/comp            ] New slave port (name: power_supply, port: 0x60fef4fc6368)
0: -1: [/soc/my_comp2/comp            ] New slave port (name: voltage, port: 0x60fef4fc6410)
0: -1: [/soc/my_comp2/comp            ] New slave port (name: notif, port: 0x60fef4fc6558)
0: -1: [/soc/my_comp2/comp            ] New master port (name: result, port: 0x60fef4fc6600)
0: -1: [/soc/my_comp/comp             ] Creating final bindings
0: -1: [/soc/my_comp/comp             ] Creating final binding (/soc/my_comp:notif -> /soc/my_comp2:notif)
0: -1: [/soc/my_comp2/comp            ] Creating final bindings
0: -1: [/soc/my_comp2/comp            ] Creating final binding (/soc/my_comp2:result -> /soc/my_comp:result)
0: 0: [/soc/my_comp/comp             ] Reset (active: 1)
0: 0: [/soc/my_comp2/comp            ] Reset (active: 1)
0: 0: [/soc/my_comp/comp             ] Reset (active: 0)
0: 0: [/soc/my_comp2/comp            ] Reset (active: 0)
1580000: 158: [/soc/my_comp/trace            ] Received request at offset 0x0, size 0x4, is_write 0
```

Note that here since we are only tracing my_comp, it only shows the instant when it responds, and we can't really see the delay of 1000 cycles here. To see that use `trace=insn`

2. Asyncronous run with `make run --runner_args="--trace=my_comp"`

```txt
ce=my_comp
0: -1: [/soc/my_comp/trace            ] New slave port (name: clock, port: 0x5c3baf288778)
0: -1: [/soc/my_comp/trace            ] New slave port (name: reset, port: 0x5c3baf288580)
0: -1: [/soc/my_comp/comp             ] New slave port (name: power_supply, port: 0x5c3baf288628)
0: -1: [/soc/my_comp/comp             ] New slave port (name: voltage, port: 0x5c3baf2886d0)
0: -1: [/soc/my_comp/comp             ] New slave port (name: input, port: 0x5c3baf288818)
0: -1: [/soc/my_comp/comp             ] New slave port (name: result, port: 0x5c3baf2889c0)
0: -1: [/soc/my_comp/comp             ] New master port (name: notif, port: 0x5c3baf2888d8)
0: -1: [/soc/my_comp2/trace           ] New slave port (name: clock, port: 0x5c3baf28d578)
0: -1: [/soc/my_comp2/trace           ] New slave port (name: reset, port: 0x5c3baf28d380)
0: -1: [/soc/my_comp2/comp            ] New slave port (name: power_supply, port: 0x5c3baf28d428)
0: -1: [/soc/my_comp2/comp            ] New slave port (name: voltage, port: 0x5c3baf28d4d0)
0: -1: [/soc/my_comp2/comp            ] New slave port (name: notif, port: 0x5c3baf28d618)
0: -1: [/soc/my_comp2/comp            ] New master port (name: result, port: 0x5c3baf28d6c0)
0: -1: [/soc/my_comp/comp             ] Creating final bindings
0: -1: [/soc/my_comp/comp             ] Creating final binding (/soc/my_comp:notif -> /soc/my_comp2:notif)
0: -1: [/soc/my_comp2/comp            ] Creating final bindings
0: -1: [/soc/my_comp2/comp            ] Creating final binding (/soc/my_comp2:result -> /soc/my_comp:result)
0: 0: [/soc/my_comp/comp             ] Reset (active: 1)
0: 0: [/soc/my_comp2/comp            ] Reset (active: 1)
0: 0: [/soc/my_comp/comp             ] Reset (active: 0)
0: 0: [/soc/my_comp2/comp            ] Reset (active: 0)
1580000: 158: [/soc/my_comp/trace            ] Received request at offset 0x0, size 0x4, is_write 0
Received value 1
Received results 11111111 22222222
Hello, got 0x12345678 from my comp
```

Similar to sync case, we can't see the async behaviour with only my_comp tracing.

## Error Log

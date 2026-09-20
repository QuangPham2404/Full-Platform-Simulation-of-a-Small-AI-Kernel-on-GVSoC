# Tutorial 6 Notes

## Summary

This tutorial picks up from Tutorial 3 and add a delay of 10 cycles before my_comp2 sends the output back to my_comp1 after recieving the boolean notif signal

## Technical Notes

1. A note about `vp::ClockEvent` object usage

In this tutorial we set 1 alarm for 1 notif repsonse behaviour. Later on, when we may have multiple responses for different inputs, to schedule we need to create a `vp::ClockEvent` object corresponding to each handling function.

## Success messages

Normal run with `make run`

```txt
Received value 1
Received results 11111111 22222222
Hello, got 0x12345678 from my comp
```

Tracing run with `make run runner_args="--trace=/soc/my_comp/trace --trace=/soc/my_comp2/trace"`

You can see the difference of 10 cycles 158 --> 168 between when my_comp2 recieves the notif and when it sends output to my_comp

```txt
0: -1: [/soc/my_comp/trace            ] New slave port (name: clock, port: 0x63e32b9b37e8)
0: -1: [/soc/my_comp/trace            ] New slave port (name: reset, port: 0x63e32b9b35f0)
0: -1: [/soc/my_comp2/trace           ] New slave port (name: clock, port: 0x63e32b9bea08)
0: -1: [/soc/my_comp2/trace           ] New slave port (name: reset, port: 0x63e32b9be810)
1580000: 158: [/soc/my_comp/trace            ] Received request at offset 0x0, size 0x4, is_write 0
Received value 1
1680000: 168: [/soc/my_comp2/trace           ] Sending result
Received results 11111111 22222222
Hello, got 0x12345678 from my comp
```

## Error logs
# Tutorial 3 Notes

## Tutorial 3 summary

The aim is learn how to add traces to replace ad-hoc print statement, example below in `my_comp1.cpp`:

```cpp
// Printing out the request for logging - recall that req is a pointer to an I/O request
    printf("Received request at offset 0x%lx, size 0x%lx, is_write %d\n",
        req->get_addr(), req->get_size(), req->get_is_write());
```

Replace with:

```cpp
_this->trace.msg(vp::TraceLevel::DEBUG, "Received request at offset 0x%lx, size 0x%lx, is_write %d\n",
        req->get_addr(), req->get_size(), req->get_is_write());
```

## Technical notes

Steps to include a trace:

1. In your component's class, create a "trace channel"

```cpp
vp::Trace trace;
```

2. In the same component class's constructor, "register" that channel with a name using "traces" inherited from `vp::Components`. After this "trace" is considered a sub-component of "my_comp" in gvsock's component tree.

```cpp
this->traces.new_trace("trace", &this->trace);
```

3. Use that trace in the component's function. Note that there may be different trace level - we will explore that further if needed.

```cpp
 _this->trace.msg(vp::TraceLevel::DEBUG, "Received request at offset 0x%lx, size 0x%lx, is_write %d\n",
        req->get_addr(), req->get_size(), req->get_is_write());
```

## Success Message

Normal run with `make run`:

```txt
w_to_add_system_traces_to_a_component$ make run
/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/3_how_to_add_system_traces_to_a_component/build/install/bin/gvrun --target-dir=/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/3_how_to_add_system_traces_to_a_component --target=my_system --work-dir=/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/3_how_to_add_system_traces_to_a_component/build/work --parameter binary=/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/3_how_to_add_system_traces_to_a_component/build/test/test run
Received value 1
Received results 11111111 22222222
Hello, got 0x12345678 from my comp
```

Run with created trace channel with `make run runner_args="--trace=my_comp"`:

Some intepretation:
- The syntax is as follows: <simulation time>: <cycle-ish counter>: [<trace path>] <message>
- Those with simulation time as 0 are set up steps (e.g. creating ports, binding ports, reset before running program to simulate)
- `1580000: 158:...` is the part when the program + soc is simulated. Here it shows the tracing message we've defined. Note that the time is in GVSoC's own timing, not wall-time.

```txt
0: -1: [/soc/my_comp/trace            ] New slave port (name: clock, port: 0x60833cbc3858)
0: -1: [/soc/my_comp/trace            ] New slave port (name: reset, port: 0x60833cbc3660)
0: -1: [/soc/my_comp/comp             ] New slave port (name: power_supply, port: 0x60833cbc3708)
0: -1: [/soc/my_comp/comp             ] New slave port (name: voltage, port: 0x60833cbc37b0)
0: -1: [/soc/my_comp/comp             ] New slave port (name: input, port: 0x60833cbc38f8)
0: -1: [/soc/my_comp/comp             ] New slave port (name: result, port: 0x60833cbc3aa0)
0: -1: [/soc/my_comp/comp             ] New master port (name: notif, port: 0x60833cbc39b8)
0: -1: [/soc/my_comp2/trace           ] New slave port (name: clock, port: 0x60833cbc8088)
0: -1: [/soc/my_comp2/trace           ] New slave port (name: reset, port: 0x60833cbc7e90)
0: -1: [/soc/my_comp2/comp            ] New slave port (name: power_supply, port: 0x60833cbc7f38)
0: -1: [/soc/my_comp2/comp            ] New slave port (name: voltage, port: 0x60833cbc7fe0)
0: -1: [/soc/my_comp2/comp            ] New slave port (name: notif, port: 0x60833cbc8128)
0: -1: [/soc/my_comp2/comp            ] New master port (name: result, port: 0x60833cbc81d0)
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

## Error Logs

No error for this tutorial - just note the syntax errors from Tutorial 2

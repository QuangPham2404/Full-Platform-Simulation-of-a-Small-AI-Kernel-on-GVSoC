# Tutorial 4 Notes

## Summary

Instead of normal tracing which displays as text on terminals, using vcd trace allow us to display traces as waveforms and inspect them using the "gtkwave" application.

## Technical notes

1. To initialize the vcd trace values we do that directly on the constructor initializer, unlike normal traces:

```cpp
MyComp::MyComp(vp::ComponentConf &config)
    : vp::Component(config), vcd_value(*this, "status", 32)
{
    ...
}
```

2. set() and release()
- set(): We manage vcd traces like changing signals. So we use this function to set the signal to a specific value

```cpp
_this->vcd_value.set(value);
```

- release(): Display signal as `z` to catch high impendance (i.e. the componenet is idle/dinsconnected so signals are basically noise). This is to catch idle time, and we need to set a threshold value for the vcd trace to use release (as seen below)

```cpp

if (value == 5)
    {
        _this->vcd_value.release();
    }
```

## Success messages

Normal run with `make run`:

```
 run
Received value 1
Received results 11111111 22222222
Hello, got 0x12345678 from my comp
```

vcd-traced run with `make run runner_args="--vcd --event=.*"`

```
A Gtkwave script has been generated and can be opened with the following command:
gtkwave /home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/4_how_to_add_vcd_traces_to_a_component/build/work/view.gtkw

Received value 1
Received results 11111111 22222222
Hello, got 0x12345678 from my comp
```

After that, we can follow the gtkwave command to open gtkwave and inspect the waveform. Remember to choose zoom fit and then zoom in to the regions of interest.

## Installing gtkwave

On Linux we can install gtkwave simply with

```bash
sudo apt update

sudo apt install gtkwave

which gtkwave
```

## Error logs

1. Segmentation fault due to no I/O status being returned:

```txt
Received value 1
Received results 11111111 22222222
Hello, got 0x12345678 from my comp
Segmentation fault (core dumped)
Input error: Platform returned an error (exitcode: 139)
make: *** [Makefile:26: run] Error 1
```

Solution: Add ```return vp::IO_REQ_OK;``` at the end of the "write-detected" else if in ```handel_req```
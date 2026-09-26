# Tutorial 9 Notes

## Summary

This tutorial picks up from tutorial 4. The main goals for this tutorial are

- (1) Add 2 clock domains for the SoC: 1 for the cpu core and 1 for the rest of the components (interconnect, mem, my_comp1, my_comp2, etc)
- (2) The clock frequency control for CPU is given to my_comp, and we will dynamically change its clock speed while the program is running. 

## Technical Notes

1. **Why do we need different clock domains on a SoC**

In a real world system, not every component needs a fast clock. Therefore, defining clock domains with different clock frequency is needed to be more efficient in terms of power consumption while maintaing desirable performance. For instance, a system might look like this:

```txt
Performance CPU cores   high frequency
Efficiency CPU cores    medium frequency
GPU                     medium/high frequency
NPU                     mostly idle
camera ISP              idle
USB controller          low frequency
audio subsystem         low frequency
```

2. **Why do we need to allow clock frequency changing**

Again this circles back to efficiency. A real life example: if a phone is being used, the CPU clk should be boosted to a higher frequency. However, if it is unused - it is practical to lower the frequency.

3. **Frequency scaling**

This basially means *dynamically* changing the frequency of the component *during* a execution process. An example: while doing a workload, you find that the current clk speed is inefficient, so you boost the freq up higher to gain more performance or vice versa.

4. Notes on creating the clk_ctrl interfact

Like previous I/O interfaces (i.e. I/O ports), to create one we have 2 steps:

- (1) "Declare" in my_comp.py

```python
def o_CLK_CTRL(self, itf):
        self.itf_bind('clk_ctrl', itf, signature='clock')
```

- (2) Then "register" in my_comp.cpp

```cpp
// Create the itf object first in class declaration
vp::ClockMaster clk_ctrl_itf;

// Register it in instantiation function of the class
this->new_master_port("clk_ctrl", &this->clk_ctrl_itf);

```

## Success Results

Normal run with `make run`

```txt
Received value 1
Received results 11111111 22222222
Hello, got 0x12345678 from my comp
```

Tracing run with `make run runner_args="--trace=insn"` will expose the artifact where the frequency of the cpu shifts based on the simulation.

## Error Logs

### Error 1: `make gvsoc` reports an invalid clock signature

**What the error says**

```text
RuntimeError: Invalid signature (master: clock@soc/my_comp->clk_ctrl, slave: clock_ctrl@clock1->clock_in)
```

**Root cause**

`my_comp.py` declared the `clk_ctrl` port with `signature='clock'`, but `clock1.i_CTRL()` is a clock control interface with signature `clock_ctrl`. The signatures must match.

**Solution**

Declare the port with the control signature:

```python
def o_CLK_CTRL(self, itf):
    self.itf_bind('clk_ctrl', itf, signature='clock_ctrl')
```

The C++ port remains a `vp::ClockMaster` registered as `clk_ctrl` in `my_comp.cpp`.

### Error 2: `make run` reports an invalid slave binding

**What the error says**

```text
Binding from invalid slave (master: clock1 / out, slave: host / clock)
```

The same scope problem can also appear on the control connection, for example with `my_comp / clk_ctrl` and `clock1 / clock_in`.

**Root cause**

`clock1`, `clock2`, and `soc` are siblings under `Rv64`, while `host`, `mem`, `ico`, `loader`, and `my_comp` are children inside `Soc`. A binding recorded at the `Rv64` level cannot refer directly to those internal `Soc` components. The same applies to the clock control connection from `my_comp` to `clock1`.

**Solution**

Expose the clock and control interfaces on `Soc`. In `Rv64`, connect only the clock domains to the `Soc` ports:

```python
clock1.o_CLOCK(soc.i_HOST_CLOCK())
clock2.o_CLOCK(soc.i_SYSTEM_CLOCK())
soc.o_CLK_CTRL(clock1.i_CTRL())
```

Inside `Soc`, bind `host_clock` to the CPU clock, `system_clock` to the other clocked components, and `my_comp`'s `clk_ctrl` port to the exposed control port. This keeps the component tree intact while each connection is registered at the correct level. After changing the component tree, run `make gvsoc` before `make run` so the generated platform tree is refreshed.

# Tutorial 2 Error Logs

## Error 0: Missing Python module

### 1. What was the error

Running:

```bash
make gvsoc
```

failed while generating the GVSoC configuration:

```text
ModuleNotFoundError: No module named 'prettytable'
CMake Error at engine/CMakeLists.txt:151 (message):
  Caught error while generating gvsoc config
```

### 2. What causes the error

The `gapy` generator imports the Python package `prettytable`. The package is declared in the project's requirements, and it was installed in the project's virtual environment, but the virtual environment was not active.

As a result, `gapy` was executed with the system Python interpreter instead of the project interpreter.

### 3. How to solve the error

Activate the project virtual environment before building:

```bash
source /home/stvn/UREKA/gvsoc/.venv/bin/activate
make gvsoc
```

If the virtual environment has not been populated, install the project requirements first from the GVSoC root directory:

```bash
pip3 install -r gapy/requirements.txt
pip3 install -r core/requirements.txt
```

## Error 1: C++ component source did not compile

### 1. What was the error

After activating the virtual environment, the Python configuration step succeeded, but compilation of [`my_comp.cpp`](my_comp.cpp) failed with errors including:

```text
error: ‘IoSlave’ in namespace ‘vp’ does not name a type
error: ‘IoReqStatus’ in namespace ‘vp’ does not name a type
error: ‘statis’ does not name a type
error: expected unqualified-id before string constant
```

### 2. What causes the error

The component source contained several source-level errors:

- The I/O and wire interface headers were not included.
- `vp::io_req` used the wrong type name; the API uses `vp::IoReq`.
- `statis` was a typo for `static`.
- Several semicolons were missing.
- The `if` condition had invalid parentheses.
- The callback returned without a terminating semicolon and did not handle invalid requests.
- The `notif` master port was registered with `result_itf` instead of `notif_itf`.
- GVSoC callback signatures require `vp::Block *`, not `void *`.

### 3. How to solve the error

Add the required headers:

```cpp
#include <vp/itf/io.hpp>
#include <vp/itf/wire.hpp>
```

Use the current GVSoC callback signatures:

```cpp
static vp::IoReqStatus handle_req(vp::Block *__this, vp::IoReq *req);
static void handle_result(vp::Block *__this, MyResult *result);
```

Correct the request condition and return values:

```cpp
if (!req->get_is_write() && req->get_addr() == 0 && req->get_size() == 4)
{
    *(uint32_t *)req->get_data() = _this->value;
    _this->notif_itf.sync(true);
    return vp::IO_REQ_OK;
}

return vp::IO_REQ_INVALID;
```

Register the notification port with the matching interface:

```cpp
this->new_master_port("notif", &this->notif_itf);
```

After these changes, the following command completed successfully:

```bash
source /home/stvn/UREKA/gvsoc/.venv/bin/activate
make gvsoc
```

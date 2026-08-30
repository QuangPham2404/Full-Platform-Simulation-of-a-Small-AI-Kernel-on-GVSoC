# Tutorial 1 Error Logs

## Error 0: Invalid Make target

### 1. What was the error

Running:

```bash
make gvsok
```

causes:

```text
make: *** No rule to make target 'gvsok'. Stop.
```

### 2. What causes the error

`gvsok` is not a target defined in the Tutorial 1 [`Makefile`](Makefile). The target is named `gvsoc`.

### 3. How to solve the error

Run:

```bash
make gvsoc
```

## Error 1: Missing `i_INPUT` attribute

### 1. What was the error

Running:

```bash
make gvsoc
```

failed while generating the GVSoC configuration:

```text
AttributeError: 'MyComp' object has no attribute 'i_INPUT'
```

The failing call was in [`my_system.py`](my_system.py):

```python
ico.o_MAP(comp.i_INPUT(), "comp", ...)
```

### 2. What causes the error

The `i_INPUT` function was indented inside `MyComp.__init__`. Therefore, it was a local function and not a method of the `MyComp` class. The component instance had no `i_INPUT` method to call.

### 3. How to solve the error

Define `i_INPUT` at class scope, aligned with `__init__`:

```python
class MyComp(gvsoc.systree.Component):
    def __init__(self, parent, name, value):
        super().__init__(parent, name)
        self.add_sources(["my_comp.cpp"])
        self.add_properties({"value": value})

    def i_INPUT(self) -> gvsoc.systree.SlaveItf:
        return gvsoc.systree.SlaveItf(
            self, "input", signature="io"
        )
```

## Error 2: C++ component compilation errors

### 1. What was the error

After fixing the Python method, `make gvsoc` reached C++ compilation and reported errors including:

```text
error: ‘vp::io_req’ has not been declared
error: expected unqualified-id before string constant
error: invalid conversion from ‘vp::IoReqStatus (*)(void*, int*)’
error: no declaration matches ‘vp::IoReqStatus MyComp::handle_req(void*, vp::IoReq*)’
```

### 2. What causes the error

The C++ component contained several syntax and API mismatches:

- The request type was written as `vp::io_req`; GVSoC defines `vp::IoReq`.
- The callback context type was written as `void *`; the current GVSoC API expects `vp::Block *`.
- The class definition was missing its terminating semicolon.
- The `if` condition had an extra closing parenthesis.
- The `return vp::IO_REQ_OK` statement was missing a semicolon.
- The callback did not return a status on every path.

### 3. How to solve the error

Use the current callback declaration and definition:

```cpp
static vp::IoReqStatus handle_req(vp::Block *__this, vp::IoReq *req);
```

```cpp
vp::IoReqStatus MyComp::handle_req(vp::Block *__this, vp::IoReq *req)
```

Also ensure that the class ends with `};`, the condition is valid, and both return statements have semicolons:

```cpp
if (!req->get_is_write() &&
    req->get_addr() == 0 &&
    req->get_size() == 4)
{
    *(uint32_t *)req->get_data() = _this->value;
    return vp::IO_REQ_OK;
}

return vp::IO_REQ_INVALID;
```

## Error 3: Application binary could not be loaded

### 1. What was the error

Running:

```bash
make run
```

reported:

```text
Unable to open binary (path: .../build/test/test, error: Bad file descriptor)
Input error: Platform returned an error (exitcode: 1)
```

### 2. What causes the error

The `run` target expects the application ELF at:

```text
build/test/test
```

That file did not exist because the separate `make all` step had not successfully built it. The `make run` target only launches the simulation; it does not compile the application.

### 3. How to solve the error

Build the application before running the simulation:

```bash
make all
make run
```

If `make all` fails, fix that build error first.

## Error 4: Missing RISC-V C library header

### 1. What was the error

Running:

```bash
make all
```

failed while compiling [`../utils/prf.c`](../utils/prf.c):

```text
../utils/prf.c:16:10: fatal error: sys/types.h: No such file or directory
make: *** [Makefile:17: all] Error 1
```

The source contains:

```c
#include <sys/types.h>
```

### 2. What causes the error

The RISC-V compiler was installed, and the Picolibc header existed at:

```text
/usr/lib/picolibc/riscv64-unknown-elf/include/sys/types.h
```

However, `riscv64-unknown-elf-gcc` was not automatically searching that directory. This is a cross-compiler include-path problem.

### 3. How to solve the error

Add the Picolibc include directory to `RT_FLAGS` in the shared runtime makefile [`../utils/rt.mk`](../utils/rt.mk):

```make
RT_FLAGS = -march=rv64imafdc -O3 -fno-tree-loop-distribute-patterns \
-I../utils -I/usr/lib/picolibc/riscv64-unknown-elf/include \
-T../utils/link.ld -nostartfiles -nostdlib -Wl,--no-warn-rwx-segments
```

After adding the include path, rebuild the application and run the simulation:

```bash
make all
make run
```

The `make all` build was verified successfully with this include path.

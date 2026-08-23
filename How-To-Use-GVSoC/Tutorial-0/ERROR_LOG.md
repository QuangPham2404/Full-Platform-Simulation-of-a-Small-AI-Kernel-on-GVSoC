# Tutorial 0 Error Logs

## Error 0: Missing compiler

### 1. What was the error

Running 

```bash
make all
```
causes this error:

```bash
make: riscv64-unknown-elf-gcc: Not a directory make: *** [Makefile:17: all] Error 127
```

## 2. What causes the error

This is a simple missing-compiler problem.

## 3. How to solve the error

Install the compiler with:

```bash
sudo apt install gcc-riscv64-unknown-elf
```

After installing the compiler, we need to also install the corresponding C library header

```bash
sudo apt install picolibc-riscv64-unknown-elf
```

## Error 1: Missing RISC-V C library header

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

The source file contains:

```c
#include <sys/types.h>
```

### 2. What causes the error

`riscv64-unknown-elf-gcc` is a **cross-compiler**. It runs on the host computer but produces RISC-V code for the simulator.

A compiler also needs C library headers, such as `sys/types.h`, `stdio.h`, and `string.h`. The required Picolibc headers were installed on the system, but GCC was not automatically searching their directory:

```text
/usr/lib/picolibc/riscv64-unknown-elf/include
```

This is a toolchain search-path problem, not a missing project source file.

### 3. How to solve the error

The tutorial [`Makefile`](Makefile) was updated to enable the installed Picolibc GCC specs file:

```make
RT_FLAGS += -specs=/usr/lib/picolibc/riscv64-unknown-elf/picolibc.specs
```

The specs file is a GCC configuration file. It tells GCC where the matching Picolibc headers and libraries are located.

For a similar future error, first check whether the header exists:

```bash
find /usr -name types.h 2>/dev/null
```

Then check the compiler's search paths:

```bash
riscv64-unknown-elf-gcc -v -E -x c /dev/null
```

If the header exists outside those paths, use the toolchain's documented `--sysroot`, `-isystem`, or `-specs` option.

## Error 2: The application binary could not be read

### 1. What was the error

After the Python environment and generator paths were configured, `make run` failed with:

```text
AttributeError: 'Namespace' object has no attribute 'binary'
```

The failing code in [`my_system.py`](my_system.py) was:

```python
[args, __] = parser.parse_known_args()
binary = args.binary
```

### 2. What causes the error

The Makefile supplies the ELF application using GVSOC's parameter mechanism:

```text
--parameter binary=build/test/test
```

This is not the same as a normal command-line argument such as:

```text
--binary build/test/test
```

Therefore, Python's `argparse` parser does not create an `args.binary` field. The tutorial script used an older GVSOC interface, while the installed runner uses the newer parameter interface.

In simple terms, the Makefile and Python script were using two different ways to pass the same filename.

### 3. How to solve the error

Use GVSOC's `TargetParameter` instead of reading the value from `argparse`:

```python
from gvrun.parameter import TargetParameter

binary = TargetParameter(
    self,
    name='binary',
    value=None,
    description='Binary to be simulated'
).get_value()
```

Pass the resolved binary into the SoC:

```python
soc = Soc(self, 'soc', binary)
```

The current RISC-V model also receives the binary explicitly:

```python
host = cpu.iss.riscv.Riscv(
    self, 'host', isa='rv64imafdc', binaries=[binary]
)
```

After changing the target description, rebuild the target-specific simulator library before running:

```bash
source ../../../../../.venv/bin/activate
source ../../../../../sourceme.sh
make all
make gvsoc
make run
```

`make gvsoc` generates and compiles the platform library, for example:

```text
build/install/lib/libplatform_tree_my_system.so
```

Without this build step, `make run` can report that the platform-tree library is missing. With the updated script and rebuilt target, the simulation prints:

```text
Hello
```

# GVSoC Installation Guide on WSL (Ubuntu 24.04)

This guide documents a successful installation and first run of **GVSoC** on **Windows Subsystem for Linux (WSL)** using **Ubuntu 24.04.4 LTS**.

It is based on the current `gvsoc/gvsoc` repository and includes the issues encountered during installation, their causes, and the fixes that worked.

---

## 1. Tested Environment

The successful installation used:

```text
OS: Ubuntu 24.04.4 LTS under WSL
Architecture: x86_64
Python: 3.12.3
Shell: bash
GVSoC target: pulp-open
```

The GVSoC repository's installation instructions were originally developed using Ubuntu 22.04, so a few adjustments are required for Ubuntu 24.04.

---

## 2. Check the WSL Environment

Before installing anything, verify the operating system, architecture, compiler availability, and Python version.

Run:

```bash
lsb_release -a
uname -m
gcc --version
python3 --version
```

Example initial output:

```text
Distributor ID: Ubuntu
Description:    Ubuntu 24.04.4 LTS
Release:        24.04
Codename:       noble

x86_64

Command 'gcc' not found

Python 3.12.3
```

### What this step does

- `lsb_release -a` identifies the Linux distribution and version.
- `uname -m` checks the host architecture.
- `gcc --version` checks whether the C compiler is already installed.
- `python3 --version` checks the Python interpreter version.

A missing `gcc` installation is normal on a fresh WSL Ubuntu environment. It will be installed through `build-essential`.

---

## 3. Update Ubuntu Package Metadata

Run:

```bash
sudo apt update
```

### What this step does

`apt update` refreshes Ubuntu's local list of available packages and versions.

It should generally be run before installing a large group of development packages.

---

## 4. Install GVSoC System Dependencies

The GVSoC README lists several Ubuntu dependencies. On Ubuntu 24.04, one package name requires modification.

Install:

```bash
sudo apt install -y \
    build-essential \
    git \
    doxygen \
    python3-pip \
    python3-venv \
    libsdl2-dev \
    curl \
    cmake \
    gtkwave \
    libsndfile1-dev \
    rsync \
    autoconf \
    automake \
    texinfo \
    libtool \
    pkg-config \
    libsdl2-ttf-dev \
    wget \
    python3-sphinx
```

### What these packages provide

Some of the most important packages are:

- `build-essential` — installs the standard native compilation toolchain, including GCC, G++, and Make.
- `git` — required to clone GVSoC and retrieve its submodules.
- `cmake` — used by GVSoC's build system.
- `python3-pip` — installs Python packages.
- `python3-venv` — creates an isolated Python environment.
- `python3-sphinx` — provides the `sphinx-build` command for documentation.
- `autoconf`, `automake`, `libtool`, `pkg-config` — common native build utilities.
- `libsdl2-dev`, `libsdl2-ttf-dev`, `libsndfile1-dev` — development libraries required by parts of GVSoC.
- `gtkwave` — waveform viewing utility.
- `doxygen` — source documentation generator.

---

## 5. Error: `Unable to locate package sphinx-build`

### Error encountered

The original GVSoC dependency list contains:

```text
sphinx-build
```

Running the original installation command on Ubuntu 24.04 produced:

```text
E: Unable to locate package sphinx-build
```

### Cause

On Ubuntu 24.04, `sphinx-build` is the name of the executable, not the APT package.

The correct package name is:

```text
python3-sphinx
```

### Solution

Replace:

```bash
sphinx-build
```

with:

```bash
python3-sphinx
```

Then verify:

```bash
sphinx-build --version
```

The executable should now be available.

---

## 6. Verify the Development Toolchain

After package installation, check:

```bash
gcc --version
g++ --version
make --version
cmake --version
git --version
```

All five commands should work before continuing.

---

## 7. Clone GVSoC

Create or enter a project directory. For example:

```bash
cd ~/UREKA
```

Clone the repository:

```bash
git clone https://github.com/gvsoc/gvsoc.git
```

Enter it:

```bash
cd gvsoc
```

Verify:

```bash
git status
```

The repository should normally be on the `main` branch.

### What this step does

This downloads the top-level GVSoC repository.

However, GVSoC is composed of several separate Git repositories linked as **submodules**, so cloning the parent repository alone is not sufficient.

---

## 8. Initialize GVSoC Git Submodules

GVSoC uses submodules for components such as:

```text
core
pulp
gvtest
pulpos
gvrun
config_tree
engine
```

Initialize them recursively:

```bash
git submodule update --init --recursive --jobs 8
```

Verify them with:

```bash
git submodule status --recursive
```

### What this step does

A Git submodule is a separate repository referenced from another Git repository.

GVSoC's top-level repository therefore acts partly as an integration repository connecting several components.

The recursive command:

1. initializes each declared submodule;
2. downloads the required commits;
3. recursively initializes nested submodules;
4. performs several downloads concurrently using `--jobs 8`.

---

## 9. Error: `git submodule ... -j8` showed usage information

### Command that failed

```bash
git submodule update --init --recursive -j8
```

The installed Git version printed the `git submodule` usage page instead of running the update.

### Cause

The compact `-j8` form was not accepted by this Git invocation.

### Solution

Use the explicit form:

```bash
git submodule update --init --recursive --jobs 8
```

or:

```bash
git submodule update --init --recursive -j 8
```

---

## 10. Create a Python Virtual Environment

Because Ubuntu 24.04 uses a newer system Python configuration, it is safer to isolate GVSoC's Python dependencies.

From the GVSoC root directory:

```bash
python3 -m venv .venv
```

Activate it:

```bash
source .venv/bin/activate
```

The shell prompt should now look similar to:

```text
(.venv) user@machine:~/UREKA/gvsoc$
```

Verify:

```bash
which python
which pip
```

Both paths should point inside:

```text
~/UREKA/gvsoc/.venv/
```

### What this step does

A Python virtual environment creates an isolated Python package environment specifically for GVSoC.

This avoids:

- modifying Ubuntu's system Python installation;
- dependency conflicts with unrelated projects;
- Ubuntu 24.04's externally-managed Python restrictions.

Whenever working on GVSoC in a new shell, reactivate it with:

```bash
cd ~/UREKA/gvsoc
source .venv/bin/activate
```

---

## 11. Install GVSoC Python Dependencies

Upgrade `pip`:

```bash
pip install --upgrade pip
```

Install the requirements listed by GVSoC:

```bash
pip install -r core/requirements.txt
pip install -r gapy/requirements.txt
```

### What this step does

These commands install Python packages used by:

- GVSoC core infrastructure;
- GAPY configuration and target tooling;
- Python-based platform descriptions.

---

## 12. Build the `pulp-open` Target

GVSoC supports multiple simulated platforms.

For a PULP-style system, build:

```bash
make all TARGETS=pulp-open
```

### Why `pulp-open`?

GVSoC also supports more generic and specialized targets such as:

```text
rv64
rv64_untimed
pulp-open
pulp-open-nn
occamy
siracusa
snitch
ara
spatz
mempool
...
```

`pulp-open` is a useful first choice when learning PULP-style full-platform simulation because it models a PULP system rather than only a generic RISC-V processor.

### What `make all` does

At the top level, GVSoC defines:

```text
all: checkout build
```

Conceptually, the build flow is:

```text
make all TARGETS=pulp-open
        |
        +-- ensure submodules exist
        |
        +-- build gvrun
        |
        +-- build config_tree
        |
        +-- configure GVSoC through CMake
        |
        +-- generate the pulp-open platform configuration
        |
        +-- compile the required simulation models
        |
        +-- install GVSoC under ./install
```

---

## 13. Error: `ModuleNotFoundError: No module named 'psutil'`

During the `pulp-open` build, configuration failed with:

```text
ModuleNotFoundError: No module named 'psutil'
```

The traceback passed through:

```text
gvrun/python/gvrun/builder.py
```

and GVSoC eventually reported:

```text
RuntimeError: Dependency 'psutil' of the target module 'pulp-open' is missing
```

### Cause

The current build uses the `gvrun` submodule, whose Python code imports `psutil`.

The top-level installation instructions installed:

```text
core/requirements.txt
gapy/requirements.txt
```

but `psutil` was still absent from the active Python environment.

This is effectively an undeclared or insufficiently propagated Python dependency for this build path.

### Solution

With the virtual environment still active:

```bash
pip install psutil
```

Verify:

```bash
python -c "import psutil; print(psutil.__version__)"
```

Then rerun:

```bash
make all TARGETS=pulp-open
```

There is no need to reclone the repository or reinitialize the submodules.

---

## 14. Verify the GVSoC Executable

After the build succeeds:

```bash
ls -lh install/bin/gvsoc
```

Then:

```bash
./install/bin/gvsoc --help
```

If both commands work, GVSoC has been successfully compiled and installed locally.

---

## 15. First Attempt to Run the `pulp-open` Hello Example

The top-level README gives an example similar to:

```bash
./install/bin/gvsoc \
    --target=pulp-open \
    --binary examples/pulp-open/hello \
    image flash run
```

On the tested checkout, this failed with:

```text
Unable to open binary
(path: examples/pulp-open/hello, error: Bad file descriptor)

Input error: Platform returned an error
```

---

## 16. Cause of the Missing Example Error

Listing the top-level repository showed no:

```text
examples/
```

directory.

However, searching the recursive checkout revealed:

```text
./pulp/examples/pulp-open/hello
```

The example belongs to the `pulp` submodule.

Therefore the README's binary path did not match the actual directory layout of this checkout.

### Useful diagnostic commands

```bash
find pulp -maxdepth 4 -type f \( -name "*.c" -o -name "Makefile" -o -name "*.py" \) | head -80
```

and:

```bash
find . -type f -executable | grep -v '.venv' | head -50
```

The second command revealed several shipped binaries, including:

```text
./pulp/examples/pulp-open/hello
./pulp/examples/rv64/hello
./pulp/examples/snitch/fp32_computation_vector.elf
./pulp/examples/occamy/offload-multi_cluster.elf
...
```

---

## 17. Run the Correct `pulp-open` Hello Example

Use the actual binary path:

```bash
./install/bin/gvsoc \
    --target=pulp-open \
    --binary pulp/examples/pulp-open/hello \
    image flash run
```

Successful output:

```text
Hello from FC
```

This confirms that the installation and simulation are working.

---

## 18. What the Successful Command Means

The command:

```bash
./install/bin/gvsoc \
    --target=pulp-open \
    --binary pulp/examples/pulp-open/hello \
    image flash run
```

can be broken down as follows.

### `./install/bin/gvsoc`

Launches the GVSoC simulator that was just compiled.

### `--target=pulp-open`

Selects the simulated hardware platform.

GVSoC constructs the `pulp-open` platform model rather than a generic RISC-V processor.

### `--binary pulp/examples/pulp-open/hello`

Specifies the compiled RISC-V application that GVSoC should execute.

This binary is not executed directly by the host x86-64 CPU.

### `image`

Prepares the application image needed by the selected simulated platform.

### `flash`

Loads or prepares that image through the platform's simulated boot/storage mechanism.

### `run`

Starts simulation.

---

## 19. What `Hello from FC` Means

The output:

```text
Hello from FC
```

means the program successfully executed on the simulated PULP platform.

`FC` stands for **Fabric Controller**.

At a high level, the execution chain is:

```text
Windows PC / x86-64 CPU
        |
        | runs
        v
WSL Ubuntu
        |
        | runs
        v
GVSoC
        |
        | simulates
        v
PULP Open SoC
        |
        | simulated RISC-V execution
        v
hello application
        |
        v
"Hello from FC"
```

This distinction is important.

The host x86-64 CPU does **not** directly execute the RISC-V `hello` binary.

Instead:

1. the host runs GVSoC;
2. GVSoC models the target hardware;
3. the simulated RISC-V core executes the program.

This is the first successful **full-platform simulation sanity check**.

---

## 20. Final Working Setup

A working shell session should look approximately like:

```bash
cd ~/UREKA/gvsoc
source .venv/bin/activate

./install/bin/gvsoc \
    --target=pulp-open \
    --binary pulp/examples/pulp-open/hello \
    image flash run
```

Expected result:

```text
Hello from FC
```

---

## 21. Troubleshooting Summary

| Problem | Cause | Solution |
|---|---|---|
| `Unable to locate package sphinx-build` | Ubuntu 24.04 does not provide an APT package named `sphinx-build` | Install `python3-sphinx` |
| `gcc: command not found` | Fresh WSL installation lacks compiler toolchain | Install `build-essential` |
| `git submodule ... -j8` prints usage | Installed Git did not accept compact option form | Use `--jobs 8` or `-j 8` |
| Python package management concerns on Ubuntu 24.04 | System Python is externally managed | Use `python3 -m venv .venv` |
| `No module named 'psutil'` | `gvrun` requires `psutil`, but it was not installed by the documented requirements | Run `pip install psutil` |
| `Unable to open binary examples/pulp-open/hello` | README path does not match current checkout | Use `pulp/examples/pulp-open/hello` |
| `Hello from FC` | Not an error | Successful execution on the simulated Fabric Controller |

---

## 22. Recommended Next Steps

After this installation sanity check, useful next steps are:

1. inspect the `pulp-open` architecture used by GVSoC;
2. understand how the Fabric Controller and compute cluster differ;
3. locate the source/build process for the shipped `hello` application;
4. run a program that uses the PULP compute cluster rather than only the FC;
5. enable traces and inspect executed instructions;
6. extract cycle counts;
7. inspect memory hierarchy statistics;
8. compile and run a custom C kernel;
9. move toward a small linear-layer or attention kernel.

These steps transition from **"GVSoC is installed"** to **"GVSoC can be used for architecture-level workload characterization."**

---

## 23. Quick Reproduction Checklist

For a fresh Ubuntu 24.04 WSL environment:

```bash
sudo apt update

sudo apt install -y \
    build-essential \
    git \
    doxygen \
    python3-pip \
    python3-venv \
    libsdl2-dev \
    curl \
    cmake \
    gtkwave \
    libsndfile1-dev \
    rsync \
    autoconf \
    automake \
    texinfo \
    libtool \
    pkg-config \
    libsdl2-ttf-dev \
    wget \
    python3-sphinx

mkdir -p ~/UREKA
cd ~/UREKA

git clone https://github.com/gvsoc/gvsoc.git
cd gvsoc

git submodule update --init --recursive --jobs 8

python3 -m venv .venv
source .venv/bin/activate

pip install --upgrade pip
pip install -r core/requirements.txt
pip install -r gapy/requirements.txt
pip install psutil

make all TARGETS=pulp-open

./install/bin/gvsoc \
    --target=pulp-open \
    --binary pulp/examples/pulp-open/hello \
    image flash run
```

Expected final output:

```text
Hello from FC
```


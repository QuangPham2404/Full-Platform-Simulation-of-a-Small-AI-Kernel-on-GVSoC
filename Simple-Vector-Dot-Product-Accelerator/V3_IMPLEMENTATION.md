# V3.1–V3.2 implementation and validation

## Implementation

CONTROL=1 snapshots all 32 input words, resets the accumulator/index, marks
BUSY and schedules the first MAC on the next DPC edge. Each callback performs
one uint32_t multiply-add, emits the accumulator signal and schedules the next
edge. Only step 16 publishes RESULT, STATUS=1 and DONE. Starts while BUSY are
ignored; writes to input registers apply to the next job. RESULT is cleared on
an accepted start, as before. The MMIO register map and five requester-cycle
latency are unchanged.

The CPU MAC uses custom-0 opcode 0x0b, funct3=0, funct7=0. It computes
`uint32_t(old(rd) + uint32_t(rs1) * uint32_t(rs2))`, zero-extended to RV64.
Its metadata has three input registers (including old rd) and one output.
GNU assembler `.insn r 0x0b, 0, 0, rd, rs1, rs2` avoids toolchain changes.
It operates only on CPU registers. No ISA subset is added.

## Required GVSoC changes

The only GVSoC core changes are:

- `core/models/cpu/iss/isa_gen/isa_riscv_gen.py`: add MAC to existing Rv64i
  instruction list, with old rd as input 2 (hidden from mnemonic operands).
- `core/models/cpu/iss/include/isa/rv64i.hpp`: add the wrapping MAC handler.

`v3_iss.patch` records the exact changes, relative to the core repository.
From the GVSoC root, apply once with `git -C core apply
/path/to/project/v3_iss.patch`; check first with `git -C core apply --check
/path/to/project/v3_iss.patch`. Do not reapply to an already patched checkout.
The custom opcode becomes available to existing RV64i configurations in this
checkout; the project-specific timing configuration lives in `my_system.py`.

`MacRiscv` mirrors the existing Riscv configuration and enables timed execution
and its existing scoreboard. Following Tutorial 12, output latency metadata is
2. In this checkout that metadata installs a stalled handler but does not set
`insn->latency`. Therefore `insn.set_latency(1)` is also necessary: the existing
`iss_exec_stalled_insn_fast` timestamps results at `latency + 1`, or two cycles.
No custom stalls or hazard detector are introduced. Independent instructions
can execute while a result is pending; an immediate consumer accounts for the
remaining cycle through the scoreboard.

## Reproduce in the GVSoC working project

The GVSoC-local Makefile, generated headers and build directory remain local.
Copy the project source files into the existing working tutorial directory,
apply the ISS patch above, then:

```sh
source /home/stvn/UREKA/gvsoc/.venv/bin/activate
cd /home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/Simple-Vector-Dot-Product-Accelerator
make all gvsoc
make run runner_args='--trace=soc/dcp/trace --trace=soc/cpu/insn --vcd --event=soc/dcp/.*' > /tmp/dpc-v3-run.log 2>&1
```

VCD is `build/work/all.vcd`; the GTKWave setup is `build/work/view.gtkw`.
Instruction and DPC text traces are captured in `/tmp/dpc-v3-run.log`.
The application checks CPU/DPC agreement, expected 136, CPU wraparound and a
fixed assembly sequence exposing dependent and independent instruction timing.

## Validation evidence (2026-10-31)

- Build and simulation succeeded. CPU custom MAC and DPC both returned 136.
- Parsed text and VCD independently: all 16 accumulator values were
  1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66, 78, 91, 105, 120, 136.
  Every pair of consecutive MAC events was separated by 20 ns.
- Final baseline start was at 2970 ns, first MAC at 2980 ns and DONE at
  3280 ns. There are 16 DPC compute edges, nominally 320 ns. GVSoC enqueue(1)
  uses the next clock edge: a start between edges gives a partial first period,
  so request-to-DONE is 310 ns here (310–320 ns for this CPU/DPC phase grid).
  An earlier phase-aligned run measured exactly 320 ns. This is clock phase,
  not a missing MAC step or an extra MMIO delay.
- The dependency probe trace enters its first MAC at CPU cycle 1782, its
  immediate dependent MAC at 1783 and the independent `c.li` at 1785.
  The dependent instruction accounts for one pending-result cycle internally.
  The next MAC enters at 1786 after that independent instruction; its consumer
  follows after another independent instruction without an extra stall.
  Trace timestamps are instruction-entry timestamps, not retirement times.
- A temporary edge-case binary used A[0]=UINT32_MAX, B[0]=3, changed A[1]
  to 999 after start and issued another start while BUSY. Both implementations
  returned the expected wrapped result 132; the DPC emitted only one BUSY/DONE
  job. This validates snapshot isolation, ignored busy starts and DPC overflow.
  Its log is `/tmp/dpc-v3-edge.log`; temporary source is `/tmp/dpc-v3-edge.c`.
- CPU overflow check: MAC(UINT32_MAX, 2, 3) returned 5.

Power tracing/modeling and multicore features remain deferred. This is a
functional clock/timing comparison, not a pipeline-throughput benchmark.

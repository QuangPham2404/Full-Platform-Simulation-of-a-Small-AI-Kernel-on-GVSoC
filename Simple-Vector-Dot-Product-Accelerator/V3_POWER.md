# V3.3: illustrative CPU and DPC power

The power-enabled and ordinary simulations both pass with CPU/DPC result 136.
Computation, MMIO, 100/50 MHz clocks, dependency timing and existing state and
accumulator traces are retained. No controller, gating or scaling is added.

## Model and limitations

All values below are **illustrative and uncalibrated**, for education, not
silicon measurements or a basis for hardware efficiency claims. The fixed
operating point is **1.2 V, 25 C**, matching this checkout's native defaults
(`engine/engine/include/vp/power/power.hpp`, VP_POWER_DEFAULT_VOLT/TEMP).
Single-point tables in `power_models.py` use volts, not millivolts. Frequency
is `any`; no voltage, temperature or frequency dependence is characterized.

| Component | Static leakage | Dynamic background | Event energy |
| --- | --- | --- | --- |
| CPU | 100 uW | 200 uW | 2 pJ per executed instruction |
| DPC | 50 uW | 100 uW | 5 pJ per MAC step |

The CPU reuses the existing ISS `power_models` interface, native instruction
group accounting, and background source. All instructions, including custom
MAC, share one energy group; this is a coarse activity-based model, not a
detailed microarchitectural estimate. Extra stall energy is explicitly zero;
background energy still accrues during stalls. The CPU's native start method
starts both background contributions. The DPC uses Tutorial 14 PowerSource
APIs and starts constant background contributions in `start()`. Each compute
callback accounts exactly one energy quantum. Power sources share their
component's native power trace; no custom power counter is introduced.

No additional GVSoC ISS/engine changes were needed for V3.3. The two prior
V3.1–V3.2 core modifications and `v3_iss.patch` are still required as described
in `V3_IMPLEMENTATION.md`.

## Reproduce and validate

Use the existing GVSoC-local Makefile and generated headers. Project files
`power_models.py` and `validate_power.py` must accompany the other sources.

```sh
source /home/stvn/UREKA/gvsoc/.venv/bin/activate
cd /home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/Simple-Vector-Dot-Product-Accelerator
make all gvsoc
make run runner_args='--power --vcd --event=.* --trace=soc/cpu/insn --trace=soc/dcp/trace' > /tmp/dpc-v33-run.log 2>&1
python3 validate_power.py --work-dir build/work --log /tmp/dpc-v33-run.log
```

Native artifacts are `build/work/power_report.csv`, `build/work/all.vcd` and
`build/work/view.gtkw`. Inspect `/soc/cpu` and `/soc/dcp` signals
`power_trace`, `dyn_power_trace`, `static_power_trace`, plus DPC `accumulator`
and `state`. Native reporting averages over the entire program, including
startup, MMIO, CPU checking and printf; these are not kernel-only averages.
Run the validator before another run overwrites those artifacts.

The validator checks result 136, the existing arithmetic checks, all 16 text
and VCD partial sums, 20 ns step spacing, all 20 custom MAC executions (16
baseline + 3 dependency probe + 1 overflow check), power trace presence,
static leakage, CPU per-instruction energy, DPC power alignment and integrated
MAC energy, and component and combined power arithmetic. It fails on missing
or inconsistent required artifacts.

## Verified results (2026-10-31)

Build, power simulation, validator and separate ordinary simulation passed.
The power run lasts 44.05 us and has 3421 CPU instruction trace entries:
CPU instruction energy is 6842 pJ, and DPC MAC energy is 80 pJ.

| Component | P_static (W) | P_dynamic (W) | P_total (W) |
| --- | ---: | ---: | ---: |
| CPU | 0.000100000000 | 0.000355323496 | 0.000455323496 |
| DPC | 0.000050000000 | 0.000101816118 | 0.000151816118 |
| CPU+DPC | 0.000150000000 | 0.000457139614 | 0.000607139614 |

Combined figures are the sums of the CPU/DPC component rows, **not full-SoC
power**. RAM, interconnect and other components are not characterized here;
zero entries do not mean physical zero consumption.

VCD shows DPC dynamic background 100 uW rising to 350 uW for each MAC cycle
(5 pJ / 20 ns = 250 uW incremental power). All 16 contributions occur at the
same timestamps as accumulator steps 1, 3, 6, ..., 136, from 2980 to 3280 ns.
Consecutive contributions form a continuous plateau; at 3300 ns it returns
to background. Integrating excess power yields exactly 80 pJ. CPU instruction
activity is visible in its native dynamic trace, and both leakage traces are
constant at their configured values.

### Native parent reporting boundary

The native `/soc/power_trace` dynamic row is 0.000457094211 W, which is
45.403 nW below the component sum. At termination the last 2 pJ CPU quantum
is included in the CPU event-energy report but its full cycle has not elapsed
in the parent's propagated-power integration. This follows from local
`PowerTrace::inc_dynamic_energy` and the shutdown reporting boundary.
The validator displays that delta; it deliberately computes CPU+DPC totals
from native component rows, preserving the discrete-event energy accounting.
No engine rewrite or artificial simulation delay is used to conceal it.

# V3 Implementation Handoff — Sequential MAC + Custom CPU Instruction

## Goal and scope

Implement the agreed V3.1–V3.2 improvements to the existing mini dot-product accelerator. Keep changes surgical and close to GVSoC Tutorials 11–12. **Power modeling/tracing is deferred** to V3.3; do not implement it in this task. Do not add multicore support, a new ISA subset, DMA, or unrelated architectural features.

## Paths and workflow

- **Source-of-truth project:** `/home/stvn/UREKA/Full-Platform-Simulation-of-a-Small-AI-Kernel-on-GVSoC/Simple-Vector-Dot-Product-Accelerator`
- **GVSoC working/simulation project:** `/home/stvn/UREKA/gvsoc/engine/docs/developer_manual/tutorials/Simple-Vector-Dot-Product-Accelerator`
- **GVSoC root:** `/home/stvn/UREKA/gvsoc`

1. **Before editing**, inspect both project directories and synchronize the project-owned files **from the source-of-truth directory into the GVSoC working directory** so they start at the same version. Sync only corresponding project files. Preserve GVSoC-local build scaffolding, Makefiles, generated artifacts, environment files, and other unrelated files; do not replace or copy the entire GVSoC directory.
2. Work and test in the GVSoC checkout. Before building, activate the environment with:
   `source /home/stvn/UREKA/gvsoc/.venv/bin/activate`
3. You may inspect the source repository, GVSoC source tree, and [official documentation](https://gvsoc-developer.readthedocs.io/) as needed. Prefer the local GVSoC source/API and its existing examples.
4. Once the build, simulation, and tracing checks succeed, sync **only the changed project-owned source files** back to the source-of-truth project. GVSoC ISS source changes remain in the GVSoC checkout; document the exact changed paths and required changes so they are reproducible. Do not copy GVSoC infrastructure into the project repository.
5. Review the resulting diff; commit and push the source-of-truth repository to its configured GitHub remote. Never force-push. Report the commit and the GVSoC-local modifications.

## V3.1 — Sequential DPC MAC engine

- Preserve the current 16-element, 32-bit input vectors, MMIO register map, `CONTROL`/`STATUS`/`RESULT` interface, and CPU-to-DPC transaction flow.
- A write of `CONTROL = 1` starts the operation as before. The control-register callback initializes accumulator/index/state and schedules computation; it must **not** calculate all 16 products at once.
- A DPC clock-event callback performs **one** operation per DPC cycle: `acc = acc + A[i] * B[i]`, then advances `i`. Sixteen steps produce the final result; update `RESULT`, `STATUS`, and DONE only after the final step.
- Use the existing 50 MHz DPC clock. Nominal compute duration: **16 DPC cycles = 320 ns**, excluding bus and optional startup overhead. Keep arithmetic consistent with existing `uint32_t` wraparound semantics.
- Handle a start command while BUSY predictably (e.g., ignore it); avoid scope creep into queuing or complex control logic. Keep input values stable for the duration of a job, by a simple snapshot or an equally clear documented policy.

## V3.1 — CPU custom MAC instruction (Tutorial 11)

- Add one custom instruction to the **existing RV64 CPU ISS configuration**, without defining a separate ISA subset.
- Intended syntax/semantics: `mac rd, rs1, rs2` means `rd = old(rd) + rs1 * rs2` (word-width arithmetic consistent with the DPC test). Use a non-conflicting custom instruction encoding and a suitable execution handler.
- The custom instruction operates on **CPU architectural registers only**. It does not access DPC MMIO registers. The CPU and DPC are independent implementations of the same arithmetic for this educational comparison.
- Ensure the ISS metadata/implementation correctly accounts for reading the old `rd` as well as writing the new `rd`. Adapt the documented Tutorial 11 example to the actual RV64 ISS and available build toolchain.
- Exercise the instruction on the same vector data as the DPC and compare final results. Keep the test simple and easy to reproduce.

## V3.2 — Timing and tracing (Tutorial 12)

- Keep the existing 100 MHz CPU clock, 50 MHz DPC clock, and existing **5-cycle MMIO request latency**.
- DPC: schedule one MAC per DPC clock cycle, updating the existing `accumulator` signal after **every** step; keep the existing `state` signal (IDLE/BUSY/DONE). Do not add extra traces unless essential for debugging.
- CPU custom `mac`: assign a **2-cycle result latency**, using GVSoC's existing instruction timing/dependency machinery. This is **not an unconditional 2-cycle stall per instruction**. Verify dependent uses of the accumulator behave as supported by the chosen CPU timing model. Do not implement a custom hazard detector.
- Enable CPU instruction traces to demonstrate custom `mac` execution, and DPC VCD/system traces to show progressive accumulation and final state.

## Validation and handoff

Success means: **build → simulate with traces → confirm matching CPU/DPC results and visible MAC progress**. Use the current vectors (`A = 1..16`, `B = all ones`) as a baseline; expected result is **136**. Check that the DPC accumulator progresses `1, 3, 6, ... , 136` rather than jumping only at completion. Check the custom CPU instruction executes and its timing/dependency behavior is observable. Retain or adjust existing build commands as appropriate to the actual local checkout.

Avoid unnecessary refactors, extra components, broad rewrites, or overengineered test infrastructure. If an assumption about the local ISS/API proves incorrect, make the smallest suitable adaptation and explain it in the final report. After successful validation, sync the project files back, commit, and push; report tests run, trace evidence, affected ISS files, and any limitations. If validation fails, report the blocker and do **not** push an unverified implementation as complete.

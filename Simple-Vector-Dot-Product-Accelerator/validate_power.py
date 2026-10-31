"""Validate native V3.3 traces and report illustrative CPU+DPC averages."""
import argparse
from collections import defaultdict
from math import isclose
from pathlib import Path
import re
import power_models as model


def check(ok, message):
    if not ok:
        raise ValueError(message)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--work-dir', type=Path, default=Path('build/work'))
    parser.add_argument('--log', type=Path, required=True)
    args = parser.parse_args()
    log = args.log.read_text()
    for message in ('[CPU] Custom MAC result = 136', '[CPU] Result = 136',
                    '[CPU] MAC dependency probe and wraparound check passed.'):
        check(message in log, 'Missing success message: '+message)
    check('[CPU] FAIL' not in log, 'Application failed')
    steps = [(int(t), int(i), int(v)) for t, i, v in re.findall(
        r'^(\d+):.*DPC MAC: step=(\d+) accumulator=(\d+)', log, re.M)]
    check([i for _, i, _ in steps] == list(range(1, 17)), 'Expected 16 MAC steps')
    check([v for _, _, v in steps] == [i*(i+1)//2 for i in range(1, 17)],
          'Incorrect partial sums')
    check(all(steps[i+1][0]-steps[i][0] == 20000 for i in range(15)), 'MAC period changed')
    check(len(re.findall(r'\bmac\s+', log)) == 20, 'Expected 20 CPU MAC executions')
    scope, names, events = [], {}, defaultdict(list)
    timestamp = 0
    with (args.work_dir / 'all.vcd').open() as stream:
        for line in stream:
            fields = line.split()
            if line.startswith('$timescale'):
                check(fields[1] == '1ps', 'Expected picosecond VCD timescale')
            elif line.startswith('$scope'):
                scope.append(fields[2])
            elif line.startswith('$upscope'):
                scope.pop()
            elif line.startswith('$var'):
                names[fields[3]] = '/' + '/'.join(scope + [fields[4]])
            elif line.startswith('#'):
                timestamp = int(line[1:])
            elif line.startswith(('b', 'r')) and len(fields) == 2:
                name = names.get(fields[1], '')
                if name.startswith(('/soc/cpu/', '/soc/dcp/')):
                    if line[0] == 'r':
                        value = float(fields[0][1:])
                    elif any(c in fields[0].lower() for c in 'xz'):
                        continue
                    else:
                        value = int(fields[0][1:], 2)
                    events[name].append((timestamp, value))
    check([(t, v) for t, v in events['/soc/dcp/accumulator'] if v] ==
          [(t, v) for t, _, v in steps], 'VCD partial sums differ from text trace')
    dyn = events['/soc/dcp/dyn_power_trace']
    peak = model.DPC_BACKGROUND_W + model.DPC_MAC_PJ / 20000
    pulses = [(t, v) for t, v in dyn if v > model.DPC_BACKGROUND_W + 1e-9]
    check([t for t, _ in pulses] == [t for t, _, _ in steps], 'Energy/step alignment failed')
    check(all(isclose(v, peak, abs_tol=1e-9) for _, v in pulses), 'Wrong MAC event power')
    energy_pj = sum((v-model.DPC_BACKGROUND_W)*(t1-t0)
                    for (t0, v), (t1, _) in zip(dyn, dyn[1:]))
    check(isclose(energy_pj, 16*model.DPC_MAC_PJ, abs_tol=1e-6), 'Expected 80 pJ MAC energy')
    rows = {}
    for line in (args.work_dir / 'power_report.csv').read_text().splitlines():
        fields = [f.strip() for f in line.split(';')]
        if fields[0].startswith('/') and len(fields) >= 4:
            rows[fields[0]] = tuple(float(v) for v in fields[1:4])
    cpu, dpc = rows['/soc/cpu/power_trace'], rows['/soc/dcp/power_trace']
    for component, row, static in [('cpu', cpu, model.CPU_STATIC_W),
                                    ('dcp', dpc, model.DPC_STATIC_W)]:
        check(isclose(row[0]+row[1], row[2], abs_tol=2e-12), 'Component total mismatch')
        check(isclose(row[1], static, abs_tol=1e-12), 'Leakage mismatch')
        for suffix in ('power_trace', 'dyn_power_trace', 'static_power_trace'):
            check(events[f'/soc/{component}/{suffix}'], 'Missing power VCD trace')
    instruction_count = sum('/soc/cpu/insn ' in line for line in log.splitlines())
    check(instruction_count > 0, 'Missing CPU instruction traces')
    check(isclose(cpu[0], model.CPU_BACKGROUND_W +
                  instruction_count*model.CPU_INSN_PJ/timestamp, abs_tol=2e-12),
          'CPU instruction energy/report mismatch')
    check(isclose(dpc[0], model.DPC_BACKGROUND_W + 80/timestamp, abs_tol=2e-12),
          'DPC average energy/report mismatch')
    combined = tuple(a+b for a, b in zip(cpu, dpc))
    check(isclose(combined[0]+combined[1], combined[2], abs_tol=3e-12), 'Combined mismatch')
    print('Illustrative, uncalibrated model: 1.2 V, 25 C; whole-run averages')
    print('Component      P_static (W)    P_dynamic (W)   P_total (W)')
    for label, row in [('CPU', cpu), ('DPC', dpc), ('CPU+DPC', combined)]:
        print(f'{label:12} {row[1]:.12f}  {row[0]:.12f}  {row[2]:.12f}')
    delta = rows['/soc/power_trace'][0]-combined[0]
    print(f'Native parent dynamic minus component sum: {delta:.12g} W')
    print('Combined values are component sums, not full-SoC power.')
    print('PASS: result 136; 16 MACs / 80 pJ; partial sums, power VCD and averages')


if __name__ == '__main__':
    main()

"""Illustrative, uncalibrated power parameters at fixed 1.2 V and 25 C.

These deliberately coarse educational values are not silicon measurements.
Voltage and temperature match this checkout's VP_POWER_DEFAULT_* constants.
"""
VOLTAGE_V = 1.2
TEMPERATURE_C = 25
CPU_STATIC_W = 100e-6
CPU_BACKGROUND_W = 200e-6
CPU_INSN_PJ = 2.0
DPC_STATIC_W = 50e-6
DPC_BACKGROUND_W = 100e-6
DPC_MAC_PJ = 5.0


def characteristic(value, unit):
    return {"type": "linear", "unit": unit,
            "values": {str(TEMPERATURE_C): {str(VOLTAGE_V): {"any": value}}}}


def background(static_w, dynamic_w):
    return {"leakage": characteristic(static_w, "W"),
            "dynamic": characteristic(dynamic_w, "W")}


def energy(pj):
    return {"dynamic": characteristic(pj, "pJ")}


def cpu_model():
    # One native instruction group for all opcodes, including custom MAC.
    # Zero extra stall energy: no unsupported per-stall characterization.
    return {"background": background(CPU_STATIC_W, CPU_BACKGROUND_W),
            "insn_groups": [energy(CPU_INSN_PJ)],
            "stall_first": energy(0), "stall_next": energy(0)}

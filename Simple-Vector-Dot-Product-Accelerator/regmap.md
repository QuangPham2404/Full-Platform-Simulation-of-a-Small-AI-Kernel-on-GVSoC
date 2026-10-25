# Dot Product Component (DPC)

## Description

Memory-mapped register interface for the DPC. The interconnect maps the DPC at CPU-visible base address `0x20000000` and removes that base before forwarding requests, so the offsets below are DPC-local byte offsets.

V2.1 preserves the V1 behavior: all registers are 32 bits, all start at zero, and writing the complete 32-bit value `0x00000001` to CONTROL triggers computation. The RESERVED bits are documented as unused, but the full-register equality check means a write with any other bit set does **not** start computation. STATUS reports completion with value `1`; RESULT holds the computed dot product.

## Registers

| Register Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| A0 | 0x00 | 32 | 0x00000000 | Element 0 of vector A |
| A1 | 0x04 | 32 | 0x00000000 | Element 1 of vector A |
| A2 | 0x08 | 32 | 0x00000000 | Element 2 of vector A |
| A3 | 0x0C | 32 | 0x00000000 | Element 3 of vector A |
| B0 | 0x10 | 32 | 0x00000000 | Element 0 of vector B |
| B1 | 0x14 | 32 | 0x00000000 | Element 1 of vector B |
| B2 | 0x18 | 32 | 0x00000000 | Element 2 of vector B |
| B3 | 0x1C | 32 | 0x00000000 | Element 3 of vector B |
| CONTROL | 0x20 | 32 | 0x00000000 | Start command; computation starts only if the entire 32-bit value equals 1 |
| STATUS | 0x24 | 32 | 0x00000000 | Completion status; value 1 indicates done |
| RESULT | 0x28 | 32 | 0x00000000 | 32-bit dot-product result |

### A0

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 0 of vector A |

### A1

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 1 of vector A |

### A2

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 2 of vector A |

### A3

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 3 of vector A |

### B0

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 0 of vector B |

### B1

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 1 of vector B |

### B2

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 2 of vector B |

### B3

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 3 of vector B |

### CONTROL

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| SIGNAL | 0 | 1 | 0x0 | Start-command bit; V1-compatible logic checks the entire CONTROL register for value 1 |
| RESERVED | 1 | 31 | 0x0 | Unused upper bits; keep zero when issuing the start command |

### STATUS

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DONE | 0 | 1 | 0x0 | 1 indicates computation completed |
| RESERVED | 1 | 31 | 0x0 | Unused upper bits |

### RESULT

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | 32-bit dot-product result |

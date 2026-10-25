# Dot Product Component (DPC)

## Description

V2.2 register map for two 16-element vectors of unsigned 32-bit integers. CPU-visible base address: `0x20000000`. The interconnect removes the base address before forwarding accesses, so these are DPC-local **byte offsets**.

Each register is 32 bits (4 bytes), initialized to zero. The CONTROL callback starts computation only if the entire register equals `0x00000001`. STATUS is 1 when done, and RESULT holds the 32-bit dot product (arithmetic modulo 2^32).

## Registers

| Register Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| A0 | 0x00 | 32 | 0x00000000 | Element 0 of vector A |
| A1 | 0x04 | 32 | 0x00000000 | Element 1 of vector A |
| A2 | 0x08 | 32 | 0x00000000 | Element 2 of vector A |
| A3 | 0x0C | 32 | 0x00000000 | Element 3 of vector A |
| A4 | 0x10 | 32 | 0x00000000 | Element 4 of vector A |
| A5 | 0x14 | 32 | 0x00000000 | Element 5 of vector A |
| A6 | 0x18 | 32 | 0x00000000 | Element 6 of vector A |
| A7 | 0x1C | 32 | 0x00000000 | Element 7 of vector A |
| A8 | 0x20 | 32 | 0x00000000 | Element 8 of vector A |
| A9 | 0x24 | 32 | 0x00000000 | Element 9 of vector A |
| A10 | 0x28 | 32 | 0x00000000 | Element 10 of vector A |
| A11 | 0x2C | 32 | 0x00000000 | Element 11 of vector A |
| A12 | 0x30 | 32 | 0x00000000 | Element 12 of vector A |
| A13 | 0x34 | 32 | 0x00000000 | Element 13 of vector A |
| A14 | 0x38 | 32 | 0x00000000 | Element 14 of vector A |
| A15 | 0x3C | 32 | 0x00000000 | Element 15 of vector A |
| B0 | 0x40 | 32 | 0x00000000 | Element 0 of vector B |
| B1 | 0x44 | 32 | 0x00000000 | Element 1 of vector B |
| B2 | 0x48 | 32 | 0x00000000 | Element 2 of vector B |
| B3 | 0x4C | 32 | 0x00000000 | Element 3 of vector B |
| B4 | 0x50 | 32 | 0x00000000 | Element 4 of vector B |
| B5 | 0x54 | 32 | 0x00000000 | Element 5 of vector B |
| B6 | 0x58 | 32 | 0x00000000 | Element 6 of vector B |
| B7 | 0x5C | 32 | 0x00000000 | Element 7 of vector B |
| B8 | 0x60 | 32 | 0x00000000 | Element 8 of vector B |
| B9 | 0x64 | 32 | 0x00000000 | Element 9 of vector B |
| B10 | 0x68 | 32 | 0x00000000 | Element 10 of vector B |
| B11 | 0x6C | 32 | 0x00000000 | Element 11 of vector B |
| B12 | 0x70 | 32 | 0x00000000 | Element 12 of vector B |
| B13 | 0x74 | 32 | 0x00000000 | Element 13 of vector B |
| B14 | 0x78 | 32 | 0x00000000 | Element 14 of vector B |
| B15 | 0x7C | 32 | 0x00000000 | Element 15 of vector B |
| CONTROL | 0x80 | 32 | 0x00000000 | Start command; full 32-bit value must equal 1 |
| STATUS | 0x84 | 32 | 0x00000000 | Completion status; value 1 indicates done |
| RESULT | 0x88 | 32 | 0x00000000 | 32-bit dot-product result |

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

### A4

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 4 of vector A |

### A5

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 5 of vector A |

### A6

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 6 of vector A |

### A7

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 7 of vector A |

### A8

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 8 of vector A |

### A9

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 9 of vector A |

### A10

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 10 of vector A |

### A11

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 11 of vector A |

### A12

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 12 of vector A |

### A13

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 13 of vector A |

### A14

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 14 of vector A |

### A15

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 15 of vector A |

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

### B4

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 4 of vector B |

### B5

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 5 of vector B |

### B6

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 6 of vector B |

### B7

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 7 of vector B |

### B8

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 8 of vector B |

### B9

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 9 of vector B |

### B10

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 10 of vector B |

### B11

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 11 of vector B |

### B12

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 12 of vector B |

### B13

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 13 of vector B |

### B14

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 14 of vector B |

### B15

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | Element 15 of vector B |

### CONTROL

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| SIGNAL | 0 | 1 | 0x0 | Start bit; callback checks full CONTROL value equals 1 |
| RESERVED | 1 | 31 | 0x0 | Keep zero for valid start command |

### STATUS

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DONE | 0 | 1 | 0x0 | Computation complete when 1 |
| RESERVED | 1 | 31 | 0x0 | Unused bits |

### RESULT

#### Fields

| Field Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| DATA | 0 | 32 | 0x00000000 | 32-bit dot-product result |

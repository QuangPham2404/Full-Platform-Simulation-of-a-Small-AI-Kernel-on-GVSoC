# Dot Product Component (DPC)

## Description

This is the regmap for internal register mapping for the DPC, mapping internal registers for operands, control signals, and the output signals for the DPC. The register space starts at base adress of 0x20000000

## Registers

| Register Name | Offset | Size | Default | Description |
| --- | --- | --- | --- | --- |
| A0 | 0x00 | 32 | 0x0000000 | component 0 of vector A |
| A1 | 0x04 | 32 | 0x0000000 | component 1 of vector A |
| CONTROL | 0x20 | 32 | 0x0000000 | Control signal - 1 means start computing, 0 means do not do anything |
| STATUS | 0x24 | 32 | 0x0000000 | Status signal - 1 means computation complete, 0 means not-completed |

###  A0

#### Fields 

| Field Name | Offset | Size  | Default | Description |
| ---        | ---    | ---   | ---     | ---         |
| DATA | 0 | 32 | 0x0000000 | Only 1 field storing the 32-bit number for the whole entry |

### CONTROL

#### Fields

| Field Name | Offset | Size  | Default | Description |
| ---        | ---    | ---   | ---     | ---         |
| SIGNAL | 0 | 1 | 0x0000000 | 1 means TRUE, 0 means FALSE. Default FALSE (0) |
| BUFFER | 0 | 31 | 0x0000000 | We only 1 bit, the rest of the 31 bits are unused and must be logically left at 0x000000 so that the signal have the binary meaning |
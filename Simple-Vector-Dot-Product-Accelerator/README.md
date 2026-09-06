# GVSoC Mini Dot-Product Accelerator

## 1. Motivation and Summary

This project is a small milestone project for consolidating the foundational GVSoC concepts learned so far (Tutorial 1 to Tutorial 4) before progressing to more advanced tutorials and the larger URECA project.

The main concepts covered are:

* Building a custom GVSoC system and its component tree
* Creating custom components
* Connecting components through ports and an interconnect
* Sending and receiving requests between components
* Adding custom traces as well as vcd traces for observing component behaviour

The project implements a simple **4-element vector dot-product accelerator**.

Given two input vectors:

$$
A = [a_0, a_1, a_2, a_3]
$$

$$
B = [b_0, b_1, b_2, b_3]
$$

the accelerator calculates:

$$
A \cdot B =
a_0b_0 +
a_1b_1 +
a_2b_2 +
a_3b_3
$$

The CPU sends both vectors to a custom **Dot Product Component (DPC)** in a single request. The DPC performs the computation and returns the final scalar result to the CPU.

This provides a simplified model of CPU–accelerator interaction while introducing a computation primitive that is directly relevant to the larger URECA project. Dot products form the basis of matrix-vector multiplication, matrix multiplication, linear layers, and many AI workloads.

---

## 2. Component Tree

The system contains four main components:

```text
System
├── CPU
├── Interconnect
├── Register
└── Dot Product Component (DPC) # This is the custom component
```

### CPU

The CPU acts as the requester and controller of the system.

Responsibilities:

* Holds the two input vectors
* Constructs the dot-product request
* Sends the request through the interconnect
* Receives the result from the DPC
* Writes the final result into the register
* Prints the final result and completion status

### Interconnect

The interconnect provides the communication path between the CPU and the Dot Product Component.

Its purpose is to model the routing of requests and responses between system components.

### Register

The register stores the final result received by the CPU.

For this milestone, it is intentionally kept simple and mainly serves as an additional storage component within the system.

### Dot Product Component

The DPC is the custom compute accelerator.

Responsibilities:

* Receive both 4-element vectors in a single request
* Print a debug message confirming that the request was received
* Perform the dot-product computation
* Generate useful internal traces
* Return the result and a success status to the CPU

---

## 3. System Flow

The expected execution flow is:

```text
          Dot-product request
         {A[4], B[4]}
               │
               ▼
          ┌─────────┐
          │   CPU   │
          └────┬────┘
               │
               ▼
       ┌──────────────┐
       │ Interconnect │
       └──────┬───────┘
              │
              ▼
      ┌───────────────────┐
      │ Dot Product       │
      │ Component (DPC)   │
      └─────────┬─────────┘
                │
                │ Calculate:
                │
                │ result =
                │ Σ A[i] × B[i]
                │
                ▼
       Result + Success
                │
                ▼
       ┌──────────────┐
       │ Interconnect │
       └──────┬───────┘
              │
              ▼
          ┌─────────┐
          │   CPU   │
          └────┬────┘
               │
               ▼
          ┌──────────┐
          │ Register │
          └──────────┘
```

The detailed flow is:

1. The CPU prepares two 4-element vectors.

2. The CPU packages both vectors into a single request.

3. The request is sent through the interconnect to the DPC.

4. The DPC receives the request and prints:

   ```text
   [DPC] Request received!
   ```

5. The DPC computes:

   $$
   result = \sum_{i=0}^{3} A_iB_i
   $$

6. The DPC returns:

   * the calculated result
   * a success/completion status

7. The CPU receives the response.

8. The CPU writes the result into the register.

9. The CPU prints the final result and completion message.

10. Custom traces are observed to verify the internal operation of the DPC.

Useful traces may include:

* request received
* operand A
* operand B
* current multiplication result
* accumulator value
* computation state
* done/success signal
* final result

---

## 4. Expected Success State

The project is considered successful when the complete CPU–accelerator transaction works correctly from beginning to end.

For example, given:

$$
A = [1,2,3,4]
$$

$$
B = [5,6,7,8]
$$

the expected result is:

$$
1(5)+2(6)+3(7)+4(8)=70
$$

A successful execution should therefore demonstrate:

```text
[DPC] Request received!
[DPC] Dot-product computation completed.

[CPU] Dot-product completed successfully.
[CPU] Result = 70
```

The final register value should also be:

```text
70
```

GTKWave/Custom tracing should provide sufficient traces to observe and explain the operation of the DPC during the computation.

The final learning objective is to be able to explain the complete sequence:

```text
CPU prepares job
        ↓
request routed through interconnect
        ↓
DPC receives operands
        ↓
DPC performs multiply-accumulate operations
        ↓
DPC returns result
        ↓
CPU receives result
        ↓
CPU stores and reports result
```

Completing this milestone demonstrates that the foundational GVSoC concepts learned so far can be integrated into a small but meaningful simulated accelerator system.

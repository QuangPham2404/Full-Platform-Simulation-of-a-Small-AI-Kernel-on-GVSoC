# Full-Platform-Simulation-of-a-Small-AI-Kernel-on-GVSoC

## Project overview
GVSoC [1] is a fast, cycle-accurate simulator widely used for RISC-V IoT processors and is central to the group's plan to characterise embodied-AI workloads at the architecture level. Before more advanced trace-driven studies can start, the team needs a well-documented internal setup that runs a small, well-understood AI kernel on GVSoC and produces sensible numbers. This URECA project ports a small MobileLLM-style [2] block and one attention kernel to GVSoC, produces baseline throughput and memory-hierarchy statistics, and writes them up as an internal reference. Key tasks:

- Install GVSoC [1] and reproduce one of its shipped examples on a PULP-style RISC-V system. 
- Port a small linear layer and one attention block (as C code) that mimic a MobileLLM-scale [2] transformer. 
- Collect per-kernel cycle counts, L1 cache miss ratios, and DMA traffic; produce plots. 
- Cross-check the GVSoC numbers against a rough analytical model (roofline) and document any divergence. 

Expected outcome: 
- A documented GVSoC setup and one sanity-checked characterisation of a small transformer kernel, ready to be extended to VLA workloads in later theses. 

References: 
- [1] Bruschi et al., "GVSoC," IEEE ICCD 2021. 
- [2] Liu et al., "MobileLLM: Optimizing Sub-Billion Parameter Language Models for On-Device Use Cases," ICML 2024, arXiv:2402.14905. 
- [3] Zhang et al., "1024 RV-Cores Shared-L1 Cluster," arXiv:2408.08882, 2024. 
- [4] Kim et al., "OpenVLA," CoRL 2024, arXiv:2406.09246.

## Documents and Links

Project master doc: https://docs.google.com/document/d/1LZnBdEvRDB4BJf7hiUtCkrMXXYdT8scSYpM83jVa9sc/edit?tab=t.0

# GVSoC Simulation Agentic Workflow (GSAW)

**Status: Initial proposal for supervisor discussion.**

GSAW is a proposed researcher-operated workflow for developing and running GVSoC simulations with AI coding agents. It aims to reduce time spent locating framework-specific APIs, implementing models, and debugging simulation code, while leaving architecture decisions and interpretation of results with the engineer.

The idea arose during the GVSoC tutorial and dot-product accelerator milestones of the URECA project *Full-Platform Simulation of a Small AI Kernel on GVSoC*. GSAW is proposed as a complementary workstream alongside the original SoC, AI-kernel, and roofline-analysis work.

## Documents

| File | Purpose |
| --- | --- |
| [TWO_WORKSTREAMS.md](TWO_WORKSTREAMS.md) | Motivation and proposed split of the URECA project into GSAW and the original SoC study |
| [GSAW_INITIAL_IDEA.md](GSAW_INITIAL_IDEA.md) | First-version architecture, workflow, proof of concept, and possible extensions |
| [MOCKUP_SKILLS.md](MOCKUP_SKILLS.md) | Initial draft of the GVSoC-specific knowledge guidance; a reference, not yet a finalized skill |

The immediate proposal is deliberately modest: start with GVSoC-specific knowledge guidance and an engineer-to-agent implementation/simulation loop. More advanced features such as MCP, RAG, and extended agent orchestration can be evaluated later if useful.

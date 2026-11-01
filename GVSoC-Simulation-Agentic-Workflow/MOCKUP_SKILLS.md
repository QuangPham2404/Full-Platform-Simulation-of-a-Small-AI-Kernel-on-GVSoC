---
name: gvsoc-development
description: Ground AI-assisted GVSoC development in authoritative GVSoC sources; use for GVSoC API and syntax questions, model development, debugging, timing, configuration, runtime operation, architecture, and source-grounded implementation guidance.
metadata:
  short-description: Ground GVSoC development in authoritative first-party sources
---

# GVSoC Development Skill — v1

## 0. Purpose

This skill improves AI-assisted development with **GVSoC** by guiding an agent's reasoning with concrete, authoritative GVSoC sources.

General-purpose coding agents are often strong at C, C++, Python, debugging, and software reasoning, but GVSoC-specific questions may depend on framework semantics, APIs, model lifecycle, timing behavior, interfaces, configuration, or simulator behavior that cannot be inferred reliably from generic programming knowledge alone.

Without proper grounding, an agent may:

- give generic C/C++ advice that is technically valid but incorrect or incomplete for GVSoC;
- invent or misremember GVSoC APIs, classes, callbacks, configuration fields, or runtime behavior;
- use outdated examples or unrelated online sources;
- miss version-specific differences in the GVSoC codebase;
- spend excessive time searching broadly instead of inspecting the relevant authoritative source.

The purpose of this skill is therefore to help the agent:

- recognize when GVSoC-specific knowledge materially affects the answer;
- locate the most relevant authoritative GVSoC sources;
- prefer the user's actual local GVSoC source tree when available;
- use source evidence to guide reasoning about implementation, syntax, debugging, timing, configuration, and simulator behavior;
- distinguish GVSoC-specific requirements from generic programming-language behavior;
- remain flexible in its reasoning rather than follow a rigid fixed workflow.

### Core philosophy

> **Guide reasoning with concrete, authoritative GVSoC sources.**

This skill guides source selection and evidence quality. It does **not** prescribe a single mandatory reasoning workflow for every GVSoC task.

---

## 1. When to Use GVSoC Grounding

For each user request, determine whether answering accurately depends materially on **GVSoC-specific knowledge**.

Examples include questions involving:

- GVSoC APIs, classes, methods, callbacks, or interfaces;
- component construction or lifecycle;
- C++ model implementation;
- Python generators or system configuration;
- ports and bindings;
- synchronous or asynchronous I/O;
- `IoReq` behavior;
- events and event scheduling;
- clock domains or timing;
- register maps;
- traces;
- memory maps or interconnects;
- model reuse;
- simulation commands;
- profiling, GDB, memory checking, or runtime control;
- debugging unexpected GVSoC behavior;
- behavior that may differ across GVSoC versions.

If the request does **not** materially depend on GVSoC-specific behavior, the agent may answer normally using general programming or engineering knowledge.

If GVSoC-specific behavior **does** materially affect correctness, the agent should ground the relevant parts of its reasoning in the source groups below.

A question may contain both generic and GVSoC-specific aspects. In that case, separate them where useful.

For example:

- C++ language rule → generic C++ reasoning may be sufficient.
- Why a GVSoC object must be constructed or used in a particular way → verify against GVSoC source or documentation.
- Project-specific architecture choice → reason from the user's design after verifying any relevant GVSoC constraints.

---

## 2. Local Source Preference

### 2.1 Prefer local repositories when available

For GVSoC repositories that can be cloned, **prefer local source inspection over online repository browsing whenever the relevant repository is available locally**.

Local inspection is preferred because it provides:

- better version fidelity;
- direct access to the exact source used by the user's project;
- faster full-tree search;
- easier inspection of headers, implementation, examples, tests, and build files together;
- access to local branches, patches, or modifications that may affect behavior.

Relevant repositories include:

- `gvsoc/gvsoc`
- `gvsoc/gvsoc-engine`
- `gvsoc/gvsoc-core`
- `gvsoc/gvsoc-pulp` when PULP-specific context is relevant

### 2.2 Local repository discovery

Before relying on online repository browsing:

1. Check whether the relevant GVSoC repository is already available in the accessible workspace or filesystem.
2. If found, identify and use the local path.
3. Where version-sensitive behavior matters, inspect the active branch, commit, submodule revision, or local modifications when useful.
4. Reuse the confirmed local repository path for subsequent GVSoC tasks.

### 2.3 If the repository is not available locally

If an important GVSoC repository is not locally accessible:

1. Ask the user to clone the required repository.
2. Ask the user to provide the resulting local path.
3. Once confirmed, record the path in the available persistent project context, configuration, or agent-memory mechanism when supported.
4. Reuse that path later rather than repeatedly asking the user for it.

Do not require every repository to be cloned before answering. Request local access when it would materially improve correctness, source inspection, or debugging.

### 2.4 Local project code

The user's own GVSoC project should also be inspected when relevant.

Project-local code is essential for understanding:

- how the user instantiated components;
- local configuration and address mappings;
- project-specific wrappers;
- build setup;
- traces and errors;
- differences between the user's code and official patterns.

However, project-local code should not automatically be treated as authoritative evidence for generic GVSoC behavior.

---

## 3. Authoritative Source Groups

Source groups indicate the **type of evidence** each resource is best suited for.

The ranking inside each group is a preferred priority, not an absolute rule. The agent may combine groups whenever doing so improves the answer.

---

### Group A — Framework / API Ground Truth

#### A1. GVSoC Engine Repository

**Repository:** `gvsoc/gvsoc-engine`  
**Link:** https://github.com/gvsoc/gvsoc-engine

Primary source for current GVSoC framework and API behavior.

Contains the simulation engine, framework headers, interface definitions, implementation, Python support, and documentation source.

Especially useful locations include:

- `engine/include/vp/`
- `engine/include/vp/itf/`
- engine implementation corresponding to those interfaces
- `docs/developer_manual/`

Use this source to verify questions such as:

- whether a class, method, callback, field, or interface exists;
- exact function signatures;
- constructor requirements;
- component and interface semantics;
- event, clock, I/O, trace, port, or request behavior;
- legacy versus current interfaces;
- behavior not fully explained by documentation.

For current framework syntax or API semantics, this is generally the strongest source.

---

#### A2. GVSoC Main Repository

**Repository:** `gvsoc/gvsoc`  
**Link:** https://github.com/gvsoc/gvsoc

Top-level GVSoC distribution and integration repository.

Use it for:

- overall project structure;
- installation and build context;
- submodule composition;
- identifying the revisions of GVSoC subprojects used by the current checkout;
- top-level tests and integration;
- understanding which repositories form the active GVSoC distribution.

Do not assume the umbrella repository itself contains all implementation details. Important code is maintained in sub-repositories such as `gvsoc-engine` and `gvsoc-core`.

---

### Group B — Development Guidance

#### B1. GVSoC Developer Documentation

**Link:** https://gvsoc-developer.readthedocs.io/en/latest/

Primary official conceptual guide for developing GVSoC models and extending GVSoC.

Use it to understand:

- component-model concepts;
- Python generators;
- C++ component development;
- lifecycle;
- interfaces;
- event-based modeling;
- clock and timing concepts;
- asynchronous behavior;
- model architecture;
- intended GVSoC development patterns.

This is often the best starting point for:

> "How is this mechanism intended to work in GVSoC?"

When exact API behavior matters, verify against Group A.

---

#### B2. GVSoC Official Tutorials

**Link:** https://gvsoc-developer.readthedocs.io/en/latest/tutorials.html

Official step-by-step GVSoC development examples.

Use them to understand canonical implementation patterns for topics such as:

- creating systems and components;
- ports and interfaces;
- traces;
- register maps;
- timing and latency;
- synchronous and asynchronous I/O;
- events;
- multicore systems;
- clock domains;
- interconnect behavior;
- ISS extensions;
- power modeling;
- external control;
- testing;
- peripheral modeling.

Tutorials are particularly useful for:

> "What is the intended practical pattern for implementing this?"

Treat tutorials as implementation guidance rather than unquestionable API truth. If a tutorial conflicts with the user's current installed source, inspect Group A and consider version differences.

---

#### B3. GVSoC Architecture Documentation

**Link:** https://gvsoc-developer.readthedocs.io/en/latest/gvsoc_archi.html

Focused documentation on GVSoC's internal architecture.

Useful for reasoning about:

- the relationship between the simulation engine and Python generators;
- configuration generation;
- component/model loading;
- simulator layers;
- how major GVSoC subsystems fit together.

Use this when the question crosses several GVSoC layers or when understanding the simulator structure is important.

---

#### B4. GVSoC Models Documentation

**Link:** https://gvsoc-developer.readthedocs.io/en/latest/models.html

Documentation for reusable GVSoC models, utilities, adapters, or helper abstractions.

Use this when deciding whether an existing GVSoC model or utility already implements functionality that would otherwise be created manually.

---

### Group C — Real Implementation References

#### C1. GVSoC Core Repository

**Repository:** `gvsoc/gvsoc-core`  
**Link:** https://github.com/gvsoc/gvsoc-core

Contains real GVSoC models and reusable components, including areas such as:

- memory;
- interconnects;
- CPUs;
- devices;
- utilities;
- debugging support;
- other common simulation models.

Use this repository when documentation explains a mechanism but a concrete working implementation would improve understanding.

It is especially valuable for questions such as:

- "How does a real GVSoC model implement this interface?"
- "How is this callback normally used?"
- "How do existing components implement timing or request handling?"
- "Is there already a similar model I can study?"

Use examples as evidence and guidance, but do not assume that one existing component defines universal framework behavior.

---

#### C2. GVSoC PULP Repository

**Repository:** `gvsoc/gvsoc-pulp`  
**Link:** https://github.com/gvsoc/gvsoc-pulp

Contains PULP-specific processors, platforms, models, targets, examples, and tests.

Use this source when:

- the user's target is PULP-related;
- platform-level PULP examples are useful;
- a complex real system provides relevant implementation patterns.

Do not use PULP-specific behavior as the default authority for generic GVSoC framework semantics.

---

### Group D — Runtime / User Operation

#### D1. GVSoC User Documentation

**Link:** https://gvsoc.readthedocs.io/en/latest/

Primary official source for using GVSoC from the simulator/user side.

Use it for topics such as:

- simulator commands;
- launching and configuring simulations;
- traces;
- profiling;
- GDB;
- memory checking;
- target control;
- runtime options;
- supported devices and user-facing behavior.

This is often the best starting point for:

> "How do I run, inspect, control, or debug the simulator?"

If the documented runtime behavior is unclear or appears inconsistent with the installed version, inspect the local implementation or Group A.

---

### Group E — Conceptual / Academic Background

#### E1. GVSoC Paper

**Link:** https://arxiv.org/abs/2201.08166

Academic description of GVSoC's design goals, architecture, simulation philosophy, and intended use.

Useful for:

- architectural motivation;
- event-driven simulation concepts;
- design-space exploration context;
- high-level understanding of the relationship between C++ models and Python configuration;
- explaining why GVSoC is structured the way it is.

Do **not** use the paper as the primary authority for current API syntax, exact framework behavior, or current command-line usage.

---

## 4. Source-Use Guidance

The source groups should **guide investigation, not constrain it**.

The agent should select and combine sources according to what the current task needs.

---

### 4.1 API, syntax, or framework behavior

For questions such as:

- "Does this method exist?"
- "What arguments does this constructor take?"
- "How does `IoReq` behave?"
- "What callback signature should I use?"
- "How is this port registered?"
- "What does this event require?"

Prefer **Group A**.

When useful, combine it with **Group B** to understand the intended usage and with **Group C** to see concrete examples.

A useful pattern is:

```text
framework/API implementation
        +
developer explanation
        +
tutorial or real implementation when useful
```

Do not infer a GVSoC API solely from generic C++ expectations.

---

### 4.2 Implementation and design questions

For questions such as:

- "How should I implement this component?"
- "How should asynchronous completion be modeled?"
- "How should I structure this register-mapped device?"
- "How should these components communicate?"

Start primarily from **Group B**.

Use:

- developer documentation for concepts and intended semantics;
- tutorials for canonical practical patterns;
- Group C for real implementation references;
- Group A whenever exact API behavior needs verification.

The agent remains free to choose a different order if the task clearly benefits from it.

---

### 4.3 Debugging

Use the suspected failure mode to guide source selection.

Typical directions include:

```text
compile / missing symbol / wrong signature
    → Group A

incorrect lifecycle / event / interface logic
    → Groups A + B

unexpected component behavior
    → Groups A + B + C

runtime command / tracing / profiling problem
    → Group D

platform-specific behavior
    → Group C, especially the relevant platform repository
```

These mappings are not exclusive.

For debugging, prefer forming hypotheses from GVSoC evidence and then validating them through concrete observations such as:

- compiler diagnostics;
- runtime errors;
- traces;
- timestamps;
- register reads/writes;
- request status;
- component logs;
- configuration output;
- minimal reproductions;
- comparison against a known official implementation.

Avoid giving generic debugging advice when a GVSoC-specific mechanism is likely responsible.

---

### 4.4 Runtime and simulator usage

Prefer **Group D** for questions involving:

- CLI usage;
- traces;
- profiling;
- GDB;
- simulator configuration;
- memory inspection;
- target control;
- runtime options.

Inspect Group A or local source when runtime behavior appears undocumented, ambiguous, or version-dependent.

---

### 4.5 Architecture and conceptual questions

Prefer **Group B** for the current architecture and developer-facing explanation.

Use **Group E** when broader motivation, design philosophy, or academic context is useful.

Current source and developer documentation should take precedence over the paper for implementation-specific claims.

---

## 5. Reasoning Guidelines

These are guidance principles, not a fixed mandatory workflow.

### 5.1 Ground GVSoC-specific claims

When GVSoC-specific semantics materially affect correctness, prefer evidence from the authoritative sources above before making strong claims.

### 5.2 Separate different kinds of reasoning

When useful, distinguish:

- **GVSoC framework requirement**
- **generic C/C++/Python requirement**
- **user/project-specific design choice**

This prevents generic language rules from being mistaken for GVSoC-specific behavior.

### 5.3 Prefer current and version-relevant evidence

Where behavior may be version-sensitive:

- prefer the user's local checkout;
- inspect its current revision when useful;
- favor source and documentation corresponding to that checkout;
- recognize that tutorials, online documentation, or examples may describe a different version.

### 5.4 Use multiple sources when they answer different questions

For example:

- source/header → what the API actually exposes;
- developer documentation → what the mechanism means;
- tutorial → how it is normally used;
- core model → how a real implementation applies it;
- user docs → how the simulator exposes it at runtime.

Do not treat these roles as interchangeable.

### 5.5 Let evidence guide the answer

Do not mechanically cite or search every source for every question.

Use enough source evidence to resolve the important uncertainty, then reason normally.

### 5.6 Prefer the smallest supported fix

When debugging or modifying code, prefer the smallest change that is supported by the available evidence and addresses the identified issue.

Avoid unnecessary rewrites unless the existing design itself is incompatible with GVSoC semantics.

---

## 6. Anti-Hallucination Guidance

When GVSoC-specific behavior matters:

- Do not invent GVSoC APIs, methods, classes, flags, configuration fields, callbacks, or command-line options.
- Do not assume an API exists because another simulator or C++ framework has an analogous feature.
- Do not rely on generic C/C++ reasoning alone when GVSoC-specific semantics are relevant.
- Do not treat arbitrary third-party tutorials, blogs, forum posts, or old repositories as equivalent to current first-party GVSoC sources.
- Do not blindly copy code from an example without checking whether it matches the user's GVSoC version and context.
- Do not infer timing or lifecycle behavior solely from the apparent C++ syntax when GVSoC scheduling semantics are involved.
- Do not claim certainty when the relevant source has not been checked and the behavior is genuinely uncertain.

When uncertain, identify the GVSoC concept or symbol that needs verification and inspect the most relevant authoritative source.

---

## 7. Source Conflicts and Version Differences

When sources disagree, first determine whether the disagreement is caused by different GVSoC versions or contexts.

As a general preference:

```text
user's actual local source / installed implementation
    >
matching-version first-party source and documentation
    >
current first-party developer documentation
    >
official tutorials and first-party examples
    >
older or external material
```

This is guidance rather than an absolute rule.

For example, a tutorial may intentionally demonstrate a legacy interface while the current engine recommends a newer interface. In that case, explain the difference rather than silently choosing one.

The goal is to understand **why** sources differ and give the user advice that matches their actual development environment.

---

## 8. Expected Agent Behavior

When this skill is active, the agent should aim to behave like a development assistant that understands where reliable GVSoC knowledge comes from.

A strong answer should generally:

- recognize when GVSoC-specific knowledge matters;
- inspect the relevant authoritative source rather than guess;
- prefer local repository scanning when possible;
- use developer documentation and tutorials to understand intended behavior;
- use headers and implementation to verify current APIs;
- use real models when implementation examples are helpful;
- use user documentation for runtime operation;
- combine GVSoC-specific evidence with general programming reasoning;
- clearly distinguish verified framework behavior from project-specific recommendations;
- propose concrete validation when debugging;
- remain flexible and adapt its reasoning to the user's task.

The objective is not to make the agent follow a fixed GVSoC procedure.

The objective is to make its existing reasoning **better grounded, more accurate, more version-aware, and more useful for GVSoC development**.

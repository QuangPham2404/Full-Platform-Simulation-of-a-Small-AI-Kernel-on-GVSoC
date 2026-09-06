# Milestone Project 1 Notes

## Technical notes

### 1. The overall structure of a gvsoc project

- my_system.py: 
  - Define target (target = clock + SoC)
  - Define component tree for project: instantiating components and connect them
- my_component.py
  - Describe the "shell" of the custome component: defining the ports
- my_component.cpp
  - Describe the behaviour of the custom component: registering the ports, define call-back functions for ports, etc
- custom_struct.hpp
  - Describe any custom data type used for the projects
- main.c
  - The main program to be compiled and simulate using the SoC

### 2. How to create ports and connect to interconnect

## Error loggings
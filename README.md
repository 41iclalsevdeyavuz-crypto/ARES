# ARES — Autonomous Rescue & Exploration System
Building autonomous robots from software architecture to real-world exploration.

ARES (Autonomous Rescue & Exploration System) is a modular autonomous robotics platform developed to explore unknown environments, starting from software simulation and evolving into a real embedded robotic system.

The project began as a simple terminal-based room exploration simulation to strengthen object-oriented software design and finite state machine architecture. As the project evolved, its scope expanded beyond simulation into a long-term robotics platform targeting autonomous exploration in GPS-denied indoor environments.

Rather than focusing only on making the robot move, ARES emphasizes software architecture first. The navigation logic, sensing, mapping and motor control are intentionally separated into independent modules so that the same software can later be transferred to real hardware with minimal changes.

Current Capabilities

✓ Finite State Machine based navigation

✓ Dynamic Occupancy Grid Mapping

✓ Modular Object-Oriented Architecture

✓ Autonomous Sense → Decide → Act cycle

✓ Hardware abstraction layer for future Arduino integration

## Project Goal

The long-term goal of ARES is to build a small autonomous rover that can explore unknown indoor environments, avoid obstacles, and create a basic map of the explored area.

This makes the project relevant to scenarios such as:

- GPS-denied indoor navigation
- Search and rescue exploration
- Disaster-area scouting
- Embedded autonomous systems
- Robotics software architecture

---

## Current Version: V1 Terminal Simulation

V1 focuses on the software architecture before moving to hardware.

The robot currently runs in a terminal simulation where synthetic sensor values are injected and the system performs autonomous decision cycles.

### Implemented in V1

- Finite State Machine navigation
- Simulated front, left and right distance readings
- Simulated motor actions
- Dynamic occupancy grid map
- Map expansion when the robot reaches grid boundaries
- Object-oriented separation of responsibilities

---

## System Architecture

ARES is designed as a set of independent components coordinated by the Robot class.

```text
Robot
├── DistanceSensor
├── StateMachine
├── MotorController
└── OccupancyGridMap

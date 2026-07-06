# ARES — Autonomous Rescue & Exploration System

ARES is an autonomous indoor exploration rover project designed for GPS-denied environments.

The current version is a C++ terminal simulation that models the robot's decision-making architecture using:

- Finite State Machine based navigation
- Object-oriented C++ design
- Simulated distance sensing
- Simulated motor control
- Dynamic occupancy grid mapping
- Autonomous sense-decide-act cycles

---

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

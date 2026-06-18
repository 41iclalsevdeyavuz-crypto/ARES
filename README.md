# Autonomous-Room-Mapping-Rover
An autonomous robot capable of:

- Obstacle avoidance
- Indoor exploration
- Occupancy grid mapping
- Room size estimation
- Free area detection

## System Architecture

### Robot

The main controller of the system. It coordinates all components, manages the exploration process, and executes the robot's mission.

### StateMachine

Controls the robot's behavior using a finite state machine (FSM). It determines which action the robot should perform based on sensor inputs and current conditions.

### DistanceSensor

Measures distances to obstacles and provides environmental information used for navigation and decision-making.

### MotorController

Controls the robot's movement, including forward motion, backward motion, and turning maneuvers.

### OccupancyGridMap

Maintains a map of the explored environment. It stores information about free space, obstacles, and visited locations.


Status:
FSM V1 completed.

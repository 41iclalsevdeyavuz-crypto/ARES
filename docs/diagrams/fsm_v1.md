# FSM V1

## States

* IDLE
* SCAN_FRONT
* MOVE_FORWARD_STEP
* LOOK_LEFT_RIGHT
* TURN_LEFT
* TURN_RIGHT
* MOVE_BACKWARD_STEP
* UPDATE_MAP
* COMPLETED

## State Transitions

### IDLE

* Start → SCAN_FRONT

### SCAN_FRONT

* distance > 30 cm → MOVE_FORWARD_STEP
* distance <= 30 cm → SCAN_LEFT_RIGHT

### MOVE_FORWARD_STEP

* stepCompleted = 1 → UPDATE_MAP

### UPDATE_MAP

* hasBeenEverywhere = 0 → SCAN_FRONT
* hasBeenEverywhere = 1 → MISSION_COMPLETE

### SCAN_LEFT_RIGHT

* left > 30 && left > right → TURN_LEFT
* right > 30 && right >= left → TURN_RIGHT
* left <= 30 && right <= 30 → MOVE_BACKWARD_STEP

### TURN_LEFT

* turnCompleted = 1 → SCAN_FRONT

### TURN_RIGHT

* turnCompleted = 1 → SCAN_FRONT

### MOVE_BACKWARD_STEP

* backwardCompleted = 1 → SCAN_FRONT

### MISSION_COMPLETE

* Final state

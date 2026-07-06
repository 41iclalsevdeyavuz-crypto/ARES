#include<iostream>
#include<vector>
using namespace std;
enum State {
    IDLE,
    SCAN_FRONT ,
    MOVE_FORWARD_STEP,
    SCAN_LEFT_RIGHT,
    TURN_LEFT,
    TURN_RIGHT,
    MOVE_BACKWARD_STEP,
    UPDATE_MAP,
    MISSION_COMPLETED,
};
// Navigation threshold used by FSM to separate free space from obstacles.
// Keeping it as a named constant makes the behavior tunable without changing the state transition logic.
const int OBSTACLE_THRESHOLD = 30;
string stateToString(State state) {
    switch (state) {
        case IDLE: return "IDLE";
        case SCAN_FRONT: return "SCAN_FRONT";
        case MOVE_FORWARD_STEP: return "MOVE_FORWARD_STEP";
        case SCAN_LEFT_RIGHT: return "SCAN_LEFT_RIGHT";
        case TURN_LEFT: return "TURN_LEFT";
        case TURN_RIGHT: return "TURN_RIGHT";
        case MOVE_BACKWARD_STEP: return "MOVE_BACKWARD_STEP";
        case UPDATE_MAP: return "UPDATE_MAP";
        case MISSION_COMPLETED: return "MISSION_COMPLETED";
        default: return "UNKNOWN_STATE";
    }
}
// I separated this from MotorController on purpose:
// the FSM decides what should happen next, but it does not know how motors work.
// This keeps the navigation logic independent from hardware implementation.
class StateMachine {
    private:
        State currentState;
    public:
        StateMachine(){
            currentState = IDLE;
        }
        void changeState(State newState){
            currentState = newState;
        }
        State getCurrentState(){
            return currentState;
        }

        // Completion flags describe whether the previously requested action has finished.
        void update(int forwardDistance, int leftDistance, int rightDistance, bool stepCompleted, bool turnCompleted, bool backwardStepCompleted){
            switch (currentState) {
                case IDLE:
                    changeState(SCAN_FRONT);
                    break;
                case SCAN_FRONT:
                    if(forwardDistance > OBSTACLE_THRESHOLD){
                        changeState(MOVE_FORWARD_STEP);
                    }
                    else{
                        changeState(SCAN_LEFT_RIGHT);
                    }
                    break;
                case MOVE_FORWARD_STEP:
                    if(stepCompleted){
                        changeState(UPDATE_MAP);
                    }
                    break;
                case SCAN_LEFT_RIGHT:
                    if(leftDistance > OBSTACLE_THRESHOLD && leftDistance > rightDistance){
                        changeState(TURN_LEFT);
                    }
                    else if(rightDistance > OBSTACLE_THRESHOLD && rightDistance >= leftDistance){
                        changeState(TURN_RIGHT);
                    }
                    else{
                        changeState(MOVE_BACKWARD_STEP);
                    }
                    break;
                case TURN_LEFT:
                    if(turnCompleted){
                        changeState(SCAN_FRONT);
                    }
                    break;
                case TURN_RIGHT:
                    if(turnCompleted){
                        changeState(SCAN_FRONT);
                    }
                    break;
                case MOVE_BACKWARD_STEP:
                    if(backwardStepCompleted){
                        changeState(SCAN_FRONT);
                    }
                    break;
                case UPDATE_MAP:
                    changeState(SCAN_FRONT);
                    break;
                case MISSION_COMPLETED:
                    // Handle MISSION_COMPLETED state
                    break;
            }
        }
};    
// In V1, distances are injected manually to test the navigation logic.
// In the Arduino version, this interface can be replaced with real HC-SR04 readings without changing the StateMachine or Robot orchestration code.
class DistanceSensor {
    private:
        int forwardDistance;
        int leftDistance;
        int rightDistance;
    public:
        DistanceSensor(){
            forwardDistance = 0;
            leftDistance = 0;
            rightDistance = 0;
        }
        void setDistances(int forward, int left, int right) {
            forwardDistance = forward;
            leftDistance = left;
            rightDistance = right;
        }
        int getForwardDistance() {
            return forwardDistance;
        }
        int getLeftDistance() {
            return leftDistance;
        }
        int getRightDistance() {
            return rightDistance;
        }
}; 

// In the simulation, motor actions are represented by console output.
// In the hardware version, the same methods will map to Arduino motor driver commands.
class MotorController {
    private:
        int forwardStepCm;
        int backwardStepCm;
        int turnAngleDegrees;
    public:
        MotorController(int forwardStepCm, int backwardStepCm, int turnAngle){
            this->forwardStepCm = forwardStepCm;
            this->backwardStepCm = backwardStepCm;
            this->turnAngleDegrees = turnAngle;
        }
        bool moveForward() {
            cout << "Moving forward " << forwardStepCm << " cm." << endl;
            return true; 
        }
        bool moveBackward() {
            cout << "Moving backward " << backwardStepCm << " cm." << endl;
            return true; 
        }
        bool turnLeft() {
            cout << "Turning left " << turnAngleDegrees << " degrees." << endl;
            return true; 
        }
        bool turnRight() {
            cout << "Turning right " << turnAngleDegrees << " degrees." << endl;
            return true; 
        }
        void stop() {
            cout << "Stopping motors." << endl;
        }
};

// '?' = unknown
// '.' = explored free space
// 'X' = obstacle
// I initially considered asking the user for the room width and height, 
// then creating a fixed grid from those dimensions.
// Why I changed it:
// In a real exploration scenario, especially for ARES, the robot should not
// assume that it already knows the room boundaries. The map should grow as the
// robot moves and discovers new space.
class OccupancyGridMap {
private:
    vector<vector<char>> grid;
    int robotX;
    int robotY;
    int cellSizeCm;

    // I first thought of doubling the grid size, but that could create too much unknown
    // space that may not physically exist. For V1, I chose a simpler controlled
    // expansion: add one-cell padding around the current map and keep the old map centered.
    void expandMap() {
        const int OFFSET = 1;

        vector<vector<char>> newGrid(
            grid.size() + 2,
            vector<char>(grid[0].size() + 2, '?')
        );

        for (int row = 0; row < grid.size(); row++) {
            for (int col = 0; col < grid[row].size(); col++) {
                newGrid[row + OFFSET][col + OFFSET] = grid[row][col];
            }
        }

        grid = newGrid;

        robotX += OFFSET;
        robotY += OFFSET;
    }

public:
    OccupancyGridMap(int cellSizeCm) {
        const int INITIAL_MAP_SIZE = 5;

        this->cellSizeCm = cellSizeCm;

        grid = vector<vector<char>>(
            INITIAL_MAP_SIZE,
            vector<char>(INITIAL_MAP_SIZE, '?')
        );

        robotX = INITIAL_MAP_SIZE / 2;
        robotY = INITIAL_MAP_SIZE / 2;
    }

    void printMap() {
        cout << "\nCurrent Occupancy Grid Map:\n";

        for (int row = 0; row < grid.size(); row++) {
            for (int col = 0; col < grid[row].size(); col++) {
                if (row == robotY && col == robotX) {
                    cout << "R ";
                } else {
                    cout << grid[row][col] << " ";
                }
            }
            cout << endl;
        }

        cout << endl;
    }
    // The robot is not stored as 'R' inside the grid. Its position is tracked with
    // robotX and robotY, then rendered during printMap() to avoid storing the same 
    // information twice and prevents bugs where the grid says the robot is in 
    //one place but robotX/robotY say another.
    void moveRobot(int dx, int dy) {
        grid[robotY][robotX] = '.';

        int newX = robotX + dx;
        int newY = robotY + dy;

        if (newX < 0 || newX >= grid[0].size() ||
            newY < 0 || newY >= grid.size()) {
            
            expandMap();

            newX = robotX + dx;
            newY = robotY + dy;
        }

        robotX = newX;
        robotY = newY;
    }

    void markObstacle(int dx, int dy) {
        int obstacleX = robotX + dx;
        int obstacleY = robotY + dy;

        if (obstacleX < 0 || obstacleX >= grid[0].size() ||
            obstacleY < 0 || obstacleY >= grid.size()) {
            
            expandMap();

            obstacleX = robotX + dx;
            obstacleY = robotY + dy;
        }

        grid[obstacleY][obstacleX] = 'X';
    }
};
class Robot {
private:
    StateMachine stateMachine;
    DistanceSensor distanceSensor;
    MotorController motorController;
    OccupancyGridMap map;

public:
    Robot()
        : motorController(10, 5, 90), map(5) {
    }
  void runSimulation() {
    cout << "ARES V1 Terminal Simulation Started" << endl;

    map.printMap();

    // Scenario 1: front is clear -> move forward
    distanceSensor.setDistances(50, 0, 0);
    runOneCycle();

    // Scenario 2: front blocked, left is better -> turn left
    distanceSensor.setDistances(15, 60, 20);
    runOneCycle();

    // Scenario 3: front blocked, right is better -> turn right
    distanceSensor.setDistances(10, 20, 70);
    runOneCycle();

    // Scenario 4: all sides blocked -> move backward
    distanceSensor.setDistances(10, 15, 15);
    runOneCycle();

    // Scenario 5: force map expansion by moving forward several times
    for (int i = 0; i < 8; i++) {
        distanceSensor.setDistances(80, 0, 0);
        runOneCycle();
    }

    cout << "ARES V1 Terminal Simulation Finished" << endl;
}
// One autonomous decision cycle:
// read simulated sensor values, update the FSM, execute the selected action,
// then update the map if movement happened.
  void runOneCycle() {
    stateMachine.update(
        distanceSensor.getForwardDistance(),
        distanceSensor.getLeftDistance(),
        distanceSensor.getRightDistance(),
        false,
        false,
        false
    );

    cout << "Current State: " << stateToString(stateMachine.getCurrentState()) << endl;

    State currentState = stateMachine.getCurrentState();

    if (currentState == MOVE_FORWARD_STEP) {
        bool completed = motorController.moveForward();
        map.moveRobot(1, 0);

        stateMachine.update(
            distanceSensor.getForwardDistance(),
            distanceSensor.getLeftDistance(),
            distanceSensor.getRightDistance(),
            completed,
            false,
            false
        );
    }
    else if (currentState == TURN_LEFT) {
        bool completed = motorController.turnLeft();

        stateMachine.update(
            distanceSensor.getForwardDistance(),
            distanceSensor.getLeftDistance(),
            distanceSensor.getRightDistance(),
            false,
            completed,
            false
        );
    }
    else if (currentState == TURN_RIGHT) {
        bool completed = motorController.turnRight();

        stateMachine.update(
            distanceSensor.getForwardDistance(),
            distanceSensor.getLeftDistance(),
            distanceSensor.getRightDistance(),
            false,
            completed,
            false
        );
    }
    else if (currentState == MOVE_BACKWARD_STEP) {
        bool completed = motorController.moveBackward();
        map.moveRobot(-1, 0);

        stateMachine.update(
            distanceSensor.getForwardDistance(),
            distanceSensor.getLeftDistance(),
            distanceSensor.getRightDistance(),
            false,
            false,
            completed
        );
    }

    if (stateMachine.getCurrentState() == UPDATE_MAP) {
        stateMachine.update(
            distanceSensor.getForwardDistance(),
            distanceSensor.getLeftDistance(),
            distanceSensor.getRightDistance(),
            false,
            false,
            false
        );
    }

    cout << "Next State: " << stateToString(stateMachine.getCurrentState()) << endl;
    map.printMap();
}
};
int main() {
    Robot robot;
    robot.runSimulation();

    return 0;
}
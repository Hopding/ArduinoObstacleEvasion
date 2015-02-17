#include <NewPing.h>
#include <AFMotor.h>

#define TRIGGER_PIN 17
#define ECHO_PIN 19
#define MAX_DISTANCE 400

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

AF_DCMotor rightMotor(2, MOTOR12_1KHZ);
AF_DCMotor leftMotor(3, MOTOR34_1KHZ);

unsigned int distance;
int whichWay = 0;
int numbOfTurns = 0;
int state;
const int stateMoving = 0;
const int stateTurning = 1;
const int stateDoneTurning = 2;
const int REVERSE_TIME = 2000;
const int MIN_TURN_TIME = 3000;
const int MAX_TURN_TIME = 5000;
const int TO_CLOSE = 7;
const int SIGNAL_LOST = 190;

void setup() {
	Serial.begin(115200);
	rightMotor.setSpeed(255);
	leftMotor.setSpeed(255);
	move();
}

void loop() {
	distance = sonar.ping_cm();
	Serial.println(distance);
	if (moving()) {
		if (obstacleAhead()) {
			avoidObject();
		}
	}
	if (turning()) {
		if (doneTurning) {
			move();
		}
	}
}

//determine if robot is currently moving
boolean moving() {
	if (state == stateMoving)
		return true;
	else
		return false;
}

//determine if robot is currently turning
boolean turning() {
	if (state == stateTurning)
		return true;
	else
		return false;
}

//determine if an obstacle is to close
//to the robot
boolean obstacleAhead() {
	if (distance <= TO_CLOSE || distance >= SIGNAL_LOST)
		return true;
	else
		return false;
}

//determine if robot is done turning
boolean doneTurning() {
	if (distance > (TO_CLOSE + 8) && distance < SIGNAL_LOST)
		return true;
	else
		return false;
}

//drive the robot forward
void move() {
	state = stateMoving;
	driveForward();
}

//avoid an obstacle by backing up and turning
void avoidObject() {
	state = stateTurning;
	whichWay = switchTurn(whichWay, numbOfTurns);
	if (whichWay == 0) {
		driveReverse();
		delay(REVERSE_TIME);
		turnLeft();
		delay(random(MIN_TURN_TIME, MAX_TURN_TIME));
	}
	else if (whichWay == 1) {
		driveReverse();
		delay(REVERSE_TIME);
		turnRight();
		delay(random(MIN_TURN_TIME, MAX_TURN_TIME));
	}
}

//called every time a turn is executed
//causes robot to turn in the opposite 
//direction every four turns
int switchTurn(int whichWay, int turns) {
	turns++;
	if (turns == 5) {
		switch (whichWay) {
		case 0:
			whichWay = 1;
			break;
		case 1:
			whichWay = 0;
			break;
		}
		turns = 0;
	}
	numbOfTurns = turns;
	return whichWay;  
}

//drive the robot forward
void driveForward() {
	rightMotor.run(FORWARD);
	leftMotor.run(FORWARD);
}

//drive the robot backwards
void driveReverse() {
	stopMoving();
	delay(500);
	rightMotor.run(BACKWARD);
	leftMotor.run(BACKWARD);
}

//turn the robot right
void turnRight() {
	rightMotor.run(FORWARD);
	leftMotor.run(BACKWARD);
}

//turn the robot left
void turnLeft() {
	rightMotor.run(BACKWARD);
	leftMotor.run(FORWARD);
}

//cut power to motors,
//if robot is weighed down it will stop
void stopMoving() {
	rightMotor.run(RELEASE);
	leftMotor.run(RELEASE);

}
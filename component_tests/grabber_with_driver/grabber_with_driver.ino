#include <I2Cdev.h>
#include <PCA9685.h>
#include <Servo.h>

ServoDriver servo;
const uint8_t leftPin = 2;                       // pin number on DRIVER, NOT on Arduino board
const uint8_t rightPin = 16;                      // pin number on DRIVER, NOT on Arduino board
const uint8_t armPin = 14;                       // pin number on DRIVER, NOT on Arduino board

const long grabSequenceInterval = 3500;
long previousTime = 0;
long previousSubTime = 0;

uint8_t upperBoundArm = 140;
uint8_t lowerBoundArm = 0;
uint8_t grabBoundHand = 170;
uint8_t receiveBoundHand = 80;
uint8_t ungrabBoundHand = 160;

uint8_t currentHandPos = 0;
uint8_t currentArmPos = 0;

enum GS {
  IDLER,
  GRAB,
  RAISE,
  UNGRAB,
  LOWER
};

GS grabState = IDLER;

void setup() {
  Serial.begin(9600);
  Wire.begin();                               // join I2C bus (I2Cdev library doesn't do this automatically)
  servo.init(0x7f);                           // init driver address
  delay(1000);
  calibrateGrabber();
}

void loop() {
  updateGrabber();
}

void updateGrabber() {
  //Serial.println(grabState);
  switch (grabState) {
    case IDLER:
      if (digitalRead(3)) {
        grabState = GRAB;
        previousTime = millis();
      } else {
        moveArm(upperBoundArm);
        moveHands(receiveBoundHand, 5);
      }
      break;

    case GRAB:
      if (millis() - previousTime > 2000) {
        grabState = RAISE;
        previousTime = millis();
      } else {
        moveHands(grabBoundHand, 30);
      }
      break;

    case RAISE:
      if (millis() - previousTime > grabSequenceInterval) {
        grabState = UNGRAB;
        previousTime = millis();
      } else {
        moveArm(lowerBoundArm);
      }
      break;

    case UNGRAB:
      if (millis() - previousTime > 1000) {
        grabState = LOWER;
        previousTime = millis();
      } else {
        moveHands(ungrabBoundHand, 15);
      }
      break;

    case LOWER:
      if (millis() - previousTime > grabSequenceInterval) {
        grabState = IDLER;
        previousTime = millis();
      } else {
        moveArm(upperBoundArm);
      }
      break;
  }
}

void moveArm(uint8_t bound) {
  int interval = 40;
  if (millis() - previousSubTime > interval) {
    if (currentArmPos < bound) {
      currentArmPos += 2;
      servo.setAngle(armPin, currentArmPos);
    } else if (currentArmPos > bound) {
      currentArmPos -= 2;
      servo.setAngle(armPin, currentArmPos);
    } else {
      currentArmPos = bound;
    }
    previousSubTime = millis();
  }
}

void moveHands(uint8_t bound, uint8_t interval) {
  if (millis() - previousSubTime > interval) {
    if (currentHandPos < bound) {
      currentHandPos += 2;
      servo.setAngle(leftPin, currentHandPos);
      servo.setAngle(rightPin, 180 - currentHandPos);
    } else if (currentHandPos > bound) {
      currentHandPos -= 2;
      servo.setAngle(leftPin, currentHandPos);
      servo.setAngle(rightPin, 180 - currentHandPos);
    } else {
      currentHandPos = bound;
    }
    previousSubTime = millis();
  }
}

void calibrateGrabber() {
  Serial.print("calibrating... ");
  servo.setAngle(armPin, upperBoundArm);
  delay(1000);
  servo.setAngle(leftPin, grabBoundHand);
  servo.setAngle(rightPin, 180 - grabBoundHand);
  delay(1000);
  servo.setAngle(armPin, lowerBoundArm);
  delay(1000);
  servo.setAngle(armPin, upperBoundArm);
  delay(1000);
  currentArmPos = upperBoundArm;
  currentHandPos = grabBoundHand;
  Serial.println("done");
}

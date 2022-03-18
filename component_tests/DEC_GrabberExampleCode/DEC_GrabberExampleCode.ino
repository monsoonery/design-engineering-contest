#include <Servo.h>

Servo grabber;  // create servo object to control a servo
Servo grabber2;
Servo arm;

const uint8_t leftArmPin = 5;
const uint8_t rightArmPin = 2;
const uint8_t shoulderPin = 6;

const long grabSequenceInterval = 500;
long previousTime = 0;
uint8_t grabState = 0;

// twelve servo objects can be created on most boards

uint8_t upperBoundShoulder = 140;
uint8_t lowerBoundShoulder = 0;
uint8_t upperBoundHand = 170;
uint8_t lowerBoundHand = 80;
uint8_t ungrabBoundHand = 160;

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.print("calibrating... ");
  arm.attach(shoulderPin);
  arm.write(upperBoundShoulder);
  delay(1000);
  grabber.attach(leftArmPin);
  grabber.write(upperBoundHand);
  grabber2.attach(rightArmPin);
  grabber2.write(180 - upperBoundHand);
  delay(1000);
  arm.write(lowerBoundShoulder);
  delay(1000);
  lowerArm();
  Serial.println("done");
}

void loop() {
  updateGrabber();
}

void updateGrabber() {
  switch (grabState) {
    case 0:
      if (digitalRead(3)) {
        grabState = 1;
        previousTime = millis();
      } else {
        receive();
      }
      break;

    case 1:
      grabState = 2;
      break;

    case 2:
      if (millis() - previousTime > grabSequenceInterval) {
        grab();
        grabState = 3;
        previousTime = millis();
      }
      break;

    case 3:
      if (millis() - previousTime > grabSequenceInterval) {
        raiseArm();
        grabState = 4;
        previousTime = millis();
      }
      break;

    case 4:
      if (millis() - previousTime > grabSequenceInterval) {
        unGrab();
        grabState = 5;
        previousTime = millis();
      }
      break;

    case 5:
      if (millis() - previousTime > grabSequenceInterval) {
        lowerArm();
        grabState = 0;
        previousTime = millis();
      }
      break;
  }
}

void raiseArm() {
  int currentPos = arm.read();
  while (currentPos != lowerBoundShoulder) {
    if (currentPos < lowerBoundShoulder) {
      arm.write(currentPos + 2);
    } else {
      arm.write(currentPos - 2);
    }
    currentPos = arm.read();
    delay(40);
  }
}

void lowerArm() {
  int currentPos = arm.read();
  while (currentPos != upperBoundShoulder) {
    if (currentPos < upperBoundShoulder) {
      arm.write(currentPos + 2);
    } else {
      arm.write(currentPos - 2);
    }
    currentPos = arm.read();
    delay(40);
  }
}

void grab() {
  int currentPos = grabber.read();
  while (currentPos != upperBoundHand) {
    if (currentPos < upperBoundHand) {
      grabber.write(currentPos + 2);
      grabber2.write(180 - (currentPos + 2));
    } else {
      grabber.write(currentPos - 2);
      grabber2.write(180 - (currentPos - 2));
    }
    currentPos = grabber.read();
    delay(30);
  }
}

void receive() {
  int currentPos = grabber.read();
  while (currentPos != lowerBoundHand) {
    if (currentPos < lowerBoundHand) {
      grabber.write(currentPos + 2);
      grabber2.write(180 - (currentPos + 2));
    } else {
      grabber.write(currentPos - 2);
      grabber2.write(180 - (currentPos - 2));
    }
    currentPos = grabber.read();
    delay(5);
  }
}

void unGrab() {
  int currentPos = grabber.read();
  while (currentPos != ungrabBoundHand) {
    if (currentPos < ungrabBoundHand) {
      grabber.write(currentPos + 2);
      grabber2.write(180 - (currentPos + 2));
    } else {
      grabber.write(currentPos - 2);
      grabber2.write(180 - (currentPos - 2));
    }
    currentPos = grabber.read();
    delay(15);
  }
}

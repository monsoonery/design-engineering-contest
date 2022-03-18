#include <Servo.h>

Servo grabber;  // create servo object to control a servo
Servo grabber2;
Servo arm;

const uint8_t leftArmPin = 5;
const uint8_t rightArmPin = 2;
const uint8_t shoulderPin = 6;

const long grabSequenceInterval = 3500;
long previousTime = 0;
long previousSubTime = 0;

// twelve servo objects can be created on most boards

uint8_t upperBoundShoulder = 140;
uint8_t lowerBoundShoulder = 0;
uint8_t upperBoundHand = 170;
uint8_t lowerBoundHand = 80;
uint8_t ungrabBoundHand = 160;

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
        lowerArm();
        receive();
      }
      break;

    case GRAB:
      if (millis() - previousTime > 2000) {
        grabState = RAISE;
        previousTime = millis();
      } else {
        grab();
      }
      break;

    case RAISE:
      if (millis() - previousTime > grabSequenceInterval) {
        grabState = UNGRAB;
        previousTime = millis();
      } else {
        raiseArm();
      }
      break;

    case UNGRAB:
      if (millis() - previousTime > 1000) {
        grabState = LOWER;
        previousTime = millis();
      } else {
        unGrab();
      }
      break;

    case LOWER:
      if (millis() - previousTime > grabSequenceInterval) {
        grabState = IDLER;
        previousTime = millis();
      } else {
        lowerArm();
      }
      break;
  }
}

void defend() {
  grab();
  raiseArm();
}

void raiseArm() {
  int interval = 40;
  int currentPos = arm.read();
  if (millis() - previousSubTime > interval) {
    if (currentPos < lowerBoundShoulder) {
      arm.write(currentPos + 2);
    } else if (currentPos > lowerBoundShoulder) {
      arm.write(currentPos - 2);
    }
    previousSubTime = millis();
    currentPos = arm.read();
  }
}

void lowerArm() {
  int interval = 40;
  int currentPos = arm.read();
  if (millis() - previousSubTime > interval) {
    if (currentPos < upperBoundShoulder) {
      arm.write(currentPos + 2);
    } else if (currentPos > upperBoundShoulder) {
      arm.write(currentPos - 2);
    }
    previousSubTime = millis();
    currentPos = arm.read();
  }
}

void grab() {
  int interval = 30;
  int currentPos = grabber.read();
  if (millis() - previousSubTime > interval) {
    if (currentPos < upperBoundHand) {
      grabber.write(currentPos + 2);
      grabber2.write(180 - (currentPos + 2));
    } else if (currentPos > upperBoundHand) {
      grabber.write(currentPos - 2);
      grabber2.write(180 - (currentPos - 2));
    }
    previousSubTime = millis();
    currentPos = grabber.read();
  }
}

void receive() {
  int interval = 5;
  int currentPos = grabber.read();
  if (millis() - previousSubTime > interval) {
    if (currentPos < lowerBoundHand) {
      grabber.write(currentPos + 2);
      grabber2.write(180 - (currentPos + 2));
    } else if (currentPos > lowerBoundHand) {
      grabber.write(currentPos - 2);
      grabber2.write(180 - (currentPos - 2));
    }
    previousSubTime = millis();
    currentPos = grabber.read();
  }
}

void unGrab() {
  int interval = 15;
  int currentPos = grabber.read();
  if (millis() - previousSubTime > interval) {
    if (currentPos < ungrabBoundHand) {
      grabber.write(currentPos + 2);
      grabber2.write(180 - (currentPos + 2));
    } else if (currentPos > ungrabBoundHand) {
      grabber.write(currentPos - 2);
      grabber2.write(180 - (currentPos - 2));
    }
    previousSubTime = millis();
    currentPos = grabber.read();
  }
}

void calibrateGrabber() {
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
  arm.write(upperBoundShoulder);
  delay(1000);
  Serial.println("done");
}

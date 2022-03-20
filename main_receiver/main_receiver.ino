/* LIBRARIES */
#include <SPI.h>
#include <I2Cdev.h>
#include <PCA9685.h>
#include <Wire.h>
#include <RF24.h>
#include "pitches.h"

/* RADIO */
RF24 radio(9, 10);                             // create an RF24 object, pin 9 is CE, pin 10 is CSN
const byte pipeName[6] = "99999";              // our custom pipe address
byte payload[32] = {0};                        // array of bytes that will be sent over RF, size 32 bytes

/* SERVO DRIVER PINS */
ServoDriver servo;
const uint8_t leftPin = 2,                    // pin numbers on DRIVER, NOT on Arduino board
              rightPin = 1,
              armPin = 16,
              beltPin = 6,
              trapdoorPin = 7,
              kokerdoorPin = 8;


/* OTHER PINS */
const uint8_t motorR_power_pin = 5,
              motorR_direction_pin = 7,
              motorL_power_pin = 6,
              motorL_direction_pin = 8,
              buzzerPin = 4,
              indicatorLedPin = 6;             // temp debug LED for verifying radio transmissions are coming through

/* CONSTANTS */
const uint8_t constrainValue = 20,             // absolute minimum value for the wheels to have any movement
              upperBoundArm = 142,             // angle at which the arm is at the floor
              lowerBoundArm = 0,               // angle at which the arm is vertical
              grabBoundHand = 170,             // angle at which the hands can grab a block
              receiveBoundHand = 80,           // angle at which the hands can receive a block
              ungrabBoundHand = 160;           // angle at which the hands let go of a block

enum GS {
  IDLER,
  GRAB,
  RAISE,
  UNGRAB,
  LOWER,
  DEFEND
};                                             // variable for grabber state machine
GS grabState = IDLER;                          // using enum to make states clearer

/* VARIABLES */
uint8_t output_power_R = 0,                    // output speed right wheel
        output_power_L = 0,                    // output speed left wheel
        output_direction_R = 0,                // output direction right wheel
        output_direction_L = 0,                // output direction left wheel
        beltSpeed = 0,                         // desired belt speed (aka big continuous servo speed)
        sound = 0;                             // desired sound/melody for buzzer
bool grabButtonPress,                          // these speak for themselves
     redButtonPress,                           // ""
     blueButtonPress,                          // ""
     defendGrabber;

long previousTime = 0,                         // for grabbing sequence state machine
     previousSubTime = 0;                      // for subfunctions (grab() ungrab() etc) in the state machine
uint8_t currentHandPos = 0,                    // current position of the small hand servos (driver has no read function, so need to be tracked manually)
        currentArmPos = 0;                     // current position of the large arm servo
bool alreadyDone = false;                      // avoids weird button shit when trying to get out of defend mode
bool trapdoorOpen = false;                     // a flag to track the trapdoor status (needed for large doors "safety lock")
unsigned long lastTransmissionTime;            // tracks transmission times (needed to alert us when radio disconnects)


void setup() {
  setupWithoutSound();
}

void loop() {
  checkRadio();                              // check for new incoming transmissions
  checkDefendGrabber();
  updateWheels();                            // actuate wheels according to joystick input
  updateBelt();                              // update the belt speed according to potentiometer input
  updateGrabber();                           // activate the grabbing sequence if button was pressed
  checkSounds();                             // choose the desired melody/sound if applicable
  updateMelody();                            // update the melody/sound (play the next note) if one was selected
  updateDoors();                             // open or close the trapdoor or tube doors
}

void setupWithSound() {
  // start serial
  Serial.begin(9600);
  Serial.println(F("Initializing... "));
  tone(buzzerPin, NOTE_C3, 2000);
  delay(2000);

  // prepare servo driver
  Serial.print(F("Servo driver... "));
  Wire.begin();
  servo.init(0x7f);
  Serial.println(F("done"));
  tone(buzzerPin, NOTE_G3, 2000);
  delay(2000);

  //prepare radio
  Serial.print(F("Setting up radio... "));
  if (!radio.begin()) while (1);
  radio.openReadingPipe(0, pipeName);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  Serial.println(F("done"));
  tone(buzzerPin, NOTE_C4, 2000);
  delay(2000);

  // prepare motor driver
  Serial.print(F("Motor driver... "));
  pinMode(motorR_power_pin, OUTPUT);
  pinMode(motorR_direction_pin, OUTPUT);
  pinMode(motorL_power_pin, OUTPUT);
  pinMode(motorL_direction_pin, OUTPUT);
  Serial.println(F("done"));
  delay(375);

  // prepare and calibrate grabber
  tone(buzzerPin, NOTE_E4, 375);
  delay(375);
  tone(buzzerPin, NOTE_DS4, 2500);
  Serial.print(F("Calibrating grabber... "));
  calibrateGrabber();
  Serial.println(F("done"));

  Serial.println(F("Initialization complete!"));
}

void setupWithoutSound() {
  // start serial
  Serial.begin(9600);
  Serial.println(F("Initializing... "));

  // prepare servo driver
  Serial.print(F("Servo driver... "));
  Wire.begin();
  servo.init(0x7f);
  Serial.println(F("done"));

  //prepare radio
  Serial.print(F("Setting up radio... "));
  if (!radio.begin()) while (1);
  radio.openReadingPipe(0, pipeName);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  Serial.println(F("done"));

  // prepare motor driver
  Serial.print(F("Motor driver... "));
  pinMode(motorR_power_pin, OUTPUT);
  pinMode(motorR_direction_pin, OUTPUT);
  pinMode(motorL_power_pin, OUTPUT);
  pinMode(motorL_direction_pin, OUTPUT);
  Serial.println(F("done"));

  // prepare and calibrate grabber
  Serial.print(F("Calibrating grabber... "));
  calibrateGrabber();
  Serial.println(F("done"));

  Serial.println(F("Initialization complete!"));
}

void checkRadio() {
  if (radio.available()) {
    //get data
    radio.read(&payload, sizeof(payload));

    // print data for debugging
    //debugRadio();

    // sort data in appropriate variables
    output_power_L = payload[0];
    output_power_R = payload[1];
    output_direction_L = payload[2];
    output_direction_R = payload[3];
    sound = payload[4];
    grabButtonPress = payload[5];
    redButtonPress = payload[6];
    blueButtonPress = payload[7];
    beltSpeed = payload[8];
    defendGrabber = payload[9];

    // register last transmission time
    lastTransmissionTime = millis();
  } else {
    if (millis() - lastTransmissionTime > 3000) {
      //tone(buzzerPin, NOTE_FS4);
      // stop the wheels to avoid collision until radio is back
      output_power_L = payload[0];
      output_power_R = payload[1];
      output_direction_L = payload[2];
      output_direction_R = payload[3];
    }
  }
}

void updateWheels() {
  //power the motor pins
  analogWrite(    motorR_power_pin,     output_power_R);
  digitalWrite(   motorR_direction_pin, output_direction_R);
  analogWrite(    motorL_power_pin,     output_power_L);
  digitalWrite(   motorL_direction_pin, output_direction_L);
}

void updateBelt() {
  // map potentiometer value to "angles" for continuous servo
  int v = map(beltSpeed, 0, 255, 90, 270);
  servo.setAngle(beltPin, v);
}

void updateGrabber() {
  // grabbing sequence requires multiple functions to be ran in succession
  // without interrupting other functions like driving or radio transmission
  // therefore we use a switch-case statement to simulate a state machine
  //Serial.println(grabState);
  int grabSequenceInterval;
  int interval;
  switch (grabState) {
    case IDLER:
      if (grabButtonPress) {
        // go ahead and grab the block
        grabState = GRAB;
        previousTime = millis();
      } else {
        // keep the grabber ready to receive a block
        if (moveArm(upperBoundArm, 10)) {
          alreadyDone = false;
          moveHands(receiveBoundHand, 5);
        }
      }
      break;

    case GRAB:
      interval = 30;
      grabSequenceInterval = interval * (abs(receiveBoundHand - grabBoundHand));
      //Serial.println(grabSequenceInterval);
      if (moveHands(grabBoundHand, interval) && millis() - previousTime > grabSequenceInterval) {
        // go ahead and raise the arm
        grabState = RAISE;
        previousTime = millis();
      }
      break;

    case RAISE:
      interval = 30;
      grabSequenceInterval = interval * (abs(upperBoundArm - lowerBoundArm)) - 1000;
      //Serial.println(grabSequenceInterval);
      if (moveArm(lowerBoundArm, interval) && millis() - previousTime > grabSequenceInterval) {
        // go ahead and let go of the block
        grabState = UNGRAB;
        previousTime = millis();
      }
      break;

    case UNGRAB:
      interval = 10;
      grabSequenceInterval = interval * (abs(ungrabBoundHand - grabBoundHand)) + 400;
      //Serial.println(grabSequenceInterval);
      if (moveHands(ungrabBoundHand, interval) && millis() - previousTime > grabSequenceInterval) {
        // go ahead and lower the arm
        grabState = LOWER;
        previousTime = millis();
      }
      break;

    case LOWER:
      interval = 10;
      grabSequenceInterval = interval * (abs(upperBoundArm - lowerBoundArm));
      //Serial.println(grabSequenceInterval);
      if (moveArm(upperBoundArm, interval) && millis() - previousTime > grabSequenceInterval) {
        // go back to idle state
        grabState = IDLER;
        previousTime = millis();
      }
      break;

    case DEFEND:
      interval = 5;
      if (moveHands(grabBoundHand, interval)) {
        alreadyDone = false;
        moveArm(lowerBoundArm, interval);
      }
      break;

  }
}

void checkSounds() {
  switch (sound) {
    case 0:
      //do nothing
      break;

    case 1:
      stopMelody();
      startAmogusMelody(1200, false);
      break;

    case 2:
      stopMelody();
      startCurbMelody(1000, false);
      break;

    case 3:
      stopMelody();
      startSansMelody(1000, false);
      break;

    case 4:
      startBackwardsMelody();
      break;

    case 5:
      //startSpaceMelody();
      break;

    case 6:
      startDenyMelody(1000, false);
      break;

    case 7:
      startAcceptMelody(1000, false);
      break;

    case 8:
      stopMelody();
      break;
  }
}

void updateDoors() {
  // the large doors can only be open when the trapdoor is open
  if (blueButtonPress && trapdoorOpen) {
    // todo open large doors
    // servo.setAngle();
  } else {
    // todo close large doors
    // servo.setAngle();
  }
  if (redButtonPress) {
    // check if we need to open or close the trapdoor
    if (!trapdoorOpen) {
      // todo open trapdoor
      // servo.setAngle();
      trapdoorOpen = true;
    } else {
      // todo close trapdoor
      // servo.setAngle();
      trapdoorOpen = false;
    }
  }
}

/********************************/
void calibrateGrabber() {
  Serial.print(F("("));

  servo.setAngle(armPin, upperBoundArm);
  Serial.print(F("down, "));

  delay(500);
  servo.setAngle(leftPin, grabBoundHand);
  servo.setAngle(rightPin, 180 - grabBoundHand);
  Serial.print(F("grab, "));

  delay(500);
  servo.setAngle(armPin, lowerBoundArm);
  Serial.print(F("up, "));

  delay(500);
  servo.setAngle(armPin, upperBoundArm);
  Serial.print(F("down, "));

  delay(500);
  servo.setAngle(leftPin, receiveBoundHand);
  servo.setAngle(rightPin, 180 - receiveBoundHand);
  Serial.print(F("receive) "));

  currentArmPos = upperBoundArm;
  currentHandPos = grabBoundHand;
}

bool moveArm(uint8_t bound, uint8_t interval) {
  if (millis() - previousSubTime > interval) {
    if (currentArmPos < bound) {
      currentArmPos += 2;
      servo.setAngle(armPin, currentArmPos);
    } else if (currentArmPos > bound) {
      currentArmPos -= 2;
      servo.setAngle(armPin, currentArmPos);
    } else {
      currentArmPos = bound;
      return true;
    }
    previousSubTime = millis();
    return false;
  }
  return false;
}

bool moveHands(uint8_t bound, uint8_t interval) {
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
      return true;
    }
    previousSubTime = millis();
    return false;
  }
  return false;
}

void checkDefendGrabber() {
  if (defendGrabber && !alreadyDone) {
    grabState = grabState != DEFEND ? DEFEND : IDLER;
    alreadyDone = true;
  }
}

void debugRadio() {
  //if (random(0, 6) == 2) digitalWrite(indicatorLedPin, !digitalRead(indicatorLedPin));
  Serial.print(F("received: "));
  for (int i = 0; i < 32; i++) {
    Serial.print(payload[i]); Serial.print(F(" "));
  }
  Serial.println();
}

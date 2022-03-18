/* LIBRARIES */
#include <SPI.h>
#include <I2Cdev.h>
#include <PCA9685.h>
#include <Wire.h>
#include <RF24.h>

/* RADIO */
RF24 radio(9, 10);                              // create an RF24 object, pin 9 is CE, pin 10 is CSN
const byte pipeName[6] = "99999";               // our custom pipe address
byte payload[32] = {0};                         // array of bytes that will be sent over RF, size 32 bytes

/* SERVO DRIVER PINS */
ServoDriver servo;
const uint8_t leftPin = 16;                     // pin number on DRIVER, NOT on Arduino board
const uint8_t rightPin = 2;                    // pin number on DRIVER, NOT on Arduino board
const uint8_t armPin = 14;                      // pin number on DRIVER, NOT on Arduino board
const uint8_t beltPin = 4;                     // pin number corresponds to driver not to seeeduino board

/* OTHER PINS */
const uint8_t motorR_power_pin = 6;
const uint8_t motorR_direction_pin = 7;
const uint8_t motorL_power_pin = 5;
const uint8_t motorL_direction_pin = 8;
const uint8_t buzzerPin = 4;
const uint8_t indicatorLedPin = 6;


/* CONSTANTS */
const uint8_t constrainValue = 20;
uint8_t upperBoundArm = 140;
uint8_t lowerBoundArm = 0;
uint8_t grabBoundHand = 170;
uint8_t receiveBoundHand = 80;
uint8_t ungrabBoundHand = 160;
const long grabSequenceInterval = 3500;

enum GS {
  IDLER,
  GRAB,
  RAISE,
  UNGRAB,
  LOWER
};
GS grabState = IDLER;

/* VARIABLES */
int output_power_R = 0,
    output_power_L = 0,
    output_direction_R = 0,
    output_direction_L = 0,
    beltValue = 0,
    sound = 0;
bool grabButtonPress,
     redButtonPress,
     blueButtonPress;
long previousTime = 0;
long previousSubTime = 0;
uint8_t currentHandPos = 0;
uint8_t currentArmPos = 0;


void setup() {
  Serial.begin(9600);
  Serial.print(F("welcome"));

  //prepapre motor pins
  pinMode(motorR_power_pin, OUTPUT);
  pinMode(motorR_direction_pin, OUTPUT);
  pinMode(motorL_power_pin, OUTPUT);
  pinMode(motorL_direction_pin, OUTPUT);

  Serial.print(F("welcome2"));

  //prepare servo driver
  Wire.begin();
  servo.init(0x7f);

  Serial.print(F("welcome3"));

  //prepare grabber
  calibrateGrabber();

  //prepare radio
  radio.begin();                             // setup myRadio module
  radio.openReadingPipe(0, pipeName);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  Serial.println(F("done"));
  delay(50);
}

void loop() {
  checkRadio();                              // check for new incoming transmissions
  updateWheels();                            // actuate wheels according to transmission data
  updateBelt();
  updateGrabber();
  checkSounds();
  updateMelody();
  //Serial.println("pass");
}

void checkRadio() {
  if (radio.available()) {
    //get data
    radio.read(&payload, sizeof(payload));
    digitalWrite(indicatorLedPin, !digitalRead(indicatorLedPin));
    //print it for debugging
     Serial.print(F("receiving data: "));
     for (int i = 0; i < 32; i++) {
        Serial.print(payload[i]); Serial.print(F(" "));
     }
     Serial.println();
    //sort in appropriate variables
    output_power_L = payload[0];
    output_power_R = payload[1];
    output_direction_L = payload[2];
    output_direction_R = payload[3];
    sound = payload[4];
    grabButtonPress = payload[5];
    redButtonPress = payload[6];
    blueButtonPress = payload[7];
    beltValue = payload[8];
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
  int v = map(beltValue, 0, 255, 90, 270);
  servo.setAngle(beltPin, v);
  //Serial.println(v);
}

void updateGrabber() {
  //Serial.println(grabState);
  switch (grabState) {
    case IDLER:
      if (grabButtonPress) {
        grabButtonPress = false;
        grabState = GRAB;
        previousTime = millis();
      } else {
        moveArm(upperBoundArm, 10);
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
        moveArm(lowerBoundArm, 30);
      }
      break;

    case UNGRAB:
      if (millis() - previousTime > 1000) {
        grabState = LOWER;
        previousTime = millis();
      } else {
        moveHands(ungrabBoundHand, 10);
      }
      break;

    case LOWER:
      if (millis() - previousTime > grabSequenceInterval) {
        grabState = IDLER;
        previousTime = millis();
      } else {
        moveArm(upperBoundArm, 10);
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
      stopMelody();
      break;
  }
}


/********************************/
void calibrateGrabber() {
  Serial.print(F("calibrating... "));
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
  Serial.println(F("done"));
}

void moveArm(uint8_t bound, uint8_t interval) {
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

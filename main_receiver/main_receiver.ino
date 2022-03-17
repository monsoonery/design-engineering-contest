/* LIBRARIES */
#include <SPI.h>
#include "PCA9685.h"
#include <Wire.h>
#include <RF24.h>

/* RADIO */
RF24 radio(9, 10);                              // create an RF24 object, pin 9 is CE, pin 10 is CSN
const byte pipeName[6] = "99999";               // our custom pipe address
byte payload[32] = {0};                         // array of bytes that will be sent over RF, size 32 bytes

/* SERVO DRIVER */
ServoDriver servo;

/* PINS */
const uint8_t motorR_power_pin = 6;
const uint8_t motorR_direction_pin = 7;
const uint8_t motorL_power_pin = 5;
const uint8_t motorL_direction_pin = 8;
const uint8_t buzzerPin = 4;
const uint8_t beltPin = 1;                     // pin number corresponds to driver not to seeeduino board

/* CONSTANTS */
const uint8_t constrainValue = 20;

/* VARIABLES */
int output_power_R = 0,
    output_power_L = 0,
    output_direction_R = 0,
    output_direction_L = 0,
    beltValue = 0,
    sound = 0;


void setup() {
  Serial.begin(9600);

  //prepapre motor pins
  pinMode(motorR_power_pin, OUTPUT);
  pinMode(motorR_direction_pin, OUTPUT);
  pinMode(motorL_power_pin, OUTPUT);
  pinMode(motorL_direction_pin, OUTPUT);

  //prepare servo driver
  Wire.begin();
  servo.init(0x7f);
  servo.setServoPulseRange(500, 2500, 360);

  //prepare radio
  radio.begin();                             // setup myRadio module
  radio.openReadingPipe(0, pipeName);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  Serial.println("done");
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
    //print it for debugging
    /*Serial.print("receiving data: ");
      for (int i = 0; i < 32; i++) {
      Serial.print(payload[i]); Serial.print(F(" "));
      }
      Serial.println();*/
    //sort in appropriate variables
    output_power_L = payload[0];
    output_power_R = payload[1];
    output_direction_L = payload[2];
    output_direction_R = payload[3];
    sound = payload[4];
    beltValue = payload[7];
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
  //todo add coens code
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

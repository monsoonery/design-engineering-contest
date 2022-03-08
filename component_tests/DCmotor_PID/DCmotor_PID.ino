#include <SPI.h>


//CircularBuffer <float, 10> circularBufferL;
//CircularBuffer <float, 10> circularBufferR;
int bufferSize = 10;

const int forwardPinL = 5;
const int backwardPinL = 6;
const int forwardPinR = 8;
const int backwardPinR = 9;

const int sensorPinL = 2;
const int sensorPinR = 3;

const int diameterWiel = 0.213; //in meters
const int holes = 12; //aantal gaten in encoder

unsigned long timeL1 = 0,
              timeL2 = 0,
              timeR1 = 0,
              timeR2 = 0;
float timeIntervalL = 0,
      timeIntervalR = 0;

const int joystickPin_Y1 = A0;

void setup() {
  Serial.begin(9600);

  //set up interrupts for infrared sensors
  attachInterrupt(digitalPinToInterrupt(sensorPinL), interruptL, RISING);
  //attachInterrupt(digitalPinToInterrupt(sensorPinR), interruptR, RISING);
}

void loop() {
  int desiredSpeedForward = 0;
  int desiredSpeedBackward = 0;


  int joystickInput = analogRead(joystickPin_Y1);
  //Serial.println(joystickInput);
  if (joystickInput < 522) {
    desiredSpeedBackward = map(joystickInput, 522, 0, 0, 1200);
    //Serial.println("pass backward");
  } else {
    desiredSpeedForward = map(joystickInput, 522, 1024, 0, 1200);
    //Serial.println("pass forward");
  }
  
  timeIntervalL = (timeL2 - timeL1) / 1000000.0;
  float currentSpeed = 360 / 12 / timeIntervalL;

//  Serial.print("desired speed forwards: ");
//  Serial.print(desiredSpeedBackward);
//  Serial.print(",");
//  Serial.print("desired speed backwards: ");
//  Serial.print(desiredSpeedBackward);
//  Serial.print(",");
//  Serial.print("current speed: ");
//  Serial.println(currentSpeed);

  if (desiredSpeedForward != 0) {                                                         //desired speed vooruit niet nul dus je wil vooruitrijden
    int requiredPower = map(desiredSpeedForward, 0, 1200, 0, 255);
    //int properPower = constrain(desiredSpeed, 0, 255);
    analogWrite(forwardPinL, requiredPower);
    analogWrite(backwardPinL, 0);
    //Serial.println("vooruit");
  } else if (desiredSpeedBackward != 0) {                                                 //desired speed achteruit niet nul dus je wil achteruitrijden
    int requiredPower = map(desiredSpeedBackward, 0, 1200, 0, 255);
    //int properPower = constrain(desiredSpeed, 0, 255);
    analogWrite(forwardPinL, 0);
    analogWrite(backwardPinL, requiredPower);
    //Serial.println("achteruit");
  } else {
    analogWrite(forwardPinL, 0);
    analogWrite(backwardPinL, 0);
    //Serial.println("neutral");
  }
  //int requiredPower = map(desiredSpeed, 0, 1200, 0, 255);
  //int properPower = constrain(desiredSpeed, 0, 255);
  //analogWrite(forwardPinL, requiredPower);
  //analogWrite(6, 0);
  //analogWrite(8, 255);
  //analogWrite(9, 0);
  //Serial.print(F("left: ")); Serial.println(analogRead(sensorPinL));
  //Serial.print(F("right: ")); Serial.println(analogRead(sensorPinR));
}

void interruptL() {
  timeL1 = timeL2;
  timeL2 = micros();
}

//void interruptR() {
//  timeR1 = timeR2;
//  timeR2 = micros();
//}

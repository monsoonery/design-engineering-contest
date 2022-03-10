#include <SPI.h>
#include <CircularBuffer.h>

/* BUFFER DEFINITIONS */
const int bufferSize = 10;
CircularBuffer <float, bufferSize> circularBufferL;
CircularBuffer <float, bufferSize> circularBufferR;

/* PIN DEFINITIONS */
const uint8_t forwardPinL = 9;
const uint8_t backwardPinL = 8;
const uint8_t forwardPinR = 5;
const uint8_t backwardPinR = 6;

const uint8_t sensorPinL = 2;
const uint8_t sensorPinR = 3;

const int joystickPin_Y1 = A0;

/* CONSTANTS */
const float diameterWiel = 0.213; //in meters
const uint8_t nHoles = 12;          //aantal gaten in encoder
const int lowerBoundJS = 520;   //safe low boundary of joystick
const int upperBoundJS = 526;   //safe upper boundary of joystick
const uint8_t minimumPower = 70;    //minimum PWM value required to force the wheels to move. used in constrain() function

/* VARIABLES */
unsigned long timeL1 = 0,
              timeL2 = 0,
              timeR1 = 0,
              timeR2 = 0;
float timeIntervalL = 0,
      timeIntervalR = 0;

unsigned long timeInt1 = 0,
              timeInt2 = 0;

float errorL = 0,
      errorPreviousL = 0,
      integralL = 0,
      derivativeL = 0,
      LKp = 0.5,
      LKi = 0,
      LKd = 0;
float total = 0;


void setup() {
  Serial.begin(9600);

  //set up interrupts for infrared sensors
  attachInterrupt(digitalPinToInterrupt(sensorPinL), interruptL, RISING);
  //attachInterrupt(digitalPinToInterrupt(sensorPinR), interruptR, RISING);

  //fill buffers with zeroes
  for (int i = 0; i < bufferSize; i++) {
    circularBufferL.push(0);
    circularBufferR.push(0);
  }
}

void loop() {
  // set desired speed direction variables to zero
  int desiredSpeedForward = 0;
  int desiredSpeedBackward = 0;

  // read position of analog stick
  int joystickInput = analogRead(joystickPin_Y1);
  //Serial.print("joystickInput: "); Serial.println(joystickInput);

  // map analog input from joystick to a desired speed value between 0 and 1200 degrees/second
  // only map the speed to the desired direction variable (forward or backward)
  // if joystick isnt being touched: do nothing because there's no desired speed or direction
  if (joystickInput < lowerBoundJS) {
    desiredSpeedBackward = map(joystickInput, lowerBoundJS, 0, 0, 1200);
    //Serial.println("pass backward");
  } else if (joystickInput > upperBoundJS) {
    desiredSpeedForward = map(joystickInput, upperBoundJS, 1024, 0, 1200);
    //Serial.println("pass forward");
  } else {
    //Serial.println("pass neutral");
  }

  // calculate actual speed using encoder
  timeIntervalL = (timeL2 - timeL1) / 1000000.0;
  float measuredSpeed = 360 / nHoles / timeIntervalL;

  //validity check
  if (measuredSpeed > 0 && measuredSpeed < 7000) {
    circularBufferL.push(measuredSpeed);
    float currentSpeed = 0;
    for (int i = 0; i < bufferSize; i++) {
      currentSpeed += circularBufferL[i];
    }
    currentSpeed = currentSpeed / bufferSize;
    Serial.println(currentSpeed);
    //float currentSpeed = measuredSpeed;

    timeInt1 = timeInt2;
    timeInt2 = micros();
    unsigned long timeIntervalInt = (timeInt2 - timeInt1) / 1000000.0;

    // Proportional
    errorPreviousL = errorL;
    if (desiredSpeedForward != 0) {
      errorL = desiredSpeedForward - currentSpeed;
    } else if (desiredSpeedBackward != 0) {
      errorL = desiredSpeedBackward - currentSpeed;
    } else {
      errorL = 0 - currentSpeed;
    }
    float PIDp = LKp * errorL;

    // Integral
    integralL += ((errorL + errorPreviousL) / 2.0) * (timeIntervalInt);
    float PIDi = LKi * integralL;

    // Derivative
    //derivativeL = (errorL - errorPreviousL) / (timeIntervalInt);
    //float PIDd = LKd * derivativeL;

    //return total PID-control value
    total = PIDp + PIDi;
    //+ PIDd;

//    Serial.print("Desired:");   Serial.print(desiredSpeedForward);  Serial.print(", ");
//    Serial.print("Measured:");  Serial.print(measuredSpeed);        Serial.print(", ");
//    Serial.print("Current:");   Serial.print(currentSpeed);         Serial.print(", ");
//    Serial.print("PID:");       Serial.println(total);                //Serial.print(", ");

    //    Serial.print("desiredSpeedForward: "); Serial.println(desiredSpeedForward);
    //    Serial.print("currentSpeed: "); Serial.println(currentSpeed);
    //    Serial.print("errorPreviousL: "); Serial.println(errorPreviousL);
    //    Serial.print("errorL: "); Serial.println(errorL);
    //    Serial.print("PIDp: "); Serial.println(PIDp);
    //    Serial.print("PIDi: "); Serial.println(PIDi);
    //    //Serial.print("PIDd: "); Serial.println(PIDd);
    //    Serial.print("Total: "); Serial.println(total);
  }

  //  Serial.print("desired speed forwards: ");
  //  Serial.print(desiredSpeedBackward);
  //  Serial.print(",");
  //  Serial.print("desired speed backwards: ");
  //  Serial.print(desiredSpeedBackward);
  //  Serial.print(",");
  //  Serial.print("current speed: ");
  //  Serial.println(currentSpeed);

  // apply appropriate power to forward or backward pin depending on desired speed and direction
  if (desiredSpeedForward != 0) {                                                         //desired speed vooruit niet nul dus je wil vooruitrijden
    //int requiredPower = map(desiredSpeedForward, 0, 1200, 80, 255);
    int requiredPower = total;
    int properPower = constrain(requiredPower, minimumPower, 255);
    analogWrite(forwardPinL, properPower);
    analogWrite(backwardPinL, 0);
    //Serial.print("properpower:"); Serial.println(properPower);
    //Serial.println(properPower);
    //Serial.println("vooruit");
  } else if (desiredSpeedBackward != 0) {                                                 //desired speed achteruit niet nul dus je wil achteruitrijden
    //int requiredPower = map(desiredSpeedBackward, 0, 1200, 80, 255);
    int requiredPower = total;
    int properPower = constrain(requiredPower, minimumPower, 255);
    analogWrite(forwardPinL, 0);
    analogWrite(backwardPinL, properPower);
    //Serial.println(properPower);
    //Serial.println("achteruit");
  } else {                                                                                //beide zijn nul, dus doe niks
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

  delay(2);
}

void interruptL() {
  timeL1 = timeL2;
  timeL2 = micros();
}

//void interruptR() {
//  timeR1 = timeR2;
//  timeR2 = micros();
//}

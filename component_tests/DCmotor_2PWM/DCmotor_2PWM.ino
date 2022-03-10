#include <SPI.h>
#include <CircularBuffer.h>

/* BUFFER DEFINITIONS */
const int bufferSize = 5;
CircularBuffer <float, bufferSize> circularBufferX;
CircularBuffer <float, bufferSize> circularBufferY;

/* PINS */
const uint8_t motorR_power_pin = 6;
const uint8_t motorR_direction_pin = 7;
const uint8_t motorL_power_pin = 5;
const uint8_t motorL_direction_pin = 8;
const uint8_t joystick_Y_pin = A0;
const uint8_t joystick_X_pin = A1;

/* CONSTANTS */
const uint8_t constrainValue = 20;

/* VARIABLES */
int speed_R = 0;
int speed_L = 0;
int joystick_input_Y = 0;
int joystick_input_X = 0;


void setup() {
  Serial.begin(9600);
  pinMode(motorR_power_pin, OUTPUT);
  pinMode(motorR_direction_pin, OUTPUT);
  pinMode(motorL_power_pin, OUTPUT);
  pinMode(motorL_direction_pin, OUTPUT);

  //clear the buffer
  for (int i = 0; i < bufferSize; i++) {
    circularBufferX.push(0);
    circularBufferY.push(0);
  }

  Serial.println("done");
  delay(50);
}

void loop() {
  //read joystick and map its value
  int Y = analogRead(joystick_Y_pin);
  int X = analogRead(joystick_X_pin);
  joystick_input_Y = map(Y, 0, 1023, -200, 200);
  joystick_input_X = map(X, 0, 1023, -55, 55);

  //take the average joystick input to prevent abrupt changes
  circularBufferY.push(joystick_input_Y);
  circularBufferX.push(joystick_input_X);
  float avg_joystick_input_Y = 0;
  float avg_joystick_input_X = 0;
  for (int i = 0; i < bufferSize; i++) {
    avg_joystick_input_Y += circularBufferY[i];
    avg_joystick_input_X += circularBufferX[i];
  }
  avg_joystick_input_Y = avg_joystick_input_Y / bufferSize;
  avg_joystick_input_X = avg_joystick_input_X / bufferSize;

  //bepaal gewenste snelheid per wiel
  speed_R = avg_joystick_input_Y + avg_joystick_input_X;
  speed_L = avg_joystick_input_Y - avg_joystick_input_X;

  //bepaal richting en map de benodigde output power adhv de gewenste snelheid
  int output_power_R     = 0,
      output_direction_R = 0,
      output_power_L     = 0,
      output_direction_L = 0;

  if (speed_R > 0) {
    output_power_R = speed_R;
    output_direction_R = LOW;
  } else {
    output_power_R = 255 + speed_R;
    output_direction_R = HIGH;
  }

  if (speed_L > 0) {
    output_power_L = speed_L;
    output_direction_L = LOW;
  } else {
    output_power_L = 255 + speed_L;
    output_direction_L = HIGH;
  }

  //power the motor pins
  analogWrite(    motorR_power_pin,     output_power_R);
  digitalWrite(   motorR_direction_pin, output_direction_R);
  analogWrite(    motorL_power_pin,     output_power_L);
  digitalWrite(   motorL_direction_pin, output_direction_L);

  //debug
  //  Serial.print("avg joystick input Y: ");       Serial.println(avg_joystick_input_Y);
  //  Serial.print("current joystick input Y: ");   Serial.println(joystick_input_Y);
  //  Serial.print("speed R: ");                    Serial.println(speed_R);
  //  Serial.print("speed L: ");                    Serial.println(speed_L);
  //  Serial.print("avg joystick input X: ");       Serial.println(avg_joystick_input_X);
  //  Serial.print("current joystick input X: ");   Serial.println(joystick_input_X);
  //  Serial.print("output forward R: ");           Serial.println(output_power_R);
  //  Serial.print("output backward R: ");          Serial.println(output_direction_R);
  //  Serial.print("output forward L: ");           Serial.println(output_power_L);
  //  Serial.print("output backward L: ");          Serial.println(output_direction_L);
  Serial.print("speed R: ");                    Serial.print(speed_R);
  Serial.print(" speed L: ");                    Serial.println(speed_L);
}

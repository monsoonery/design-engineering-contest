/* LIBRARIES */
#include <SPI.h>
#include <RF24.h>
#include <CircularBuffer.h>

/* RADIO */
RF24 radio(9, 10);                              // create an RF24 object, pin 9 is CE, pin 10 is CSN
const byte pipeName[6] = "99999";               // our custom pipe address
byte payload[32] = {0};                         // array of bytes that will be sent over RF, size 32 bytes

/* BUFFER DEFINITIONS */
const int bufferSize = 5;
CircularBuffer <float, bufferSize> circularBufferX;
CircularBuffer <float, bufferSize> circularBufferY;

/* PINS */
const int x1Pin = A1;                           // x-position of joystick 1
const int y1Pin = A0;                           // y-position of joystick 1
const int x2Pin = A3;                           // x-position of joystick 2
const int y2Pin = A2;                           // y-position of joystick 2
const int stickPin = 4;                         // joystick button

const int ledPin = 7;                           // led button - led
const int buttonPin = 8;                        // led button - button
const int potPin = A6;

/* MELODY LIST */
enum melodyType {
  PASS,
  AMOGUS,
  CURB,
  SANS,
  BACKWARDS,
  NOSOUND
};

/* VARIABLES */
const uint8_t constrainValue = 20;
int speed_R = 0;
int speed_L = 0;
int joystick_input_Y = 0;
int joystick_input_X = 0;
int joystick_input_Y2 = 0;
int joystick_input_X2 = 0;
uint8_t output_power_R     = 0,
        output_direction_R = 0,
        output_power_L     = 0,
        output_direction_L = 0,
        prev_bw = 0;

/* BUTTON DEBOUNCE */
boolean lastButtonPressed = false;

boolean buttonPress() {
  boolean buttonStatus = false;
  boolean buttonPressed = digitalRead(buttonPin);
  if (buttonPressed && !lastButtonPressed) {
    buttonStatus = true;
    digitalWrite(ledPin, !digitalRead(ledPin));
  }
  lastButtonPressed = buttonPressed;
  return buttonStatus;
}

/* STICK DEBOUNCE */
boolean lastStickPressed = false;

boolean stickPress() {
  boolean stickStatus = false;
  boolean stickPressed = digitalRead(stickPin);
  if (stickPressed && !lastStickPressed) {
    stickStatus = true;
  }
  lastStickPressed = stickPressed;
  return stickStatus;
}

void setup() {
  Serial.begin(9600);
  pinMode(stickPin, INPUT_PULLUP);            // use internal 10k pullup resistor for joystick button
  radio.begin();                                // setup myRadio module
  radio.openWritingPipe(pipeName);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();                        // this is a transmitter
  digitalWrite(ledPin, LOW);                    // init led pin as zero

  //clear the buffers
  for (int i = 0; i < bufferSize; i++) {
    circularBufferX.push(0);
    circularBufferY.push(0);
  }
}

void loop() {
  checkInputs();
  //checkInputs2();                                // check for new controller inputs
  sendRadio();                                  // send inputs over RF
  delay(10);                                    // wait before next loop
}

void checkInputs() {
  /* WHEELS */
  int X = analogRead(x1Pin);
  int Y = analogRead(y1Pin);
  joystick_input_X = map(X, 0, 1023, -80, 80);
  joystick_input_Y = map(Y, 0, 1023, -255, 255);

  //take the average joystick input to prevent abrupt changes
  circularBufferX.push(joystick_input_X);
  circularBufferY.push(joystick_input_Y);
  float avg_joystick_input_Y = 0;
  float avg_joystick_input_X = 0;
  for (int i = 0; i < bufferSize; i++) {
    avg_joystick_input_X += circularBufferX[i];
    avg_joystick_input_Y += circularBufferY[i];
  }
  avg_joystick_input_X = avg_joystick_input_X / bufferSize;
  avg_joystick_input_Y = avg_joystick_input_Y / bufferSize;

  //bepaal gewenste snelheid per wiel
  speed_R = avg_joystick_input_Y + avg_joystick_input_X;
  speed_L = avg_joystick_input_Y - avg_joystick_input_X;

  //map de benodigde output power adhv de gewenste snelheid en richting
  if (speed_R > 0) {
    output_power_R = constrain(speed_R, constrainValue, 255);
    output_direction_R = LOW;
  } else {
    output_power_R = constrain(255 + speed_R, constrainValue, 255);
    output_direction_R = HIGH;
  }

  if (speed_L > 0) {
    output_power_L = constrain(speed_L, constrainValue, 255);
    output_direction_L = LOW;
  } else {
    output_power_L = constrain(255 + speed_L, constrainValue, 255);
    output_direction_L = HIGH;
  }
  /* END WHEELS CALCULATIONS */

  // wheel motor driver
  payload[0] = output_power_L;
  payload[1] = output_power_R;
  payload[2] = output_direction_L;
  payload[3] = output_direction_R;

  // playing the right melodies
  if (output_direction_R && output_direction_L && 
              output_power_L < 220 && output_power_R < 220) {
    if (!prev_bw) {
      payload[4] = BACKWARDS;
      prev_bw = true;
    } else {
      otherSounds();
    }
  } else {
    if (prev_bw) {
      payload[4] = NOSOUND;
      prev_bw = false;
    } else {
      otherSounds();
    }
  }
  //payload[5];

  // potentiometer state
  int potValue = analogRead(potPin);
  payload[7] = map(potValue, 0, 1023, 0, 255);
}

void checkInputs2() {
  joystick_input_X = map(analogRead(x1Pin), 0, 1023, -255, 255);
  joystick_input_X2 = map(analogRead(x2Pin), 0, 1023, -255, 255);

  speed_R = joystick_input_X;
  speed_L = joystick_input_X2;
  Serial.println(speed_R);
  Serial.println(speed_L);

  int output_power_R     = 0,
      output_direction_R = 0,
      output_power_L     = 0,
      output_direction_L = 0;


  if (joystick_input_X > 0) {
    output_power_R = joystick_input_X;
    output_direction_R = LOW;
  } else {
    output_power_R = 255 + joystick_input_X;
    output_direction_R = HIGH;
  }

  if (joystick_input_X2 > 0) {
    output_power_L = joystick_input_X2;
    output_direction_L = LOW;
  } else {
    output_power_L = 255 + joystick_input_X2;
    output_direction_L = HIGH;
  }

  payload[0] = output_power_L;
  payload[1] = output_power_R;
  payload[2] = output_direction_L;
  payload[3] = output_direction_R;
}

void otherSounds() {
  if (stickPress()) {
    payload[4] = SANS;
  } else if (buttonPress()) {
    payload[4] = AMOGUS;
  } else {
    payload[4] = PASS;
  }
}

void sendRadio() {
  radio.write(&payload, sizeof(payload));
  Serial.print("sending payload -- ");
  for (int i = 0; i < 32; i++) {
    Serial.print(payload[i]);
    Serial.print(" ");
  }
  Serial.println("");
}

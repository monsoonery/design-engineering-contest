/* LIBRARIES */
#include <SPI.h>
#include <RF24.h>
#include <CircularBuffer.h>
#include "pitches.h"

/* RADIO */
RF24 radio(9, 10);                              // create an RF24 object, pin 9 is CE, pin 10 is CSN
const byte pipeName[6] = "99999";               // our custom pipe address
byte payload[32] = {0};                         // array of bytes that will be sent over RF, size 32 bytes

/* BUFFER DEFINITIONS */
const int bufferSize = 5;
CircularBuffer <float, bufferSize> circularBufferX;
CircularBuffer <float, bufferSize> circularBufferY;

/* PINS */
const int x1Pin = A0;                           // x-position of joystick
const int y1Pin = A1;                           // y-position of joystick
const int defendButtonPin = A2;
const int stickPin = A3;                         // joystick button

const int buzzerPin = 2;
const int grabButtonPin = 3;                    // grab button
const int precisionLedPin = 4;                  // indicator light for precision mode
const int blueLedPin = 5;                       // blue led button - led
const int blueButtonPin = 6;                    // blue led button - button
const int redLedPin = 7;                        // red led button - led
const int redButtonPin = 8;                     // red led button - button

const int potPin = A6;

/* MELODY LIST */
enum melodyType {
  PASS,
  AMOGUS,
  CURB,
  SANS,
  BACKWARDS,
  SPACE,
  DENY,
  ACCEPT,
  NOSOUND
};  // KEEP THIS LIST UP TO DATE ACCROSS BOTH CODE FILES OR SHIT GOES HAYWIRE

/* VARIABLES */
const uint8_t constrainValue = 20,
              maxSpeedPower = 255,
              maxSteerPower = 100,
              maxSpeedPowerPrec = 100,
              maxSteerPowerPrec = 50;
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
bool precisionMode = false,
     trapdoorOpen = false,
     doorOpen = false,
     defending = false;

/* RED BUTTON DEBOUNCE */
boolean lastRedButtonPressed = false;
boolean redButtonPress() {
  boolean redButtonStatus = false;
  boolean redButtonPressed = !digitalRead(redButtonPin);
  if (redButtonPressed && !lastRedButtonPressed) {
    redButtonStatus = true;
    digitalWrite(redLedPin, !digitalRead(redLedPin));
  }
  lastRedButtonPressed = redButtonPressed;
  return redButtonStatus;
}

/* BLUE BUTTON DEBOUNCE */
boolean lastBlueButtonPressed = false;
boolean blueButtonPress() {
  boolean blueButtonStatus = false;
  boolean blueButtonPressed = !digitalRead(blueButtonPin);
  if (blueButtonPressed && !lastBlueButtonPressed) {
    blueButtonStatus = true;
    digitalWrite(blueLedPin, !digitalRead(blueLedPin));
  }
  lastBlueButtonPressed = blueButtonPressed;
  return blueButtonStatus;
}

/* GRAB BUTTON DEBOUNCE */
boolean lastGrabButtonPressed = false;
boolean grabButtonPress() {
  boolean grabButtonStatus = false;
  boolean grabButtonPressed = digitalRead(grabButtonPin);
  if (grabButtonPressed && !lastGrabButtonPressed) {
    grabButtonStatus = true;
  }
  lastGrabButtonPressed = grabButtonPressed;
  return grabButtonStatus;
}

/* DEFEND MODE DEBOUNCE */
boolean lastDefendButtonPressed = false;
boolean defendButtonPress() {
  boolean defendButtonStatus = false;
  boolean defendButtonPressed = digitalRead(defendButtonPin);
  if (defendButtonPressed && !lastDefendButtonPressed) {
    defendButtonStatus = true;
  }
  lastDefendButtonPressed = defendButtonPressed;
  return defendButtonStatus;
}

/* STICK DEBOUNCE */
boolean lastStickPressed = false;
boolean stickPress() {
  boolean stickStatus = false;
  boolean stickPressed = !digitalRead(stickPin);
  if (stickPressed && !lastStickPressed) {
    stickStatus = true;
  }
  lastStickPressed = stickPressed;
  return stickStatus;
}

void setup() {
  // start serial
  Serial.begin(9600);
  Serial.println("Initializing...");

  // use internal 10k pullup resistor for joystick button
  Serial.print("I/O... ");
  pinMode(stickPin, INPUT_PULLUP);
  Serial.println("done.");

  // setup radio transmitter
  Serial.print("Radio... ");
  if (!radio.begin()) {
    Serial.print("failed! Check wires and reset Arduino.");
    while (1);
  }
  radio.openWritingPipe(pipeName);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  Serial.println("done.");

  //clear the buffers
  Serial.print("Emptying buffers... ");
  for (int i = 0; i < bufferSize; i++) {
    circularBufferX.push(0);
    circularBufferY.push(0);
  }
  Serial.println("done.");

  // play sussy music
  const int melody[] = {NOTE_D4, NOTE_D4, NOTE_D5, NOTE_A4, 0, NOTE_GS4, NOTE_G4, NOTE_F4, NOTE_D4, NOTE_F4, NOTE_G4};
  const uint8_t noteDurations[] = {8, 8, 4, 3, 16, 4, 4, 4, 8, 8, 8};
  const uint8_t melodyLength = sizeof(melody) / sizeof(melody[0]);
  for (uint8_t note = 0; note < melodyLength; note++) {
    int noteDuration = 800 / noteDurations[note];
    tone(buzzerPin, melody[note], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzerPin);                                            //play no sound through the buzzer
  }
  Serial.println("Initialization complete!");
}

void loop() {
  checkInputs();
  sendRadio();                                  // send inputs over RF
  delay(10);                                    // wait before next loop
}

void checkInputs() {
  // press stick to (de)activate precision mode
  if (stickPress()) {
    precisionMode = !precisionMode;
    digitalWrite(precisionLedPin, precisionMode);
    if (precisionMode) {
      tone(buzzerPin, NOTE_C2, 50);  // auditory feedback on controller
    } else {
      tone(buzzerPin, NOTE_B2, 50);
    }
  }

  /* START WHEEL CALCULATIONS */
  // read joystick values
  int X = analogRead(x1Pin);
  int Y = analogRead(y1Pin);
  // map the joystick values to byte-sized speed values
  // (which are smaller if precision mode is activated)
  if (precisionMode) {
    joystick_input_X = map(X, 0, 1023, -maxSteerPowerPrec, maxSteerPowerPrec);
    joystick_input_Y = map(Y, 0, 1023, -maxSpeedPowerPrec, maxSpeedPowerPrec);
  } else {
    joystick_input_X = map(X, 0, 1023, -maxSteerPower, maxSteerPower);
    joystick_input_Y = map(Y, 0, 1023, -maxSpeedPower, maxSpeedPower);
  }

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
  /* END WHEEL CALCULATIONS */

  // wheel speed and direction
  payload[0] = output_power_L;
  payload[1] = output_power_R;
  payload[2] = output_direction_L;
  payload[3] = output_direction_R;

  // playing the backwards driving sound (aka spaghetticode) 
  // activating another sound/melody will interrupt this sound!
  // todo checken of het ook werkt zonder prev_bw aangezien ik nu heb if !melodyplaying in de receiver code
  if (output_direction_R && output_direction_L &&
      output_power_L < 220 && output_power_R < 220) {
    if (!prev_bw) {
      payload[4] = BACKWARDS;
      prev_bw = true;
    } else {
      payload[4] = PASS;
    }
  } else {
    if (prev_bw) {
      payload[4] = NOSOUND;
      prev_bw = false;
    } else {
      payload[4] = PASS;
    }
  }

  // press button for grab and throw block sequence
  if (grabButtonPress()) {
    payload[5] = true;
    tone(buzzerPin, NOTE_A5, 50);                 // auditory feedback on controller            
    payload[4] = DENY;                               // play the grab block sound on the car
  } else {
    payload[5] = false;
  }

  // todo if button press open trap door
  if (redButtonPress()) {
    payload[6] = true;
    trapdoorOpen = !trapdoorOpen;
    if (trapdoorOpen) {
      tone(buzzerPin, NOTE_A5, 50);                 // auditory feedback on controller            
      payload[4] = DENY;                               // play the open trapdoor sound on the car
    } else {
      tone(buzzerPin, NOTE_G4, 50);                 // auditory feedback on controller
      payload[4] = DENY;                               // play the close trapdoor sound on the car
    }
  } else {
    payload[6] = false;
  }

  // todo if button press open or close big doors
  if (blueButtonPress()) {
    payload[7] = true;
    doorOpen = !doorOpen;
    // auditory feedback on controller
    if (doorOpen) {
      tone(buzzerPin, NOTE_F4, 50);                 // auditory feedback on controller 
      //payload[4] = DENY;                               // play the open door sound on the car
    } else {
      tone(buzzerPin, NOTE_E4, 50);                 // auditory feedback on controller 
      //payload[4] = DENY;                               // play the close door sound on the car
    }
  } else {
    payload[7] = false;
  }

  // turn potentiometer to adjust belt speed/direction
  int potValue = analogRead(potPin);
  payload[8] = map(potValue, 0, 1023, 0, 255);
  if (payload[8] % 42 - 2 == 0) {
    // auditory feedback on controller every 1/6th step
    tone(buzzerPin, 1000, 50);
  }

  // press button to activate defend mode
  if (defendButtonPress()) {
    payload[9] = true;
    defending = !defending;
    if (defending) {
      tone(buzzerPin, NOTE_D4, 50);                         // auditory feedback on controller
      payload[4] = DENY;                                       // play DENY
    } else {
      tone(buzzerPin, NOTE_C4, 50);                         // auditory feedback on controller
      payload[4] = ACCEPT;                                       // play ACCEPT
    }
  } else {
    payload[9] = false;
  }
}


void sendRadio() {
  radio.write(&payload, sizeof(payload));
  debugRadio();
}

void debugRadio() {
  Serial.print(F("sent: "));
  for (int i = 0; i < 32; i++) {
    Serial.print(payload[i]);
    Serial.print(F(" "));
  }
  Serial.println("");
}

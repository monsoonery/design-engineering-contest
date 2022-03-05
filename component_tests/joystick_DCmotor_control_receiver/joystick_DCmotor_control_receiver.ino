// libraries
#include <SPI.h>
#include <RF24.h>

RF24 radio(9, 10);                           // create an RF24 object, pin 9 is CE, pin 10 is CSN
const byte pipeName[6] = "99999";            // our custom pipe adress
byte payload[32] = {0};                      // array of bytes that will be sent over RF

// values corresponding to neutral position of joystick
const int centerX = 124;                     
const int centerY = 129;

// pin definitions
const int ledPin = 4;
const int forwardPin = 5;
const int backwardPin = 6;

// payload values
int VRX = centerX;                           // [0] init to neutral position
int VRY = centerY;                           // [1] init to neutral position
int GRX = centerX;                           // [2] init to neutral position
int GRY = centerY;                           // [3] init to neutral position
bool VRS = false;                            // [4] init to button non-pressed
bool GRS = false;                            // [5] init to button non-pressed

void setup() {
  //Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  
  radio.begin();                             // setup myRadio module
  radio.openReadingPipe(0, pipeName);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();                    // this is a receiver
}

void loop() {
  checkRadio();                              // check for new incoming transmissions
  updateWheels();                            // actuate wheels according to transmission data
  updateLed();                               // power LED according to transmission data
}


void checkRadio() {
  if (radio.available()) {
    Serial.print("receiving data: ");
    radio.read(&payload, sizeof(payload));
    for (int i = 0; i < 32; i++) {
      Serial.print(payload[i]); Serial.print("  ");
    }
    Serial.println();
    unpackPayload();                         // put data in variables for further processing
  }
}

void unpackPayload() {
  // x and y values from V-joystick
  VRX = payload[0];
  VRY = payload[1];
  // x and y values from G-joystick
  GRX = payload[2];
  GRY = payload[3];
  // button press state from joysticks
  VRS = payload[4]; 
  GRS = payload[5];
}

void updateWheels() {
  // als joystick omhoog gericht, dan moet hij naar voren rijden, dus gebruik backwardPin
  if (VRX < centerX) {
    int estPower = (centerX - VRX) * 2;
    int power = constrain(estPower, 0, 255);
    analogWrite(backwardPin, power);
    analogWrite(forwardPin, 0);
    Serial.print(power);
    Serial.println(" -- forward");
    // als joystick omlaag gericht, dan moet hij naar achter rijden, dus gebruik pin backwardPin
  } else if (VRX > centerX) {
    int estPower = (VRX - centerX) * 2;
    int power = constrain(estPower, 0, 255);
    analogWrite(backwardPin, 0);
    analogWrite(forwardPin, power);
    Serial.print(power);
    Serial.print(" -- backward");
    // als joystick in neutrale positie, dan moet hij niet rijden
  } else {
    analogWrite(backwardPin, 0);
    analogWrite(forwardPin, 0);
    Serial.print(0);
    Serial.print("neutral");
  }
  delay(10);
}

void updateLed() {
  // led turns on when joystick is pressed
  digitalWrite(ledPin, VRS ? HIGH : LOW);
  if (VRS) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

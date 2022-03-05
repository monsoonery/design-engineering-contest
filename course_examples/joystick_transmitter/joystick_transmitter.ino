//Include Libraries
#include <SPI.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(9, 10);  // Arduino pin 9 is CE, pin 10 is CSN

//address through which to communicate.
const byte pipeName[6] = "99999";

uint8_t drempelwaarde = 20;

const int x1Pin = A1;
const int y1Pin = A0;

const int x2Pin = A4;
const int y2Pin = A3;

int previousVRX,
    previousVRY,
    previousGRX,
    previousGRY = 0;


void setup() {
  Serial.begin(9600);
  //initialize the radio module
  radio.begin();
  radio.openWritingPipe(pipeName); //use this pipe name
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();     //Set module as transmitter
}

void loop() {
  // The contents of this array of bytes gets transported; it starts filled with zeros
  byte payload[32] = {0};

  // take the position of the joysticks connected to analog pins
  int VRX = analogRead(x1Pin);
  int VRY = analogRead(y1Pin);
  int GRX = analogRead(x2Pin);
  int GRY = analogRead(y2Pin);

//  Serial.println(F("pog"));
//  Serial.println(abs(VRX - previousVRX));
//
//  int previousVRX = VRX;
//
//  Serial.println(abs(VRX - previousVRX));

  // did the joystick values change significantly?
  if ((abs(VRX - previousVRX) > drempelwaarde) || (abs(VRY - previousVRY) > drempelwaarde) ||
      (abs(GRX - previousGRX) > drempelwaarde) || (abs(GRY - previousGRY) > drempelwaarde)) {
    // map it from 0 - 1023 to 0 - 255
    Serial.println(F("pass"));
    payload[0] = map(VRX, 0, 1023, 0, 255);
    payload[1] = map(VRY, 0, 1023, 0, 255);
    payload[2] = map(GRX, 0, 1023, 0, 255);
    payload[3] = map(GRY, 0, 1023, 0, 255);

    //Send message
    Serial.print(" -- sending payload");
    radio.write(&payload, sizeof(payload));
    delay(10);

    previousVRX = VRX;
    previousVRY = VRY;
    previousGRX = GRX;
    previousGRY = GRY;
  }
}

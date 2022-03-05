// libraries
#include <SPI.h>
#include <RF24.h>

// radio communications
RF24 radio(9, 10);                              // create an RF24 object, pin 9 is CE, pin 10 is CSN
const byte pipeName[6] = "99999";               // our custom pipe address
byte payload[32] = {0};                         // array of bytes that will be sent over RF

// pin definitions
const int x1Pin = A1;                           // x-position of joystick
const int y1Pin = A0;                           // y-position of joystock
const int button1Pin = 4;                       // joystick button

// payload values
int VRX, 
    VRY;
bool buttonPress;

void setup() {
  Serial.begin(9600);
  pinMode(button1Pin, INPUT_PULLUP);            // use internal 10k pullup resistor for joystick button
  radio.begin();                                // setup myRadio module
  radio.openWritingPipe(pipeName);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();                        // this is a transmitter
}

void loop() {
  checkInputs();                                // check for new controller inputs
  sendRadio();                                  // send inputs over RF
  delay(10);                                    // wait before next loop
}

void checkInputs() {
  VRX = analogRead(x1Pin);                      // read position of the joysticks
  VRY = analogRead(y1Pin);
  buttonPress = !digitalRead(button1Pin);       // read button state
  // TODO: implement debounce
  preparePayload();                             // put values in array for sending over RF
}

void preparePayload() {
  // map joystick range to byte-sized range
  payload[0] = map(VRX, 0, 1023, 0, 255);
  payload[1] = map(VRY, 0, 1023, 0, 255);
  // button state
  payload[4] = buttonPress;
}

void sendRadio() {
  Serial.print("sending payload -- "); Serial.println(payload[4]);
  radio.write(&payload, sizeof(payload));
}

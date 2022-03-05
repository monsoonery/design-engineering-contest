//Include Libraries
#include <SPI.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(9, 10);  // Arduino pin 9 is CE, pin 10 is CSN
const byte pipeName[6] = "99999";

int x1Pin = A1;
int y1Pin = A0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
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
  Serial.println(VRX);

  payload[0] = map(VRX, 0, 1023, 0, 255);
  payload[1] = map(VRY, 0, 1023, 0, 255);

  Serial.print(payload[0]);
  Serial.println(" -- sending payload");
  radio.write(&payload, sizeof(payload));
  delay(10);
}

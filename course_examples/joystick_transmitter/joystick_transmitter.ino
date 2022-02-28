//Include Libraries
#include <SPI.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(9, 10);  // Arduino pin 9 is CE, pin 10 is CSN

//address through which to communicate.
const byte pipeName[6] = "12345";

void setup(){
  Serial.begin(9600);
  //initialize the radio module
  radio.begin();
  radio.openWritingPipe(pipeName); //use this pipe name
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();     //Set module as transmitter
}

void loop(){
  // The contents of this array of bytes gets transported; it starts filled with zeros
  byte payload[32] = {0};
  
  // take the position of the joystick connected to A0 and A1
  int VRX = analogRead(A0);
  int VRY = analogRead(A1);
  // map it from 0 - 1023 to 0 - 255
  payload[0]=map(VRX,0,1023,0,255);
  payload[1]=map(VRY,0,1023,0,255);

  //Send message
  Serial.print(" -- sending payload");
  radio.write(&payload, sizeof(payload));
  
  delay(1000);
}

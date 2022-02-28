//Include Libraries
#include <SPI.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(9, 10);  // Arduino pin 9 is CE, pin 10 is CSN

//address through which to communicate.
const byte pipeName[6] = {1,2,3,4,5,6};

void setup(){
  Serial.begin(9600);
  //initialize the radio module
  radio.begin();
  radio.openWritingPipe(pipeName); //use this pipe name
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();     //Set module as transmitter
}

void loop(){
  char text[]="hello world";

  //Send message
  Serial.print(" -- sending text");
  radio.write(&text, sizeof(text));
  
  delay(1000);
}

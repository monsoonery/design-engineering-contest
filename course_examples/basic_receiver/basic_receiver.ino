// this is the receiver.
#include <SPI.h>
#include <RF24.h>

// configure myRadio module
RF24 myRadio(9, 10); // CE, CSN
const byte pipeName[6] = {1,2,3,4,5,6};

void setup() {
  Serial.begin(9600);
  myRadio.begin();      // setup myRadio module
  myRadio.openReadingPipe(0, pipeName);  // the radio can listen to 6 transmitters, we only use one.
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.startListening();
}

void loop() {
  char text[32] = "";
  if (myRadio.available())
  {
    Serial.print("got a message:  ");
    myRadio.read(&text, sizeof(text));
    Serial.println(text);
  }
  delay(10);
}

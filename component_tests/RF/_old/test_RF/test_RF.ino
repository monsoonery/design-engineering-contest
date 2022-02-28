#include <SPI.h>
#include <RF24.h>

RF24 myRadio(9, 10); // CE is pin 9, die andere is pin 10
const byte pipeAdress[6] = "42069";

void setup() {
  Serial.begin(9600);
  myRadio.begin();
  myRadio.openReadingPipe(0, pipeAdress);
  myRadio.setPALevel(RF24_PA_MIN);
}

void loop() {
  myRadio.stopListening();
  char Payload[] = "faka jens";
  Serial.println(" -- sending payload");
  myRadio.write(&Payload, sizeof(Payload));
  delay(1000);  
}

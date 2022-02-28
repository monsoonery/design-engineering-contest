// this is the receiver.
#include <SPI.h>
#include <RF24.h>

// configure myRadio module
RF24 myRadio(9, 10); // CE, CSN
const byte pipeName[6] = "12345";

void setup(void) {
  Serial.begin(9600);
  myRadio.begin();      // setup myRadio module
  myRadio.openReadingPipe(0, pipeName);  // the radio can listen to 6 transmitters, we only use one.
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.startListening();
}

void loop() {
  byte payload[32] = {0};  // Storage for the received data. First it is emptied

  if (myRadio.available())
  {
    Serial.print("receiving data: ");
    myRadio.read(&payload, sizeof(payload));
    for (int i = 0; i < 32; i++) {
      Serial.print(payload[i]);
      Serial.print("  ");
    }
    Serial.println();
  }
  delay(10);
}

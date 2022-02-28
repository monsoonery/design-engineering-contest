#include <SPI.h>
#include "printf.h"
#include "RF24.h"

// instantiate an object for the nRF24L01 transceiver
RF24 radio(9, 10); // using pin 7 for the CE pin, and pin 8 for the CSN pin

uint8_t address[][6] = {"1Node", "2Node"};

bool radioNumber = 0; 
bool role = false;  // true = TX role, false = RX role

float payload = 0.0;

void setup() {
  Serial.begin(115200);
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {} // hold in infinite loop
  }

  Serial.println(F("Receiver Test v1.0"));

  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.
  radio.setPayloadSize(sizeof(payload)); // float datatype occupies 4 bytes

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[0]);     // always uses pipe 0
  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[1]); // using pipe 1

  radio.startListening(); // put radio in RX mode
} // setup

void loop() {
  // This device is a RX node
  uint8_t pipe;
  if (radio.available(&pipe)) {             // is there a payload? get the pipe number that recieved it
    uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
    radio.read(&payload, bytes);            // fetch payload from FIFO
    Serial.print(F("Received "));
    Serial.print(bytes);                    // print the size of the payload
    Serial.print(F(" bytes on pipe "));
    Serial.print(pipe);                     // print the pipe number
    Serial.print(F(": "));
    Serial.println(payload);                // print the payload's value
  }
}

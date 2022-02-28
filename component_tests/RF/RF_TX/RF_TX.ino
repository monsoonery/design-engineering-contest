#include <SPI.h>
#include "printf.h"
#include "RF24.h"

// instantiate an object for the nRF24L01 transceiver
RF24 radio(9, 10); // using pin 7 for the CE pin, and pin 8 for the CSN pin

uint8_t address[][6] = {"1Node", "2Node"};

bool radioNumber = 1; 
bool role = true;  // true = TX role, false = RX role

float payload = 0.0;

void setup() {
  Serial.begin(115200);
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {} // hold in infinite loop
  }

  Serial.println(F("Transmitter Test v1.0"));

  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.
  radio.setPayloadSize(sizeof(payload)); // float datatype occupies 4 bytes

  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[1]);     // always uses pipe 0
  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[0]); // using pipe 1

  radio.stopListening();  // put radio in TX mode
} // setup

void loop() {
  // This device is a TX node
  unsigned long start_timer = micros();                    // start the timer
  bool report = radio.write(&payload, sizeof(float));      // transmit & save the report
  unsigned long end_timer = micros();                      // end the timer

  if (report) {
    Serial.print(F("Transmission successful! "));          // payload was delivered
    Serial.print(F("Time to transmit = "));
    Serial.print(end_timer - start_timer);                 // print the timer result
    Serial.print(F(" us. Sent: "));
    Serial.println(payload);                               // print payload sent
    payload += 0.01;                                       // increment float payload
  } else {
    Serial.println(F("Transmission failed or timed out")); // payload was not delivered
  }
  delay(1000);
} 

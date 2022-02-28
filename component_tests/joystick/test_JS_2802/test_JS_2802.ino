#include <SPI.h>
#include <RF24.h>

uint16_t JSX = 0; //x-coord of joystick
uint16_t JSY = 0; //y-coord of joystick
const uint8_t pinJSX = A0; //joystick pin transmitting x-coord
const uint8_t pinJSY = A1; //joystick pin transmitting y-coord

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
}

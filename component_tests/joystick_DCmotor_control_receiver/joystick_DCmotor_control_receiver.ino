//Include Libraries
#include <SPI.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(9, 10);  // Arduino pin 9 is CE, pin 10 is CSN
const byte pipeName[6] = "99999";

const int centerX = 124;
int VRX = centerX;

void setup() {
  // put your setup code here, to run once:
  radio.begin();      // setup myRadio module
  radio.openReadingPipe(0, pipeName);  // the radio can listen to 6 transmitters, we only use one.
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  // Storage for the received data. First it is emptied
  byte payload[32] = {0};

  if (radio.available()) {
    Serial.print("receiving data: ");
    radio.read(&payload, sizeof(payload));
    for (int i = 0; i < 32; i++) {
      Serial.print(payload[i]);
      Serial.print("  ");
    }
    Serial.println();

    VRX = payload[0];
    updateWheels();
  }
}

void updateWheels() {
  // als joystick omhoog gericht, dan moet hij naar voren rijden, dus gebruik pin 5
  if (VRX < centerX) {
    int estPower = (centerX - VRX) * 2;
    int power = constrain(estPower, 0, 255);
    analogWrite(5, power);
    analogWrite(6, 0);
    Serial.print(power);
    Serial.println(" -- forward");
    // als joystick omlaag gericht, dan moet hij naar achter rijden, dus gebruik pin 5
  } else if (VRX > centerX) {
    int estPower = (VRX - centerX) * 2;
    int power = constrain(estPower, 0, 255);
    analogWrite(5, 0);
    analogWrite(6, power);
    Serial.print(power);
    Serial.print(" -- backward");
    // als joystick in neutrale positie, dan moet hij niet rijden
  } else {
    analogWrite(5, 0);
    analogWrite(6, 0);
    Serial.print(0);
    Serial.print("neutral");
  }
  delay(10);
}

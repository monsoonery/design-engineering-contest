// libraries
#include <I2Cdev.h>
#include <PCA9685.h>
#include <Wire.h>

ServoDriver servo;                            // servodriver object

const int servoPin = 1;                       // pin number on DRIVER, NOT on Arduino board

void setup() {
  Serial.begin(9600);
  Wire.begin();                               // join I2C bus (I2Cdev library doesn't do this automatically)
  servo.init(0x7f);                           // init driver address
  servo.setServoPulseRange(500, 2500, 360);   // pulse range for our servos
}

void loop() {
  for (int i = 90; i <= 270; i++) {
    servo.setAngle(servoPin, i);                     // loop through all values
    Serial.println(i);
    delay(50);
  }
  servo.setAngle(servoPin, 270);                     // maximum rotation speed for 1 second
  Serial.println("max linksom");
  delay(1000);
  servo.setAngle(servoPin, 180);                     // no rotation, neutral position for 1 second
  Serial.println("neutral");
  delay(1000);
  servo.setAngle(servoPin, 90);                      // maximum rotation speed for 1 second (opposite direction)
  Serial.println("max rechtsom");
  delay(1000);
}

int x1Pin = A1;
int y1Pin = A0;

const int centerX = 129;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // The contents of this array of bytes gets transported; it starts filled with zeros
  //byte payload[32] = {0};

  // take the position of the joysticks connected to analog pins
  int VRX = map(analogRead(x1Pin), 0, 1023, 0, 255);
  int VRY = map(analogRead(y1Pin), 0, 1023, 0, 255);
  //Serial.println(VRX);


  if (VRX < centerX) {
    int estPower = (centerX - VRX) * 2;
    int power = constrain(estPower, 0, 255);
    analogWrite(5, power);
    analogWrite(6, 0);
    Serial.print(power);
    Serial.println(" -- forward");
  } else if (VRX > centerX) {
    int estPower = (VRX - centerX) * 2;
    int power = constrain(estPower, 0, 255);
    analogWrite(5, 0);
    analogWrite(6, power);
    Serial.print(power);
    Serial.print(" -- backward");
  } else {
    analogWrite(5, 0);
    analogWrite(6, 0);
    Serial.print(0);
    Serial.print("neutral");
  }

  delay(10);
}

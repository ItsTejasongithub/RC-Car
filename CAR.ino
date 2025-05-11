// Motor pins (L298P Motor Shield)
const int pwmA = 10;     // Motor A speed (PWM)
const int dirA = 12;     // Motor A direction
const int pwmB = 11;     // Motor B speed (PWM)
const int dirB = 13;     // Motor B direction
const int buzzerPin = 4; // Buzzer (on button press)

// Joystick calibration
const int centerX = 518;
const int centerY = 511;
const int deadZone = 50;

// Bluetooth timeout
unsigned long lastCommandTime = 0;
const unsigned long timeoutMs = 1000;

// Motor speeds
int leftMotorSpeed = 0;
int rightMotorSpeed = 0;

void setup() {
  Serial.begin(38400);  // For built-in HC-05 on L298P

  pinMode(pwmA, OUTPUT);
  pinMode(dirA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(dirB, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(buzzerPin, LOW);
  stopMotors();

  Serial.println("Bluetooth Car Ready with Buzzer on Button Press");
}

void loop() {
  if (Serial.available()) {
    String dataString = Serial.readStringUntil('\n');
    int xVal, yVal, button;

    if (sscanf(dataString.c_str(), "%d,%d,%d", &xVal, &yVal, &button) == 3) {
      Serial.print("X: "); Serial.print(xVal);
      Serial.print(", Y: "); Serial.print(yVal);
      Serial.print(", Button: "); Serial.println(button);

      lastCommandTime = millis();
      calculateMotorSpeeds(xVal, yVal);
      controlMotors(button);  // <- updated here
    }
  }

  if (millis() - lastCommandTime > timeoutMs) {
    if (leftMotorSpeed != 0 || rightMotorSpeed != 0) {
      Serial.println("Timeout: Stopping motors");
      stopMotors();
      leftMotorSpeed = 0;
      rightMotorSpeed = 0;
    }
  }
}

void calculateMotorSpeeds(int x, int y) {
  int xOffset = x - centerX;
  int yOffset = centerY - y;  // Up = forward

  if (abs(xOffset) < deadZone) xOffset = 0;
  if (abs(yOffset) < deadZone) yOffset = 0;

  int speed = 0;
  if (yOffset != 0) {
    speed = map(abs(yOffset), 0, 600, 0, 255);
    if (yOffset < 0) speed = -speed;
  }

  int turn = 0;
  if (xOffset != 0) {
    turn = map(xOffset, -centerX, (1023 - centerX), -255, 255);
  }

  leftMotorSpeed = constrain(speed + turn, -255, 255);
  rightMotorSpeed = constrain(speed - turn, -255, 255);
}

void controlMotors(int buttonState) {
  controlMotor(pwmA, dirA, leftMotorSpeed);
  controlMotor(pwmB, dirB, rightMotorSpeed);

  // Buzzer on joystick button press
  if (buttonState == 0) {
    digitalWrite(buzzerPin, HIGH);
  } else {
    digitalWrite(buzzerPin, LOW);
  }
}

void controlMotor(int pwmPin, int dirPin, int speed) {
  if (speed > 0) {
    digitalWrite(dirPin, HIGH);
    analogWrite(pwmPin, speed);
  } else if (speed < 0) {
    digitalWrite(dirPin, LOW);
    analogWrite(pwmPin, abs(speed));
  } else {
    analogWrite(pwmPin, 0);
  }
}

void stopMotors() {
  analogWrite(pwmA, 0);
  analogWrite(pwmB, 0);
  digitalWrite(buzzerPin, LOW);
}

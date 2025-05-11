#include <SoftwareSerial.h>
 
#define VRX A0  // Joystick X-Axis
#define VRY A1  // Joystick Y-Axis
#define SW 4    // Joystick button switch
 
SoftwareSerial BTSerial(11, 10);  // RX | TX
 
void setup() {
  Serial.begin(9600);
  BTSerial.begin(38400);  
  pinMode(SW, INPUT_PULLUP);  // Button with internal pull-up
  Serial.println("Bluetooth Connected! Ready for Communication.");
}
 
void loop() {
  int xVal = analogRead(VRX);
  int yVal = analogRead(VRY);
  int buttonState = digitalRead(SW);
 
  // Format data string
  String dataOut = String(xVal) + "," + String(yVal) + "," + String(buttonState);
 
  // Send data via Bluetooth
  BTSerial.println(dataOut);
 
  // Also print to Serial Monitor for debugging
  Serial.println("Sent: " + dataOut);
 
  delay(100); // Small delay for stability
}

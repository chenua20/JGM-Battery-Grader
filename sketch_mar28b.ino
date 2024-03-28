void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 
  pinMode(33, OUTPUT);    // sets the digital pin 13 as output
  Serial.println("PIN SET");
}

void loop() {
  digitalWrite(33, HIGH); // sets the digital pin 13 on
  delay(1000);            // waits for a second
  Serial.println("ON");  // Power
  digitalWrite(33, LOW);  // sets the digital pin 13 off
  delay(1000);            // waits for a second
  Serial.println("OFF");  // Power
}

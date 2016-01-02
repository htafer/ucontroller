void setup() {
  pinMode(0, OUTPUT);
}

void loop() {
  digitalWrite(0, HIGH);
  delay(1500);
  digitalWrite(0, LOW);
  delay(500);
}

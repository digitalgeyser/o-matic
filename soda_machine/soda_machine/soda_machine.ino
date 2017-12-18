void setup() {
  pinMode(7, OUTPUT);
}

void loop() {
  delay(1000);
  digitalWrite(7, HIGH);
  delay(1000);
  digitalWrite(7, LOW);
}

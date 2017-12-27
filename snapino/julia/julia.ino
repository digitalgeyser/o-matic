void setup() {
  pinMode(9, OUTPUT);
}

void loop() {
  digitalWrite(9, HIGH);
  delay(100);
  digitalWrite(9, LOW);
  delay(200);
  digitalWrite(9, HIGH);
  delay(300);
  digitalWrite(9, LOW);
  delay(400);
}

void setup() {
  pinMode(16, OUTPUT);
  pinMode(2, OUTPUT);
}

void loop() {
  digitalWrite(16, LOW);
  digitalWrite(2, HIGH);
  delay(1000);
  digitalWrite(16, HIGH);
  digitalWrite(2, LOW);
  delay(1000);
}

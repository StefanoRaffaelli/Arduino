int sensor = 13;  // Digital pin D7

void setup() {
  pinMode(sensor, INPUT);
  Serial.begin(115200);
}

void loop() {
  long state = digitalRead(sensor);
  if (state == HIGH) {
    Serial.println("Motion detected!");
    delay(1000);
  }
  else {
    Serial.println("No motion detected.");
    delay(1000);
  }
}

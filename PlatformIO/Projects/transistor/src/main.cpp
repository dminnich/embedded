#include <Arduino.h>
const uint8_t led = 0; 

void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(led, HIGH);
  delay(8000);
  digitalWrite(led, LOW);
  delay(8000);
}
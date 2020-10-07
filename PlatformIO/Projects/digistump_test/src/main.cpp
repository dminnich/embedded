#include <Arduino.h>

const uint8_t led = PB1;
const uint8_t reed = PB0;
void setup() {
  pinMode(led, OUTPUT);
  pinMode(reed, INPUT_PULLUP);
}

void loop() {
  //digitalWrite(led, HIGH);
  //delay(1000);
  //digitalWrite(led,LOW);
  //delay(1000);
  if (digitalRead(reed) == LOW)
  {
    //magnet present
    digitalWrite(led,HIGH);
  }
  else
  {
    digitalWrite(led,LOW);
  }
}
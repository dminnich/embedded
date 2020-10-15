//main.C NOT main.CPP
//http://www.helton.de/post/development-boards-pinout/
#include <Arduino.h>
const uint8_t led = PD3;

void setup() {
  pinMode(led, OUTPUT);
}

void loop() {
  digitalWrite(led,HIGH);
  delay(1000);
  digitalWrite(led,LOW);
  delay(1000);
}
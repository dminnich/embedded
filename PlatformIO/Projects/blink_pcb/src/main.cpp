#include <Arduino.h>
/*
ATTINY 13a:
1/pb5 = nothing
2/pb3/a3 = nothing
3/pb4/a2 = nothing
4/gnd = gnd 
        led
5/pb0 = nothing
6/pb1 = 330 ohm reisistor > LED > gnd
7/pb2 = nothing
8/vcc = nothing
*/

//vars
const uint8_t led = 1; // led pin
const int bms = 1000; // blink ms 

void setup() {
  pinMode(led,OUTPUT);
}

void loop() {
  digitalWrite(led,HIGH);
  delay(bms);
  digitalWrite(led,LOW);
  delay(bms);
}
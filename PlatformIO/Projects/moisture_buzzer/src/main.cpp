/*
When somebody walks through the beam, make a noise. 

BOOTLOADER, ETC: 
https://github.com/MCUdude/MicroCore- 
https://learn.sparkfun.com/tutorials/sik-experiment-guide-for-arduino---v32/experiment-11-using-a-piezo-buzzer
https://randomnerdtutorials.com/guide-for-soil-moisture-sensor-yl-69-or-hl-69-with-the-arduino/


Buzzer:
negative = gnd
positive = 5/pb0

soil moisture sensor:
vcc = vcc
gnd = gnd
D0 = 7/pb2
A0 = nothing

ATTINY:
1/pb5 = nothing
2/pb3 = nothing
3/pb4 = nothing
4/gnd = gnd on power supply

5/pb0 = positive on buzzer
6/pb1 = nothing
7/pb2 = D0 on sensor
8/vcc = vcc


*/

#include <Arduino.h>

//vars
const uint8_t bp = 0; // buzzer pin
const uint8_t sp = 2; //sensor pin

void setup() {
pinMode(bp,OUTPUT);
digitalWrite(bp,LOW); //turn buzzer off initially
pinMode(sp,INPUT);
digitalWrite(sp, HIGH); //set no water initially 
}


void loop() {
if (digitalRead(sp) == LOW){  // if water is detected buzz
    digitalWrite(bp,HIGH);
}
else{
    digitalWrite(bp,LOW);
}
delay(300);
}
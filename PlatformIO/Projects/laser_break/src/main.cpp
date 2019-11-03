/*
When somebody walks through the beam, make a noise. 

BOOTLOADER, ETC: 
https://github.com/MCUdude/MicroCore- 
https://learn.sparkfun.com/tutorials/sik-experiment-guide-for-arduino---v32/experiment-11-using-a-piezo-buzzer
http://henrysbench.capnfatz.com/henrys-bench/arduino-sensors-and-input/arduino-ir-obstacle-sensor-tutorial-and-manual/

Buzzer:
negative = gnd
positive = 5/pb0

obstacle avoidance sensor:
out = 6/pb1
+ = vcc
- = gnd

ATTINY:
1/pb5 = nothing
2/pb3 = nothing
3/pb4 = nothing
4/gnd = gnd on power supply

5/pb0 = positive on buzzer
6/pb1 = out on sensor
7/pb2 = nothing
8/vcc = vcc


*/

#include <Arduino.h>

//vars
const long bt = 500; //time to run the buzzer
const uint8_t bp = 0; // buzzer pin
const uint8_t sp = 1; //sensor pin
unsigned long myendtime = 0; //need to add bt to current time upon event and then buzz for that long.  delays don't work with buzzers.

void setup() {
pinMode(bp,OUTPUT);
digitalWrite(bp,LOW); //turn buzzer off initially
pinMode(sp,INPUT);
digitalWrite(sp,HIGH); //set no obstacle initially
}

long endtime(long x){
    long r = x + bt;
    return r;
}

void loop() {
unsigned long currentMillis = millis();  //get current amount of ms arduino has been running
if (digitalRead(sp) == LOW){  // if person is detected set now + bt to be when the buzzer should stop buzzing
    myendtime = endtime(currentMillis);
}
if (currentMillis <= myendtime){ //buzz until this time. if person stays in the beam it will keep buzzing.
    digitalWrite(bp,HIGH);
}
else{//no more buzzing
    digitalWrite(bp,LOW);
}
delay(300);
}
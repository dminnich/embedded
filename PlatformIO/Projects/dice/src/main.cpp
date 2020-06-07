/*

Push a button and light up between 1 and 6 LEDs.
https://github.com/MCUdude/MicroCore- 
http://toptechboy.com/arduino-tutorial-42-understanding-how-to-use-a-serial-to-parallel-shift-register-74hc595/
https://www.arduino.cc/reference/en/language/functions/random-numbers/random/
https://www.arduino.cc/en/Tutorial/switchCase2

Button:
https://www.arduino.cc/en/tutorial/button
bottom - 3/pb4
bottom - 22k resistor - ground
top - vcc

74hc595 shift register:
http://2.bp.blogspot.com/-tn7lCPkm69Y/U7QzbeqIorI/AAAAAAAABEw/uYi3y1WMdR4/s1600/74hc595_function.jpg
https://components101.com/sites/default/files/component_pin/74HC595-IC-Pinout_0.png
1/q1 - 330 resistor - led 1 long leg - led short leg - ground
2/q2 - 330 resistor - led 2 long leg - led short leg - ground
3/q3 - 330 resistor - led 3 long leg - led short leg - ground
4/q4 - 330 resistor - led 4 long leg - led short leg - ground
5/q5 - 330 resistor - led 5 long leg - led short leg - ground
6/q6 - 330 resistor - led 5 long leg - led short leg - ground
7/q7 - nothing
8/gnd - ground

9/serial out - for dasiy chaining -  nothing
10/master reset - active low - vcc
11/serial clock - 5/pb0
12/latch - 6/pb1
13/output enable - ground to enable
14/serial data - 7/pb2
15/q0 - nothing
16/vcc - vcc



ATTINY:
1/pb5 = nothing. used for random data.
2/pb3 = nothing
3/pb4 = button bottom
4/gnd = gnd 

5/pb0 = 11/serial clock on shift register
6/pb1 = 12/latch on shift register
7/pb2 = 14/serial data on shift register
8/vcc = vcc


*/

#include <Arduino.h>
//setup pin numbers
const uint8_t clock = 0; 
const uint8_t latch = 1;
const uint8_t data = 2;
const uint8_t button = 4;
//we will generate a random decimal number
uint8_t randNumber;
//we will convert the random decimal into a binary number
uint8_t roll;


void setup() {
  //sending data to shift register
  pinMode(clock, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(data, OUTPUT);
  //reading data from the button
  pinMode(button, INPUT);
  //using an unconnected analog pin makes random more random according to docs
  randomSeed(analogRead(0));
}

void loop() {
  //if the button is pressed
  if (digitalRead(button) == HIGH){
    //choose a random decimal number between 1 and 6
    randNumber = random(1, 7);
    switch (randNumber){
    //convert the random decimal number to a binary number
    //0b = start binary sequence
    // 1st digit = q0 - not in use. always 0
    // 2nd = q1 .. 7th = q6
    // 8th = q7 - not in use. always 0
    //the 1's between the 0 ends set which LEDs will be on
    //ie: 0b01111110 = all on
      case 6:
        roll = 0b01111110;
        break;
      case 5:
        roll = 0b01111100;
        break;
      case 4:
        roll = 0b01111000;
        break;
      case 3:
        roll = 0b01110000;
        break;
      case 2:
        roll = 0b01100000;
        break;
      case 1:
        roll = 0b01000000;
        break;
      default:
      //if something weird happened light up a strange pattern
        roll = 0b01010100;
    }
    //send the data to the shift register
    digitalWrite(latch, LOW);
    shiftOut(data,clock,LSBFIRST,roll);
    digitalWrite(latch, HIGH);
  }
}
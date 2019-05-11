/*
BOOTLOADER, ETC: 
#https://engineersportal.com/blog/2018/5/19/attiny85-arduino-board-how-to-flash-the-arduino-bootloader-and-run-a-simple-sketch

LED:
pinout = red, ground(long leg), greeen, blue
red = 330 resistor, 5/PB0
ground = ground
green = 330 resistor, 6/PB1
blue = 330 resistor, 7/PB2
#https://www.youtube.com/watch?v=5Qi93MjlqzE


ATTINY:
1/pb5 = one side of the button
2/pb3 = nothing
3/pb4 = nothing
4/gnd = gnd on power supply

5/pb0 = 330 resistor, red led leg
6/pb1 = 330 resistor, green led leg
7/pb2 = 330 resistor, blue led leg 
8/vcc = vcc
#https://engineersportal.com/blog/2018/5/19/attiny85-arduino-board-how-to-flash-the-arduino-bootloader-and-run-a-simple-sketch


BUTTON:
both same side
1: ground
2: 1/pb5
 #https://programmingelectronics.com/how-to-use-an-external-reset-button-with-arduino/
 */

const int red = 0;
const int green = 1;
const int blue = 2; 
const int CheckFreqSec = 5;
const int CountdownHours = 5;
unsigned long uptime = 0;
//1-4 = green 
//4-5 = yello
//5+ = red
//https://www.calculateme.com/time/hours/to-milliseconds/
//unsigned long CoutdownMillis = CountdownHours * 3600000;
//unsigned long CountdownMillisMinusHour = (CountdownHours - 1) * 3600000;
//DEBUG minutes
//const int uptime = 4;
unsigned long CoutdownMillis = CountdownHours * 60000;
unsigned long CountdownMillisMinusHour = (CountdownHours - 1) * 60000;

void setup() {
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);

//on boot blink how long the countdown timer is set for
//countdown time will be off by this plus a small bit of boot time.
//so at 5 hours == 5*2 + (5+1)= 16 sec.  off by roughly 20 sec
  delay (5000);
  int i = 0;
  while (i < CountdownHours) {
    digitalWrite(blue, LOW);
    delay (1000);
    digitalWrite(blue, HIGH);
    delay (1000);
    i++;
  }
  digitalWrite(blue, LOW);
  delay (1000);
}

int AllOff(){
    digitalWrite(green, LOW);
    digitalWrite(blue, LOW);
    digitalWrite(red, LOW);
}

void loop() {
  uptime = millis();
//if between 0 uptime and hours -1 uptime
if (uptime >= 0 && uptime < CountdownMillisMinusHour) {
    AllOff();
    digitalWrite(green, HIGH);
}
//else if between hours -1 uptime and hours uptime
else if (uptime >= CountdownMillisMinusHour && uptime < CoutdownMillis) {
    AllOff();
    digitalWrite(red, HIGH);
    digitalWrite(green, HIGH);
}
//else if uptime is greater than hours
else if (uptime >= CoutdownMillis) {
    AllOff();
    digitalWrite(red, HIGH);
}
//else this sholdn't happen.  blue is error.
else {
    AllOff();
    digitalWrite(blue, HIGH);
}
delay (CheckFreqSec * 1000);
}

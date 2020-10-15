/*
STM8 board that roughly measures distance in inches and cms using a HC-SR04 ultrasonic sensor and then prints it to a Nokia 5110 display.
May need to reset it a couple times at boot with something in front of the sensor.

main.C NOT main.CPP

STM8 Pinout:
http://www.helton.de/post/development-boards-pinout/

Screen pinout:
https://lastminuteengineers.com/wp-content/uploads/arduino/Nokia-5110-LCD-Module-Pinout.png

PlatformIO and example code links
https://docs.platformio.org/en/latest/platforms/ststm8.html#examples
https://tenbaht.github.io/sduino/api/PCD8544/
https://github.com/platformio/platform-ststm8/blob/master/examples/arduino-ping-hc04/src/ping-hc04.c


STM8S103F3P6
1/PD4/D13 - 330ohm resistor - led - gnd
2/PD5/A3/D14 - nothing
3/PD6/A4/D15 - nothing
4 - reset
5/PA1/D0 - nothing
6/PA2/D1 - nothing
7 - gnd - to rail
8 - 5v - to rail - power like this and not through usb
9 - 3.3v - nothing
10/PA3/D2 - nothing

11/PB5/D3/SDA - nothing
12/PB4/D4/SCL - nothing
13/PC3/D5 - nothing
14/PC4/A0/D6 - DC on screen
15/PC5/SCK/D7 - CLK on screen
16/PC6/MOSI/D8 - DIN on screen
17/PC7/MISO/D9 - RST on screen
18/PD1/D10 - CE on screen
19/PD2/A1/D11 - echo on HC-SR04
20/PD3/A2/D12 - trigger on HC-SR04

Nokia 5510
RST - 17/PC7/MISO/D9
CE - 18/PD1/D10 - SCE on screen
DC - 14/PC4/A0/D6
DIN - 16/PC6/MOSI/D8
CLK - 15/PC5/SCK/D7
VCC - 5vv
BL - 330ohm - 5v
GND - GND

HC-SR04 
gnd - gnd
echo - 19/PD2/A1/D11 - echo on HC-SR04
trig - 20/PD3/A2/D12 - trigger on HC-SR04
vcc - vcc
*/

#include <Arduino.h>

//screen
#include <PCD8544.h>
PCD8544(lcd, PC5, PC6, PC4, PC7, PD1); // sclk,sdin,dc,reset,sce

//ultrasonic sensor
const int trigPin = PD3;
const int echoPin = PD2;

//strobe an led to let us know a new measurement is about to happen
const uint8_t led = PD4;



void setup() {
  //screen
  lcd_begin();

 //ultrasonic sensor
  digitalWrite(trigPin, HIGH);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  //led
  pinMode(led, OUTPUT);
}

void loop() {
 //ultrasonic sensor 
 //vars to store measurements
 long duration, inches, cm;
 //lcd print function whats char text array on longs
 //3 digits numbers + XX: + space + null = 8
 char sinches[8], scms[8];
 digitalWrite(trigPin, LOW);
 //wait up to 200ms for the pin to go high and then fall low
 duration = pulseInLong(echoPin, HIGH, 200000L);
 digitalWrite(trigPin, HIGH);
 //always whole numbers 
 //duration = 1132 microsecnds to get ping back
 //inches = 7,  cms = 19
 inches = duration / 74 / 2;
 cm = duration / 29 / 2;
 //https://cboard.cprogramming.com/c-programming/154538-how-do-i-convert-long-long-int-into-char-array.html
 //http://www.cplusplus.com/reference/cstdio/printf/
 //set char array equal to the formatted text from this function
 sprintf(sinches,"in: %lu",inches);
 sprintf(scms,"cm: %lu",cm);

 //screen
 //first line
  lcd_setCursor(0, 0);
  lcd_clearLine();
  lcd_print_s(sinches);
  //2nd line
  lcd_setCursor(0, 1);
  lcd_clearLine();
  lcd_print_s(scms);
  
  //led
  //informs that a measurment is about to happen
  //also slows thing down a bit so readings are more accurate and so the screen doesn't strobe
  digitalWrite(led,HIGH);
  delay(1000);
  digitalWrite(led,LOW);
  delay(1000);
}
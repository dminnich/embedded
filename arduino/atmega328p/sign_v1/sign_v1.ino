/*BOOTLOADER, ETC: 
https://www.arduino.cc/en/Tutorial/ArduinoToBreadboard


Matrix max7219 4 block
CLK = 19/13/sck
CS = 16/10/ss
DIN = 17/11/mosi
gnd = gnd
vcc = vcc

button
24/15/a1 = button
gnd

cp2102 serial programmer
5v = vcc
gnd = gnd
txd = pin 2/0
rxi = 3/1
dtr = positive side of 10uf cap > negative side of 10uf cap > 1/22
3v = nothing


atmega328p:
*may have issues using
1/22* = negative side of 10uf cap > positive side of 10uf cap > usb to serial debug and program DTR
1/22* = 10k resistor to vcc
2/0 = usb to serial debug and program transfer
3/1 = usb to serial debug and prgroam recieve
4/2 = nothing
5/3 = nothing
6/4 = nothing
7 = vcc
8 = gnd
9/20* = nothing
10/21* = nothing
11/5 = nothing
12/6/ain0 = nothing
13/7/ain1 = noting
14/8 = resistor + led long leg  for debugging

15/9 = nothing
16/10/ss = cs on matrix display
17/11/mosi = din on matrix display 
18/12/miso = nothing
19/13/sck = clk on matrix display
20 = vcc
21 = aref/nothing
22 = gnd
23/14/a0 = nothing
24/15/a1 = button
25/16/a2 = nothing
26/17/a3 = nothing
27/18/a4/sda = nothing
28/19/a5/scl = nothing

 */

//SETUP THE LED MATRIX
//libraries needed:
//md_max72xx by majicDesigns
//md_maxpanel by majicdesigns
//md_parola by majicdesigns
//lots of code poached from parola scroll example.  
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW //chinese device
#define MAX_DEVICES 4 //number of 8x8 squares
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
uint8_t scrollSpeed = 25;    // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 2000; // in milliseconds

//button  1 when not pushed.  0 when pushed.
const int BUTTON = 15;
byte ButtonState; //pushed or no?


//vars
int count = 0;
//char count of longest message plus one. or some high number if you have plenty of memory. 
#define  BUF_SIZE  25
char curMessage[BUF_SIZE] = { "" }; //holding space
//number of messages
const int NUM_OF_ELEMENTS = 7;
//array of messages
char messages [NUM_OF_ELEMENTS] [BUF_SIZE] = {
  { "Whoop Whoop" }, 
  { "Fam-a-lee" },
  { "17" },
  { "Best Sign" },
  { "More Butthole" },
  { "Fuck Off" },
  { "RIP Lahey" }
};


void setup() {
  delay(5000); //time to debug
  Serial.begin(57600);
  //button
  pinMode(BUTTON, INPUT_PULLUP);
  //screen
  P.begin(); // the display
  P.setIntensity(1);
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
}

void loop() {
ButtonState = digitalRead(BUTTON);

  if(ButtonState == LOW) { //if button pressed increase counter
    count++;
    delay(500); //try and stop some bouncing
  }
  if (count < NUM_OF_ELEMENTS) {
      Serial.println (count); //debug
      Serial.println (messages [count]); //debug
      strcpy(curMessage, messages [count]);
    }
  else { //when NUM_OF_ELEMENTS +1 reset to zero and print 0 on next loop
    count = 0;
  }
 //print to the matrix 
  if (P.displayAnimate())
  {
     P.displayReset();
  }
     
}

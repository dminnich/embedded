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

const int BUTTON = 2;
int count = 0;
byte ButtonState; //pushed or no?
byte lastState = LOW;
//stupid hack to rollover the count back to zero
bool fl = true;

//char count of longest message plus one. or some high number if you have plenty of memory. 
#define  BUF_SIZE  25
char curMessage[BUF_SIZE] = { "" }; //holding space
//number of messages
const int NUM_OF_ELEMENTS = 4;
//array of messages
char messages [NUM_OF_ELEMENTS] [BUF_SIZE] = {
  { "Whoop Whoop" }, 
  { "Fam-a-lee" },
  { "Gary Way" },
  { "FTFOMF" }
};


void setup() {
  Serial.begin(57600);
  pinMode(BUTTON, INPUT);
  P.begin(); // the display
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
  delay(5000); //time to debug
}

void loop() {
ButtonState = digitalRead(BUTTON);
  if (fl && count == 0){
      Serial.println (count); //debug
      Serial.println (messages [count]); //debug
      strcpy(curMessage, messages [count]);
      fl = false;
  }
  if(ButtonState && ButtonState != lastState) { //if button pressed increase counter
    count++;
    delay(500); //try and stop some bouncing
    if (count < NUM_OF_ELEMENTS) {
      Serial.println (count); //debug
      Serial.println (messages [count]); //debug
      strcpy(curMessage, messages [count]);
    }
    else { //when NUM_OF_ELEMENTS +1 reset to zero and print 0 on next loop
      count = 0;
      fl = true;
    }
 }
  //print to the matrix 
  if (P.displayAnimate())
  {
     P.displayReset();
  }
     
}

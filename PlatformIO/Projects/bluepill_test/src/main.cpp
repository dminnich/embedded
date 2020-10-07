/*
//pinout http://land-boards.com/blwiki/index.php?title=STM32
//pio https://docs.platformio.org/en/latest/boards/ststm32/genericSTM32F103C8.html
//arduino + stm32 basics https://www.onetransistor.eu/2020/01/stm32-bluepill-arduino-support.html
/https://www.youtube.com/watch?v=Myon8H111PQ
/https://www.youtube.com/watch?v=cmHQxd_qGl8


*/
#include <Arduino.h>

const uint8_t button = PB4; 
const uint8_t led = PB7; 
uint8_t currentbstate = LOW;
uint8_t lastbstate = LOW;
uint8_t ledstate = LOW;


void setup() {
pinMode(button,INPUT_PULLUP);
pinMode(led, OUTPUT);
}

void loop() {
  lastbstate = currentbstate;
  currentbstate = digitalRead(button);
  if(lastbstate == HIGH && currentbstate == LOW){
    ledstate =!ledstate;
    digitalWrite(led, ledstate);
  }
 delay(100);
}
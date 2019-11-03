/*

Real time clock with matrix display.  

BOOTLOADER, ETC: 
https://www.arduino.cc/en/Tutorial/ArduinoToBreadboard

sensors
https://www.instructables.com/id/Simple-Guide-to-Setting-Time-on-a-DS3231DS3107DS13/
https://howtomechatronics.com/tutorials/arduino/8x8-led-matrix-max7219-tutorial-scrolling-text-android-control-via-bluetooth/


Matrix max7219 4 block
CLK = D13/sck
CS = D10/ss
DIN = D11/mosi
gnd = gnd
vcc = vcc

RTC ds3231
32k = nothing
sqw = nothing
scl = A5/scl
sda = A4/sda
vcc = vcc
gnd = gnd

Nano
D13 = Matrix CLK
3v3 = nothing
RET = nothing
A0 = nothing
A1 = nothing
A2 = nothing
A3 = nothing
A4 = RTC SDA
A5 = RTC SCL
A6 = nothing
A7 = nothing
5V = matrix vcc, rtc vcc
RST = nothing
GND = matrix gnd, rtc gnd
VIN = nothing

TXT = nothing
RXD = nothing
RST = nothing
GND = nothing
D2 = nothing
D3 = nothing
D4 = nothing
D5 = nothing
D6 = nothing
D7 = nothing
D8 = nothing
D9 = nothing
D10 = matrix CS
D11 = matrix DIN
D12 = nothing

 */


//matrix display
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "Font_Data.h"
#define HARDWARE_TYPE MD_MAX72XX::ICSTATION_HW
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10
// Hardware SPI connection
MD_MAX72XX P = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);



//rtc
#include <DS3231.h>
DS3231  rtc(SDA, SCL);
int myhour;
int mymin;
String fulltime;
String phour;
String pmin;
String Ppmin;
String Pphour;
String suffix;
Time  t;

//vars
//how bright to be at night 0-15 0 be dimmest
const int nightbright = 0;
//how bright to be in the day 0-15 0 be dimmest
const int daybright = 4;
//dim after this hour in 24hour time
const int dimafter = 20; //9pm to 11:59pm
//dim before this hour in 24hour time
const int dimbefore = 9; //12am to 8:59am



void setup() {
  //matrix display
  P.begin();
  P.setFont(numeric7Seg);
  P.control(MD_MAX72XX::INTENSITY, nightbright);
  //rtc
  rtc.begin();
  Serial.begin(9600);
  // The following lines can be uncommented to set the date and time
  //Uncomment. Upload.  Comment. Upload.
  //rtc.setDOW(SUNDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(13, 05, 00);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(03, 11, 2019);   // dom, month, year
}

void dim(){
  if (t.hour>dimafter || t.hour<dimbefore){
    P.control(MD_MAX72XX::INTENSITY, nightbright);
  }
  else{
    P.control(MD_MAX72XX::INTENSITY, daybright);
  }
}

void gettime(){
  t = rtc.getTime();
}

void prettydate(){
//store previous reading of values in P variables for deciding when to update the screen
Ppmin = pmin;
Pphour = phour;
  //hours
  //PM
  if (t.hour > 12) {
    myhour = t.hour - 12;
      if (String(myhour).length() == 1) {
      phour = '0' + String(myhour);
    }
    else{
      phour = myhour;
    }
    suffix = "PM";
  }

  //noon
  else if (t.hour == 12){
    phour = t.hour;
    suffix = "PM";
  }
  
  //midnight
  else if (t.hour == 00){
    myhour = 12;
    phour = myhour;
    suffix = "AM";
  }

  //AM
  else {
    myhour = t.hour;  
    if (String(myhour).length() == 1) {
      phour = '0' + String(myhour);
    }
    else{
      phour = myhour;
    }
    suffix = "AM";
  }

//mins
    if (String(t.min).length() == 1) {
      pmin = '0' + String(t.min);
    }
    else{
      pmin = t.min;
    }

fulltime = String(phour) + ':' + String(pmin);
}

//directly from md_max72xx_printtext example
void printText(uint8_t modStart, uint8_t modEnd, char *pMsg)
// Print the text string to the LED matrix modules specified.
// Message area is padded with blank columns after printing.
{
  uint8_t   state = 0;
  uint8_t   curLen;
  uint16_t  showLen;
  uint8_t   cBuf[8];
  int16_t   col = ((modEnd + 1) * COL_SIZE) - 1;

  P.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  do     // finite state machine to print the characters in the space available
  {
    switch(state)
    {
      case 0: // Load the next character from the font table
        // if we reached end of message, reset the message pointer
        if (*pMsg == '\0')
        {
          showLen = col - (modEnd * COL_SIZE);  // padding characters
          state = 2;
          break;
        }

        // retrieve the next character form the font file
        showLen = P.getChar(*pMsg++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
        curLen = 0;
        state++;
        // !! deliberately fall through to next state to start displaying

      case 1: // display the next part of the character
        P.setColumn(col--, cBuf[curLen++]);

        // done with font character, now display the space between chars
        if (curLen == showLen)
        {
          showLen = 1;
          state = 2;
        }
        break;

      case 2: // initialize state for displaying empty columns
        curLen = 0;
        state++;
        // fall through

      case 3:  // display inter-character spacing or end of message padding (blank columns)
        P.setColumn(col--, 0);
        curLen++;
        if (curLen == showLen)
          state = 0;
        break;

      default:
        col = -1;   // this definitely ends the do loop
    }
  } while (col >= (modStart * COL_SIZE));

  P.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}


void mprinttime(){
  //string length + newline ending
  char cfulltime[6];
  fulltime.toCharArray(cfulltime,6);
  printText(0, MAX_DEVICES-1, cfulltime);  
}

void sprinttime() {
  Serial.println(fulltime);
}

void loop() {
  gettime();
  prettydate();
  
  //on hour change dim if needed.
  if (Pphour != phour){
    dim();  
  }
  
  //on minute change print it to the places
  if (Ppmin != pmin) {
    mprinttime();
    sprinttime();    
  }
}
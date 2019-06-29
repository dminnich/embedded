/*

Real time clock with matrix display.  

BOOTLOADER, ETC: 
https://www.arduino.cc/en/Tutorial/ArduinoToBreadboard

sensors
https://www.instructables.com/id/Simple-Guide-to-Setting-Time-on-a-DS3231DS3107DS13/
https://howtomechatronics.com/tutorials/arduino/8x8-led-matrix-max7219-tutorial-scrolling-text-android-control-via-bluetooth/


Matrix max7219 4 block
CLK = 19/13/sck
CS = 16/10/ss
DIN = 17/11/mosi
gnd = gnd
vcc = vcc

RTC ds3231
32k = nothing
sqw = nothing
scl = 28/19/a5/scl
sda = 27/18/a4/sda
vcc = vcc
gnd = gnd

cp2102 serial programmer
5v = vcc
gnd = gnd
txd = pin 2/0
rxi = 3/1
dtr = positive side of 10uf cap > negative side of 10uf cap > 1/22
3v = nothing

led for debugging
long leg = reistor + 14/8
short leg = gnd



atmega328p:
*may have issues using
1/22* = negative side of 10uf cap > positive side of 10uf cap > usb to serial debug and program DTR
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
24/15/a1 = nothing
25/16/a2 = nothing
26/17/a3 = nothing
27/18/a4/sda = ds3231 sda
28/19/a5/scl = ds3231 scl

 */

//matrix display
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "Font_Data.h"
#define HARDWARE_TYPE MD_MAX72XX::ICSTATION_HW
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10
// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);


//rtc
#include <DS3231.h>
DS3231  rtc(SDA, SCL);
int myhour;
int mymin;
String fulltime;
String phour;
String pmin;
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
//how often in sec to update the display
const int pause = 10;
const int pausems = pause * 1000;


void setup() {
  //matrix display
  P.begin();
  P.setFont(numeric7Seg);
  P.setTextAlignment(PA_CENTER);
  //rtc
  rtc.begin();
  // The following lines can be uncommented to set the date and time
  //Uncomment. Upload.  Comment. Upload.
  //rtc.setDOW(FRIDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(20, 12, 00);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(28, 06, 2019);   // dom, month, year
}

void printtime(){
  t = rtc.getTime();

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


  P.displayClear();
  P.print(fulltime);
}

void loop() {
P.print("loop");
  if (t.hour>dimafter || t.hour<dimbefore){
    P.setIntensity(nightbright);
  }
  else {
    P.setIntensity(daybright);
  }
  printtime();
  delay(pausems);
}

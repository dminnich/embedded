/*
  Time based relay.  Like those Christmas light or vacataion "somebody is really home" devices.

  BOOTLOADER, ETC:
  https://www.arduino.cc/en/Tutorial/ArduinoToBreadboard

  parts
  https://www.instructables.com/id/Simple-Guide-to-Setting-Time-on-a-DS3231DS3107DS13/
  https://howtomechatronics.com/tutorials/arduino/control-high-voltage-devices-arduino-relay-tutorial/
  https://lastminuteengineers.com/nokia-5110-lcd-arduino-tutorial/

  #rtc
  5v tolerant
  https://tronixlabs.com.au/breakout-boards/real-time-clock/precision-ds3231-real-time-clock-module-australia/
  https://www.digikey.com/product-detail/en/maxim-integrated/DS3231S-T-R/DS3231S-T-RCT-ND/4895454
  wiring
  https://lastminuteengineers.com/wp-content/uploads/2018/12/Wiring-DS3231-RTC-module-with-Arduino.png


  #lcd
  digital pins are 5v tolerant.  need 3.3v on vcc of the screen.
  https://forum.arduino.cc/index.php?topic=434650.0
  https://www.brainy-bits.com/how-to-use-the-nokia-5110/
  wiring
  https://circuitdigest.com/sites/default/files/circuitdiagram_mic/setup-for-Interfacing-Nokia5110-Graphical-LCD-with-Arduino.png
  voltage dividers can't provide current. level shifters are for logic only and need refrence voltages on both sides. need a relay or a buck convertor.
  http://www.learningaboutelectronics.com/Articles/How-to-divide-voltage-in-half-with-resistors.php
  https://hackingmajenkoblog.wordpress.com/2016/08/30/why-you-cant-use-resistors-as-a-voltage-regulator/
  https://core-electronics.com.au/tutorials/how-to-use-logic-level-shifters-converters.html
  https://electronics.stackexchange.com/questions/42727/what-is-the-difference-between-level-shifter-voltage-regulator-and-dc-dc-conver
  lots of ebay and ads say 5v capable on vcc.  i've had one running like that for a while.  so gonna risk it.
  https://www.ebay.com/itm/84x48-Nokia-LCD-Module-Blue-Backlight-Adapter-PCB-Nokia-5110-LCD-For-Arduino-GG/293132857342?epid=2120138486&_trkparms=ispr%3D1&hash=item4440146ffe:g:JvIAAOSwZTBbiQ-d&enc=AQAEAAAB0BPxNw%2BVj6nta7CKEs3N0qXC2XgDlru33du4Y%2BrZhKb6sFV%2B%2BN1LVFXkh%2F2k2LAvuBPgqPybuzIUFcea42DIqt99bBCIy1Lzw%2B%2FUeqCsNtrbtgS3NClCXehRdbsj4GnwrBlVaos9RhnLh0D0Jk82oCc03soVsJvPloM6XYxO5tVIO7%2F4AqEnnary6sN1Df3QVNeXZtjyrAgW041m%2B6oXF%2BaEy76bLui2gK9vR3yikFofCoWveyM563n9iBdDH8p6afSYld9fbWixXD3VA2kQUzdGDDizWIY%2FpirB6mpniIwbarztvlaAiUP7lur5NvomFm2Wm5KjQX1%2FdAI%2FXnwO0neJFZW27MHDAfm4gq%2Ft7dKG%2B%2FV3LavS5CTf%2BJSlLr8e6WF8Ddf7ceCXSzSiSo9re%2FvsrtUfQcaXs6Sk%2FIPrp3UG%2F88z1wMCfqZd4lGWp1IvfolXsJ18LhoblDYL8vfDXEmFX0iuRcXUebkSqtm563DsYkjyUK4KbflT5WwyLHtVdigt9ig4LQvsdcIGOJKRdoYu%2F%2FUU5SLdceaiZ6CXByHrn836t%2BuTSZb%2Bk6w2T7lmzj%2BnG7LmQ6U4dwTxLYqlz6kvZwoKW48lQC8QpotIF%2BjN&checksum=29313285734285bb7ec312e74b24aea0183aa385e5fa
  https://www.aliexpress.com/item/32614334972.html?spm=2114.search0204.3.9.278c5d07YqZPW8&ws_ab_test=searchweb0_0%2Csearchweb201602_9_10065_10130_10068_10547_319_10546_317_10548_10545_10696_10084_453_454_10083_10618_10307_537_536_10059_10884_10887_321_322_10103%2Csearchweb201603_53%2CppcSwitch_0&algo_expid=1b9eeb30-523d-4ff8-adc6-e2fc45a317a6-1&algo_pvid=1b9eeb30-523d-4ff8-adc6-e2fc45a317a6


  not running backlight.


  #relay
  http://osoyoo.com/wp-content/uploads/2017/08/1channel-relay-fritzing-1.jpg



  CP2102
  DTR -> positive side of 10uf capacitor  > negative side of capcitor > 1/22/reset
  RX  -> 3/1/TX
  TX  -> 2/0/RX
  gnd breadboard gnd
  5v breadboard vcc

  led:
  short leg = gnd
  long leg = 330 restistor > 13/7/ain1

  RTC ds3231
  32k = nothing
  sqw = nothing
  scl = 28/19/a5/scl
  sda = 27/18/a4/sda
  vcc = vcc
  gnd = gnd

  LCD:
  rst = 18/12/miso
  ce = 17/11/mosi
  dc = 16/10/ss
  din = 15/9
  clk = 14/8
  vcc = vcc
  bl = gnd
  gnd = gnd

  relay:
  vcc = vcc
  gnd = gnd
  in = 26/17/a3

  atmega328p:
  may have issues using
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
  13/7/ain1 = resistor > led long leg > gnd
  14/8 = clk on LCD

  15/9 = din on LCD
  16/10/ss = dc on LCD
  17/11/mosi = ce on LCD
  18/12/miso = rst on LCD
  19/13/sck = nothing
  20 = vcc
  21 = aref/nothing
  22 = gnd
  23/14/a0 = nothing
  24/15/a1 = nothing
  25/16/a2 = nothing
  26/17/a3 = relay in
  27/18/a4/sda = sda on RTC
  28/19/a5/scl = scl on RTC



*/
//led
#define LED_BUILTIN 7
const bool debug = true;

//rtc
#include <DS3231.h>
DS3231  rtc(SDA, SCL);
//where calcs happen
int myhour; 
int mymin;
//full string
String fulltime;
String suffix;
//adding 0 padding
String phour;
String pmin;
//used to track if something happened since last read and if LCD needs to be updated
String Ppmin;
String Pphour;
Time  t;

//relay
const uint8_t relaypin = 17;

//LCD setup 
// installed library from website. do not allow updates from IDE.
// web: http://www.RinkyDinkElectronics.com/
// It is assumed that the LCD module is connected to
// the following pins using a levelshifter to get the
// correct voltage to the module.
//      SCK  - Pin 8 //clk 
//      MOSI - Pin 9 // din
//      DC   - Pin 10 // dc 
//      RST  - Pin 12 // rst 
//      CS   - Pin 11 //ce 
//
#include <LCD5110_Graph.h>
LCD5110 myGLCD(8,9,10,12,11);
extern uint8_t SmallFont[];


//vars
//relay on after this hour in 24hour time
//1 BEFORE when you want it on. 17 == 5:59 == 6:00PM
const int onafter = 17; //6pm to 10pm
//relay off at this hour in 24hour time
const int offat = 22; //6pm to 10pm
//const int offat = 19; //6pm to 10pm
//for display purposes we want the real time on the after
const int onafterC = onafter + 1;
//status line prefixes
String tL = "Time: ";
String onL = "On: ";
String offL = "Off: ";

void setup() {
  //debugging
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  
  //rtc
  rtc.begin();
  // The following lines can be uncommented to set the date and time
  //Uncomment. Upload.  Comment. Upload.
 // rtc.setDOW(SUNDAY);     // Set Day-of-Week to SUNDAY
 // rtc.setTime(15, 31, 30);     // Set the time to 12:00:00 (24hr format)
 // rtc.setDate(21, 07, 2019);   // dom, month, year
  
  //relay
  //LOW = ON
  pinMode(relaypin, OUTPUT);
  digitalWrite(relaypin, HIGH);

  //setup lcd
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);

  //create human readable on/off lines
  //call the functions that the RTC usually calls which convert the hours to 12 hour with suffix.
  //we only need to do this once since the on/off times are static.  
  prettyhour(offat);
  offL = offL + String(phour) + ":00" + " " + String(suffix); 
  prettyhour(onafterC);
  onL = onL + String(phour) + ":00" + " " + String(suffix); 
}


void gettime(){
  t = rtc.getTime();
}

void prettyhour(int hour){
//used to track if relay work needs to happen after an hour changes.
Pphour = phour;
//hours
  //PM
  if (hour > 12) {
    myhour = hour - 12;
      if (String(myhour).length() == 1) {
      phour = '0' + String(myhour);
    }
    else{
      phour = myhour;
    }
    suffix = "PM";
  }

  //noon
  else if (hour == 12){
    phour = hour;
    suffix = "PM";
  }
  
  //midnight
  else if (hour == 00){
    myhour = 12;
    phour = myhour;
    suffix = "AM";
  }

  //AM
  else {
    myhour = hour;  
    if (String(myhour).length() == 1) {
      phour = '0' + String(myhour);
    }
    else{
      phour = myhour;
    }
    suffix = "AM";
  }  
}


void prettyminute(int min){
//used to determine if the LCD screen needs an update.  only does that once a minute
Ppmin = pmin;
//mins
    if (String(min).length() == 1) {
      pmin = '0' + String(min);
    }
    else{
      pmin = min;
    }
}

void prettydate(){
prettyhour(t.hour);
prettyminute(t.min);
fulltime = tL + String(phour) + ":" + String(pmin) + " " + String(suffix);
}


void relaycheck(){
  //on hour changes
  if (debug) { Serial.println("checking relay"); }
  if (t.hour>onafter && t.hour<offat){
    //turn on relay if it isn't on already
    if (digitalRead(relaypin) != LOW){
    if (debug) { Serial.println("turning relay on"); }
      digitalWrite(relaypin, LOW); 
    }
  }
  //not during specified time.  turn off relay.
  else{
    if (debug) { Serial.println("turning relay off"); }
    digitalWrite(relaypin, HIGH);
  }
}

void lcdprinttime() {
  //once a minute
  if (debug) { Serial.println("updating time on lcd"); }
  myGLCD.clrScr();
  myGLCD.print(fulltime, LEFT, 0);
  myGLCD.print(onL, LEFT, 8);
  myGLCD.print(offL, LEFT, 16);
  myGLCD.update();
}

void sprinttime() {
  //once a minute
  if (debug) { Serial.println("updating time on serial"); }
  Serial.println(fulltime);
  Serial.println(onL);
  Serial.println(offL);
}

void loop() {
  gettime();
  prettydate();

  //on hour change change the relay if needed.
  if (Pphour != phour){ relaycheck(); }
  
  //on minute change print it to the places
  if (Ppmin != pmin) {
    lcdprinttime();
    if (debug) { sprinttime(); }
  }
}

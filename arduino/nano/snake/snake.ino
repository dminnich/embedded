//temp sensor setup.  
//installed library from library manager
#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#include <DHT.h>
#include <math.h> //for rounding numbers
#define DHTPINT            7  //top sensor
#define DHTPINB            6 //botton sensor
#define DHTTYPE           DHT11     // DHT 11 model
DHT_Unified dhtT(DHTPINT, DHTTYPE);  //top sensor
DHT_Unified dhtB(DHTPINB, DHTTYPE); //bottom sensor


//LCD setup 
// installed library from website. do not allow updates from IDE.
// web: http://www.RinkyDinkElectronics.com/
// It is assumed that the LCD module is connected to
// the following pins using a levelshifter to get the
// correct voltage to the module.
//      SCK  - Pin 8
//      MOSI - Pin 9
//      DC   - Pin 10
//      RST  - Pin 12
//      CS   - Pin 11
//
#include <LCD5110_Graph.h>
LCD5110 myGLCD(8,9,10,12,11);
extern uint8_t SmallFont[];

//Time sensor setup
//installed library from website. do not allow updates from IDE.
// DS3231_Serial_Easy
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
// To use the hardware I2C (TWI) interface of the Arduino you must connect
// the pins as follows:
//
// Arduino Uno/2009:
// ----------------------
// DS3231:  SDA pin   -> Arduino Analog 4 or the dedicated SDA pin
//          SCL pin   -> Arduino Analog 5 or the dedicated SCL pin
//
// The internal pull-up resistors will be activated when using the 
// hardware I2C interfaces.
//
// You can connect the DS3231 to any available pin but if you use any
// other than what is described above the library will fall back to
// a software-based, TWI-like protocol which will require exclusive access 
// to the pins used, and you will also have to use appropriate, external
// pull-up resistors on the data and clock signals.
#include <DS3231.h>
#include <Wire.h>
DS3231  rtc(SDA, SCL);
Time  t;

// General configs
//Bottom light status line and its vars set to unknown at boot.
String lsl = "? ?"; //last status line.  HL L indicator
String hls; //HL status
String ls; //L status

//time vars
int myhour;
int mymin;
String suffix; //AM or PM
String fulltime; //stringified version of what i want to display
String phour; //padded with 0
String pmin; //padded with 0

//relay vars
const int hlrelay = 4; //in2 HL relay
const int lrelay = 5; //in1 L relay

//Poll and update screen with temp and time every polldelay s. 
// Do this pollcount times before doing anything with the lights to prevent strobing.
// So pd=3 and pc=10 means update temp every 3sec and mess with lights every 30
int starter = 0;
int pollcount = 300; //15mins
int polldelay = 3;
int polldelayms = polldelay * 1000;

//Switches.  fahrenheit and 24 hour.  
//HL on between toocold and toohot and when less than toocold
//L on between onhour and offhour 
int toocold = 77;
int toohot = 83;
int onhour = 8;
int offhour = 22;


void setup()
{
  Serial.begin(9600);
  
  //setup lcd
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  
  //setup temp sensors
  dhtT.begin(); //top sensor
  dhtB.begin(); //bottom sensor
  
  //setup relaypins
  //logic is backwards.  LOW = ON. Start off
  pinMode(hlrelay, OUTPUT); //heatlamp
  pinMode(lrelay, OUTPUT); //lamp
  Serial.println("cutting off relays at boot");
  digitalWrite(hlrelay, HIGH);
  digitalWrite(lrelay, HIGH);

  //setup time
  rtc.begin();
  // The following lines can be uncommented to set the date and time
  //Uncomment. Upload.  Comment. Upload.
  //rtc.setDOW(SATURDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(17, 36, 00);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(29, 9, 2018);   // dom, month, year
}

void loop()
{
 //Normal polling and screen updating
  while (starter < pollcount) {
    probeandprint();
    starter++;
    Serial.println(starter);
  }

  
  //Adjust the lights and light status line after pollcount * polldelay to prevent srobing
  //get temp
  sensors_event_t eventB;  //bottom sensor info
  dhtB.temperature().getEvent(&eventB);
  float fB = (eventB.temperature * 1.8) + 32; //to F degress
  int fBr = round(fB); //rounded

  //get time
  t = rtc.getTime();

  //get current on/off status of relays. logic is reverse.  put LOW to turn on
  int chls = digitalRead(hlrelay); //current hl status
  int cls = digitalRead(lrelay); //current l status
  
  
  
  if ((fBr > toocold) && (fBr < toohot)){
    hls = "HL "; //heat lamp status
    if (chls == HIGH){ //if off, turn on.
      Serial.println("Turning on HL middle range");
      digitalWrite(hlrelay, LOW);
      delay(5000); //don't fire both relays at once 
    }
  }
  else if (fBr < toocold){
    hls = "HL "; //heat lamp status
    if (chls == HIGH){ //if off, turn on.
      Serial.println("Turning on HL low range");
      digitalWrite(hlrelay, LOW);
      delay(5000); //don't fire both relays at once 
    }
  }
  else {
    hls = " "; //heat lamp status
    if (chls == LOW){ //if on, turn off.
      Serial.println("Turning off HL");
      digitalWrite(hlrelay, HIGH);
      delay(5000); //don't fire both relays at once 
    }
  }
  

  if ((t.hour >= onhour) && (t.hour <= offhour)){
    ls = "L "; //lamp status
   if (cls == HIGH){ //if off, turn on.
      Serial.println("Turning on L inside of time range");
      digitalWrite(lrelay, LOW);
    }
  }
  else {
    ls = " "; //lamp status
    if (cls == LOW){ //if on, turn off.
      Serial.println("Turning off L outside of time range");
      digitalWrite(lrelay, HIGH);
    }
  }

  lsl = hls + ls; //last status line = hl and l status
  probeandprint();
  starter = 0; //do it all again.
}

//do most of the work
void probeandprint() {
    //top temp and humidity 
    sensors_event_t eventT;
    dhtT.temperature().getEvent(&eventT);
    float f = (eventT.temperature * 1.8) + 32; //degrees F
    int fr = round(f); //rounded
    String tsl = "T: " + String(fr) + "F "; //top sensor status line
    dhtT.humidity().getEvent(&eventT);
    float h = eventT.relative_humidity; //humidity
    int hr = round(h); //rounded
    tsl += String(hr); //add humidity to top status line

    //bottom temp and humidity 
    sensors_event_t eventB;
    dhtB.temperature().getEvent(&eventB);
    float fB = (eventB.temperature * 1.8) + 32; //degrees F
    int fBr = round(fB); //rounded
    String bsl = "B: " + String(fBr) + "F "; //bottom sensor status line
    dhtB.humidity().getEvent(&eventB);
    float hB = eventB.relative_humidity; //humidity
    int hBr = round(hB); //rounded
    bsl += String(hBr); //add humidity to bottom status line
    

    t = rtc.getTime();

    //hours
    //PM
    if (t.hour > 12) {
      myhour = t.hour - 12;
      if (String(myhour).length() == 1) {
        phour = '0' + String(myhour); //pad with a zero
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
        phour = '0' + String(myhour); //pad with a zero
      }
      else{
        phour = myhour;
      }
      suffix = "AM";
    }

    //mins
    if (String(t.min).length() == 1) {
      pmin = '0' + String(t.min); //pad with a zero
    }
    else{
      pmin = t.min;
    }

    fulltime = String(phour) + ':' + String(pmin) + ' ' + suffix; //stringified with padding and suffix
    

    //print the results
    myGLCD.clrScr();
    myGLCD.print(fulltime, LEFT, 0);
    Serial.println(fulltime); //time line
    myGLCD.print(tsl, LEFT, 8); //top sensor status line
    Serial.println(tsl);
    myGLCD.print(bsl, LEFT, 16); //bottom sensor status line
    Serial.println(bsl);
    myGLCD.print(lsl, LEFT, 40);  //last status line.  HL and L indicators
    Serial.println(lsl);
    myGLCD.update();
    delay(polldelayms); //wait this long before probing again.  
    Serial.println(""); //seperator for easier debugging
}


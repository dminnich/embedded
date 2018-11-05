// DS3231_Serial_Easy
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// A quick demo of how to use my DS3231-library to 
// quickly send time and date information over a serial link
//
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
//

#include <DS3231.h>
#include <math.h> //for rounding numbers

/***************************************************
  This is a library for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
  The 1.8" TFT shield
  ----> https://www.adafruit.com/product/802
  The 1.44" TFT breakout
  ----> https://www.adafruit.com/product/2088
  as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618
 
  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
 
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
  ****************************************************/
 
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
 
 
// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    8  // you can also connect this to the Arduino reset
                       // in which case, set this #define pin to -1!
#define TFT_DC     9

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
 
// For 1.44" and 1.8" TFT with ST7735 use
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
//#define TFT_SCLK 13   // set these to be whatever pins you like!
//#define TFT_MOSI 11   // set these to be whatever pins you like!
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
// create vars
float tempf;
float tempc;
int myhour;
int mymin;
int mysec;
String suffix;
String fulltime; 
String phour;
String pmin;
String psec;
Time  t;

void setup()
{
  // Setup Serial connection
  Serial.begin(115200);
  
  // Initialize the rtc object
  rtc.begin();
 // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setRotation(3);
  tft.setTextWrap(false);

  // The following lines can be uncommented to set the date and time
  //Uncomment. Upload.  Comment. Upload.
  //rtc.setDOW(MONDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(18, 21, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(5, 11, 2018);   // dom, month, year
}

void loop()
{
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

//secs
    if (String(t.sec).length() == 1) {
      psec = '0' + String(t.sec);
    }
    else{
      psec = t.sec;
    }

  
  // Send date
  Serial.print("Time: ");
    //fulltime = String(phour) + ':' + String(pmin) + ':' + String(psec) + ' ' + suffix;
  fulltime = String(phour) + ':' + String(pmin) + ' ' + suffix;
  Serial.println(fulltime);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  tft.setCursor(0, 0);
  tft.print(fulltime);
  Serial.print("Date: ");
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.setCursor(0, 35);
  tft.print(rtc.getDateStr(FORMAT_LONG,FORMAT_MIDDLEENDIAN,'/'));
  Serial.println(rtc.getDateStr(FORMAT_LONG,FORMAT_MIDDLEENDIAN,'/'));
  Serial.print("Temp: ");
  tempc = rtc.getTemp();
  tempf = (tempc*1.8)+32;
  int tempfr = round(tempf);
  Serial.println(tempfr);
  tft.setCursor(0, 70);
  tft.print(String(tempfr) + " F");
  Serial.println(tempf);
  Serial.println ("");
  
  // Wait 15 seconds before repeating :)
  delay (15000);
}

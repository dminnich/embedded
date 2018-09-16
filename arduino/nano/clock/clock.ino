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
#include <LiquidCrystal.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
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
LiquidCrystal lcd(8,9,4,5,6,7);


void setup()
{
  // Setup Serial connection
  Serial.begin(115200);
  // Uncomment the next line if you are using an Arduino Leonardo
  //while (!Serial) {}
  
  // Initialize the rtc object
  rtc.begin();
  lcd.begin(16,2);
  // The following lines can be uncommented to set the date and time
  //Uncomment. Upload.  Comment. Upload.
  //rtc.setDOW(SUNDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(16, 58, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(5, 8, 2018);   // dom, month, year
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
  lcd.setCursor(0,0);
  lcd.print("  ");
  fulltime = String(phour) + ':' + String(pmin) + ':' + String(psec) + ' ' + suffix;
  Serial.println(fulltime);
  lcd.print(fulltime);
  Serial.print("Date: ");
  lcd.setCursor(0,1);
  lcd.print("  ");
  lcd.print(rtc.getDateStr(FORMAT_LONG,FORMAT_MIDDLEENDIAN,'/'));
  Serial.println(rtc.getDateStr(FORMAT_LONG,FORMAT_MIDDLEENDIAN,'/'));
  Serial.print("Temp: ");
  tempc = rtc.getTemp();
  tempf = (tempc*1.80000)+32.00;
  Serial.println(tempf);
  Serial.println ("");
  
  // Wait one second before repeating :)
  delay (1000);
}

/*

For snake care.
Readings from 2 temperature sensors and NTP lookups are shown on an OLED display. 
If the "hot side" temperature is outside of a range it fires a relay to turn on a heat lamp.
During specific hours a different relay is turned on to activate a plant grow lamp.
https://hackster.imgix.net/uploads/attachments/944050/node-mcu_nRId0HmElJ.jpg?auto=compress%2Cformat&w=1280&h=960&fit=max
https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
https://community.blynk.cc/uploads/default/original/2X/3/32941e2c2225641862dca63c203fbc19e287b871.jpg
amzn.com/B010N1SPRK
amzn.com/B01DKC2GQ0
amzn.com/B07F3KY8NF
amzn.com/B01M0E6SQM


NODEMCU V3:
3.3v - vcc
gnd - gnd
D10/TX/GPIO1 - nothing
D9/RX/GPIO3 - nothing
D8/GPIO15/RTS0/HCS - nothing
D7/GPIO13/CTS0/HMOSI - nothing
D6/GPIO12/HMISO - cold side temerature sensor
D5/GPIO14/HSCLK - hot side temperature sensor
gnd - gnd
3.3v - vcc
D4/GPIO2/TXD1 - Relay 1 in
D3/GPIO0/FLASH - Relay 2 in
D2/GPIO4/SDA - SDA on screen
D1/GPIO5/SCL - SCL on screen
DO/GPIO16/Wake - nothing


VIN - nothing
gnd - gnd
RST - nothing
EN/CH-PD - nothing
3.3v- vcc
gnd - gnd
CLK/GPIO6/ - nothing
SD0/GPI07/MISO/SDD0 - nothing
CMD/GPIO11/CS/SDCMD - nothing
SD1/MOSI/SDD1/GPIO8 - nothing
D11/SD2/GPIO9/SDD2 - nothing
D12/SD3/GPIO10/SDD3 - nothing
rsv - nothing
rsv - nothing
A0/ADC0 - nothing


SSD1306 screen
gnd - gnd
vcc - vcc
sdc - D1/GPIO5/SCL
sda - D2/GPIO4/SDA

DHT11 cold side
vcc - vcc
data - D6/GPIO12/HMISO
gnd - gnd

DHT11 hot side
vcc - vcc
data - D5/GPIO14/HSCLK
gnd - gnd

relay 1 - grow light
vcc - vcc
in - D4/GPIO2/TXD1
gnd - gnd

relay 2 - heat lamp
vcc - vcc
in - D3/GPIO0/FLASH
gnd - gnd


*/

#include <Arduino.h>

//declare some vars for storage
String timeline = "", hotline = "", coldline = "", relayline = "";
int myhour, mymin, mytemp;

//config
//how many seconds to wait between main loop executions.  temp sensors may struggle to be read more frequently than every 250ms. 
const uint8_t looppauses = 3;
//how often in seconds are we willing to toggle the relays?  you don't want to strobe them.  5 minutes here
int relaypauses = 300;
//grow light runs roughly between the hours specified below.  Can be + or - relaypauses
//grow light on hour in 24h format.
const uint8_t growonh = 8;
//grow light off hour in 24h format.
const uint8_t growoffh = 21;
//heat lamp runs roughly at any temperature below offtemp.  Can be up to relaypauses before an adjustment happens
const uint8_t offtemp = 86;
//time display format us (12 hour) or world (24 hour)
const String timeformat = "us";
//temp display format F (fahrenheit) or C (celsius)
const String tempformat = "F";
//your timezone location in Olson format https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
const String olsontz = "America/New_York";
//what NTP server to query https://www.ntppool.org/en/
const String ntpserver = "192.168.2.1";
//how frequently to ask the NTP servers for the time. 
//nodemcu keeps good time so every 6 hours is good enough. don't hammer public servers
const uint16_t ntppollsec = 21600;
//put your wifi credentials in another file in include and .gitignore it
//format of
//#define WIFI_SSID "myssid"
//#define WIFI_PASSWD "mypass"
#include "credentials.h"
char ssid[] = WIFI_SSID;
char password[] = WIFI_PASSWD;
//enable debug settings and output
const bool debug = false;

//temp sensors "DHT sensor library" 
//https://github.com/adafruit/DHT-sensor-library/blob/master/examples/DHTtester/DHTtester.ino
#include <DHT.h>
//gpio numbers
#define COLDTSPIN 12
#define HOTTSPIN 14
#define DHTTYPE DHT11
DHT coldts(COLDTSPIN, DHTTYPE);
DHT hotts(HOTTSPIN, DHTTYPE);

//software timer. http://sofapirate.github.io/Chrono/#heading_toc_j_6
//do relay adjustments roughly every specified interval
#include <Chrono.h>
Chrono relayadjust(Chrono::SECONDS);

//NTP https://github.com/ropg/ezTime
#include <ezTime.h>
#include <ESP8266WiFi.h>

//screen
//https://github.com/adafruit/Adafruit_SSD1306/blob/master/examples/ssd1306_128x64_i2c/ssd1306_128x64_i2c.ino
//https://lastminuteengineers.com/oled-display-esp8266-tutorial/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//relays
const uint8_t r1 = D4; //grow light
const uint8_t r2 = D3; //heat lamp


void setup() {
  //some serial for troubleshooting
  if (debug){
    Serial.begin(9600);
  }
  //relays are outputs the ones I built are active LOW.  Make sure they are turned off at boot.
  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  digitalWrite(r1, HIGH);
  digitalWrite(r2, HIGH);

  //temp sensors
  coldts.begin();
  hotts.begin();
  
  //NTP time
  //connect to wifi
  WiFi.begin(ssid, password);
  //library specific troubleshooting output
  if (debug){
    //setDebug(INFO);
  }
  //NTP server to query
  setServer (ntpserver); 
  //sync with NTP server this often
  setInterval (ntppollsec); 
  //get the time from NTP servers.  wait up to one minute
  waitForSync(60);
  
  //screen
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void getTime() {
  //call the eztime events method that notices if it needs to resync the time or not
  events();
  //create a time object
  Timezone myTZ;
  //eztime converts your olson timezone location into a posix format timezone offset string that it then uses for time calculations.
  //it does this by calling out to a server on the internet.  Since that server could be down, its best to do the conversion once
  //and cache the results.
  if (!myTZ.setCache(0)) {
    myTZ.setLocation(olsontz);
    myTZ.setCache(0);
  }

  //get the current time
  time_t t = myTZ.now();
  //spit into hours and minutes
  myhour = hour(t);
  mymin = minute(t);
  
  if (timeformat == "us"){
    timeline = String(myTZ.dateTime("h:i A"));
  }
  else {
    timeline = String(myTZ.dateTime("H:i"));
  }
}

void getMeasurements() {
  //declare some vars for storage
  float ctempfloat, htempfloat;

  //get temp and humidity measurements from both sensors. 
  if (tempformat == "F"){
    ctempfloat = coldts.readTemperature(true);
    htempfloat = hotts.readTemperature(true);
  }
  else {
    ctempfloat = coldts.readTemperature();
    htempfloat = hotts.readTemperature();
  }
  int chumidityfloat = coldts.readHumidity();
  int hhumidityfloat = hotts.readHumidity();
  //i don't care about decmials
  int ctemp = round(ctempfloat);
  int htemp = round(htempfloat);
  mytemp = htemp;
  int chumidity = round(chumidityfloat);
  int hhumidity = round(hhumidityfloat);
  //i want to print temps and humidity readings
  if (tempformat == "F"){
    hotline = String("Hot: " + String(htemp) + "F " + String(hhumidity));
    coldline = String("Cold: " + String(ctemp) + "F " + String(chumidity));
  }
  else {
    hotline = String("Hot: " + String(htemp) + "C " + String(hhumidity));
    coldline = String("Cold: " + String(ctemp) + "C " + String(chumidity));
  }
}

void adjustRelays() {
  //overide hour and temp for debugging
  if (debug){
    Serial.println("RLEAY");
    //myhour = 14;
    //mytemp = 9;
   }

  //reset status each time we enter the loop
  relayline = "";
  //make sure the grow light is on during our specified times
  if (myhour >= growonh && myhour < growoffh) { 
    if (digitalRead(r1) == HIGH) {
      digitalWrite(r1, LOW); 
    }
    //add GL to our output line to signify the grow light is on
    relayline = String(relayline + "GL ");
  }
  //ensure grow light is off during non-specified hours
  else {
    if (digitalRead(r1) == LOW) {
      digitalWrite(r1, HIGH); 
    }
  }
  //make sure the heat lamp is on if hot side is cooler than our specified temp
  if (mytemp < offtemp) {
    if (digitalRead(r2) == HIGH) {
      digitalWrite(r2, LOW);
    }
    //add HL to our relay status line
    relayline = String(relayline + "HL ");
  }
  //ensure heat lamp is off if hot side is hotter than our specified temp
  else {
    if (digitalRead(r2) == LOW) {
      digitalWrite(r2, HIGH);
    }
  }
}

void adjustScreen() {
  //clear screen. 
  display.clearDisplay();
  //set text size, color, location of lines
  display.setTextSize(1); 
  display.setTextColor(WHITE); 
  display.setCursor(0,0); 
  display.println(timeline); 
  display.setCursor(0,20);
  display.println(hotline);
  display.setCursor(0,30);
  display.println(coldline);
  display.setCursor(0,50); 
  display.println(relayline); 
  //print the lines
  display.display();
}

void loop() {
  //get time
  getTime();
  //get temp, humidity
  getMeasurements();
  //if its been longer than this amount of time, see if the relays need adjusting
  if ( relayadjust.hasPassed(relaypauses) ) { 
    //reset timer
    relayadjust.restart();  
    adjustRelays();
  }
  //print what we got
  if (debug) {
    Serial.println(timeline);
    Serial.println(hotline);
    Serial.println(coldline);
    Serial.println(relayline);
  }
  //write the data to the OLED screen
  adjustScreen();
  //pause a bit to prevent sensor and screen strobing
  delay(looppauses * 1000);
}
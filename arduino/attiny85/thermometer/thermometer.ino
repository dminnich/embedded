/*

Portable temperature gauge with led screen

BOOTLOADER, ETC: 
#https://engineersportal.com/blog/2018/5/19/attiny85-arduino-board-how-to-flash-the-arduino-bootloader-and-run-a-simple-sketch - must use 8mhz clock to support dht
#https://github.com/nickgammon/SendOnlySoftwareSerial - debugging
#https://www.arduinolibraries.info/libraries/ss_oled - screen 
#https://github.com/bitbank2/BitBang_I2C - screen
#https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTlib - temp sensor
#other notes to self: 
 - new library folder with just this stuff or adafruit or other libs may conflict
 - you may have to try to upload a few times and/or disconnect the temp sensor to upload.  
 - cables fall off the screen, sensor and out of breadboard easily.  
 - unhook 13-10 IVR programming pins from Uno when testing the attiny85 code

Screen:
SDA = 5/pb0
SCK = 7/pb2
vcc = vcc
gnd = gnd


thermometer dht11:
gnd = gnd
data = 6/pb1
vcc = vcc

ATTINY:
1/pb5 = nothing
2/pb3 = nothing (led for testing)
3/pb4 = nothing (serial recieve)
4/gnd = gnd on devices and power supply

5/pb0 = sda on screen
6/pb1 = data on dht11
7/pb2 = sck on screen
8/vcc = vcc on devices and power supply


 */ 

// debug
#include <SendOnlySoftwareSerial.h>
SendOnlySoftwareSerial mySerial (4);
// screen
#include <ss_oled.h>
// temp sensor
#include <dht.h>
dht DHT;
#define DHT11_PIN 1


//vars
const int checksec = 5; 
int checkmsec = checksec * 1000;
int led = 3;

void setup() {
 // for debugging use CP210x UART hooked to device via RX and laptop via USB. Switch port in the IDE between IVR for programming and the CP210x for serial monitor.
 mySerial.begin(9600);
 //setup the screen.  resolution, ?, ?, pb0, pb2, ?
  int rc;
  rc = oledInit(OLED_128x32, 0, 0, 0xb0, 0xb2, 400000L); // for ATtiny85, use P0 as SDA and P2 as SCL
 //simple light to show setup complete
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  delay(3000);
  digitalWrite(led, LOW);
}

//convert C to F https://forum.arduino.cc/index.php?topic=557125.0
int fahr(int x) {return ((((((x << 1) * 9) + 5) >> 1) / 5) + 32);}

void loop() {
//read the temp
int chk = DHT.read11(DHT11_PIN);
//error handling of temp sensor
  switch (chk)
  {
    case DHTLIB_OK:  
    mySerial.print("OK,\t"); 
    break;
    case DHTLIB_ERROR_CHECKSUM: 
    mySerial.print("Checksum error,\t"); 
    break;
    case DHTLIB_ERROR_TIMEOUT: 
    mySerial.print("Time out error,\t"); 
    break;
    case DHTLIB_ERROR_CONNECT:
        mySerial.print("Connect error,\t");
        break;
    case DHTLIB_ERROR_ACK_L:
        mySerial.print("Ack Low error,\t");
        break;
    case DHTLIB_ERROR_ACK_H:
        mySerial.print("Ack High error,\t");
        break;
    default: 
    mySerial.print("Unknown error,\t"); 
    break;
}

  //get temp
  int c = (DHT.temperature);
  //convert C to F
  int f = fahr(c);
  //oledWriteString expects a charArray of text. convert our int to that. https://arduino.stackexchange.com/questions/42986/convert-int-to-char
  char mytemp[5];
  //char, int, base of number
  itoa(f, mytemp, 10);
  //blank screen
  oledFill(0x0);
  //row,col,char array of text, font, invert [1|0]
  oledWriteString(0,1,mytemp,FONT_LARGE,0);
  mySerial.println(f);
  //quick blink to show the reading is done
  delay(checkmsec);
  digitalWrite(led, HIGH);
  delay(300);
  digitalWrite(led, LOW);
}

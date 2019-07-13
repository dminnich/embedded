    /////////////////////////////////////////////////////////////////
   //           Arduino FM Radio Project v2.0                     //
  //       Get the latest version of the code here:              //
 //        📥 http://educ8s.tv/arduino-fm-radio-2               //
/////////////////////////////////////////////////////////////////

#include <TEA5767N.h>  //https://github.com/mroger/TEA5767
#include <SPI.h>
#include <Adafruit_GFX.h>  //https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_PCD8544.h> //https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library
#include "GrenadierNF18pt7b.h"
#include "GrenadierNF15pt7b.h"
#include "splash.h"
#include <ClickEncoder.h> //https://github.com/0xPIT/encoder
#include <TimerOne.h>  //https://github.com/PaulStoffregen/TimerOne
#include <EEPROM.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(5, 4, 3);

int SECONDS_TO_AUTOSAVE = 300;
float previous_frequency = 0;
float frequency = 95.0f;

ClickEncoder *encoder;
int16_t last, value;

boolean up = false;
boolean down = false;
boolean middle = false;

long seconds = 0;
long previousMillis = 0; 

bool backlightON = false;

long interval = 1000; 
  
TEA5767N radio = TEA5767N();

void setup() 
{
  pinMode(7, OUTPUT);
  digitalWrite(7,HIGH);
  
  initDisplay();
  
  showSplashScreen();
  
  radio.setMonoReception();
  
  radio.setStereoNoiseCancellingOn();

  initRotaryEncoder();
  
  Serial.begin(9600);

  frequency = readFrequencyFromEEPROM();

  if(isnan(frequency))
  {
    Serial.println("setup(): EEPROM has no stored frequency.");
    frequency = 88.0;
    writeFrequencyToEEPROM(&frequency);
  }
    
  printText(String(frequency,1),String(frequency,1));
}

void loop() {

  unsigned long currentMillis = millis();

  checkRotaryEncoderButton();

  if(currentMillis - previousMillis > interval) 
  {
    if(frequency!=previous_frequency)
    {
      previous_frequency = frequency;
      radio.selectFrequency(frequency);  
      seconds = 0;
    }else
    {
      seconds++;
      if(seconds == SECONDS_TO_AUTOSAVE)
      {
          float read_frequency = readFrequencyFromEEPROM();
          if(read_frequency!=frequency)
          {
            Serial.println("loop(): Saving new frequency to EEPROM");
            writeFrequencyToEEPROM(&frequency);
          }
      }
    }
    previousMillis = currentMillis; 
    Serial.println("loop(): "+String(seconds));  
  }

 readRotaryEncoder();

 if(up)
 {
  up = false;
  previous_frequency = frequency;
  frequency = frequency-0.1f;
  printText(String(frequency,1),String(previous_frequency,1));
 }

 if(down)
 {
  down = false;
  previous_frequency = frequency;
  frequency = frequency+0.1f;
  printText(String(frequency,1),String(previous_frequency,1));
 }
}

void printText(String text, String previous_text)
{
  display.clearDisplay();   // clears the screen and, WHITE buffer  

  if(previous_text.length() == 4)
  {
    display.setFont(&GrenadierNF18pt7b);
  }else
  {
    display.setFont(&GrenadierNF15pt7b);
  }
  display.setTextColor(WHITE);
  display.setCursor(0,37);
  display.println(previous_text);
  display.display();
  
  if(text.length() == 4)
  {
    display.setFont(&GrenadierNF18pt7b);
  }else
  {
    display.setFont(&GrenadierNF15pt7b);
  }
  display.setTextColor(BLACK);
  display.setCursor(0,37);
  display.println(text);
  display.display();
}

void initRotaryEncoder()
{
  encoder = new ClickEncoder(A1, A0, A2);
  encoder->setAccelerationEnabled(false);
   
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr); 

  last = encoder->getValue();  
}

void initDisplay()
{
  display.begin();
  display.setContrast(20);
  display.clearDisplay();   // clears the screen and, WHITE buffer  
  display.display();
}

 void timerIsr() 
{
  encoder->service();
}

void showSplashScreen()
{
  display.clearDisplay();
  display.drawBitmap(0, 0,  SPLASH, 84, 48, 1);
  display.display();
  delay(3000);
  display.clearDisplay();   // clears the screen and, WHITE buffer  
}

void writeFrequencyToEEPROM(float *freq)
{
 byte ByteArray[4];
 memcpy(ByteArray, freq, 4);
 for(int x = 0; x < 4; x++)
 {
   EEPROM.write(x, ByteArray[x]);
 }  
 Serial.println("writeFrequencyFromEEPROM(): "+String(frequency));
}

float readFrequencyFromEEPROM()
{
  float frequency = 86.0;
  byte ByteArray[4];
  for(int x = 0; x < 4; x++)
  {
   ByteArray[x] = EEPROM.read(x);    
  }
  memcpy(&frequency, ByteArray, 4);
  Serial.println("readFrequencyFromEEPROM(): "+String(frequency));
  return frequency;
}

void checkRotaryEncoderButton()
{
    ClickEncoder::Button b = encoder->getButton();
  if (b != ClickEncoder::Open) {
    if(b==ClickEncoder::Clicked)
    {
      Serial.println("Toggle BackLight");
      if(backlightON == false)
      {
        backlightON = true;
        turnOnBacklight();
      }else
      {
        backlightON = false;
        turnOffBacklight();
      }
    }
  }
}

void turnOnBacklight()
{
  digitalWrite(7,LOW);
}

void turnOffBacklight()
{
  digitalWrite(7,HIGH);
}

void readRotaryEncoder()
{
  value += encoder->getValue();
  
  if (value/2 > last) {
    last = value/2;
    up = true;
    delay(50);
  }else   if (value/2 < last) {
    last = value/2;
    down = true;
    delay(50);
  }
}

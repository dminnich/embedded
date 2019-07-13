#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>

#define ONE_WIRE_BUS D2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);



void setup() {
Serial.begin(9600);
  // Start up the library
  sensors.begin();
}

void loop() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  int tc = sensors.getTempCByIndex(0);
  float tf = (tc * 1.8) + 32;
  int tfr = round(tf);
  String s = String(tfr) + "F";
  Serial.println(s);
  delay(5000);
}

#include <math.h>
//test cases
//if bowl is full - do nothing. just keep looping and checking.
//if bowl is nearly empty - add water up to trigger, cut off pump, normal loops to follow
//if bowl is nearly empty  and water pump runs too long - failsafe
const int relay = D2;
const int trigpin = D4;
const int echopin = D3;
long waterdistance, duration, wd;
//if water is more than cm from the sensor cut on the waterpump
//These calcs arent perfect.  exepct to play with them.
int waterdistancetrigger = 13;
//relay strobe prevention.  we can't go from on/off to quickly. pause this many ms after state change.
//account for this much water flow buffer before overflowing the bowl.
int strobepausems = 500; //.5 sec
//waterpump should NEVER run more than this many seconds straight
int failsafe = 30;
//int failsafe = 22; //debug
//how many cycles equal a sec
const int strobepauses = round(1000/strobepausems);
int fscounter = failsafe * strobepauses;
//check to see if there is work to do every x secs.
//Don't do it too often or water may be added while the pet is drinking.
int checkfreq = 300; //every 5 minutes
//int checkfreq = 5;  //debug
//int checkfreq = 60;
int checkfreqms = checkfreq * 1000;
//a single check of needs water is occasionally followed by the next check of no-need for water as the measurements aren't exact and
//by nature we stay right on that line. this causes strobing of the relay.  To prevent we will read the measurement several times quickly.
//how many samples per measure
const int MEASURE_SAMPLES = 25;
const int MEASURE_SAMPLE_DELAY = 5;
//^125 ms worth of measurements + .5 ms from microsec pauses across the total of all individual mesaurements



void setup() {
  pinMode(relay, OUTPUT);
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  Serial.begin(9600);
  //pump on when relay is LOW off when HIGH
  digitalWrite(relay, HIGH);
}

long currentdistance(){
  //https://randomnerdtutorials.com/complete-guide-for-ultrasonic-sensor-hc-sr04/
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigpin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(11);
  digitalWrite(trigpin, LOW);
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  delayMicroseconds(5);
  duration = pulseIn(echopin, HIGH);
  //https://lastminuteengineers.com/arduino-sr04-ultrasonic-sensor-tutorial/
  waterdistance= (duration*0.034)/2;
  return waterdistance;
}


//adding average measurements to prevent strobing 
//https://arobito-docs.readthedocs.io/en/latest/mcu/experiments/pages/hc-sr04_distance_measurement.html
long avgcurrentdistance(){
    long measureSum = 0;
  for (int i = 0; i < MEASURE_SAMPLES; i++)
  {
    delay(MEASURE_SAMPLE_DELAY);
    measureSum += currentdistance();
  }
  return measureSum / MEASURE_SAMPLES;
}


void loop() {
  delay(8000); //debug. allows serial monitor launching before things happen
  int i = 0;
  wd = avgcurrentdistance();
  Serial.println("Initial distance is:" + String(wd));
  if (wd >= waterdistancetrigger + 2){
  while ((wd > waterdistancetrigger) && (i < fscounter)) {
    //run the pump
    if(digitalRead(relay) == HIGH) {
      Serial.println("running pump in while loop");
      digitalWrite(relay, LOW);
    }
    delay (strobepausems);
    wd = avgcurrentdistance();
    Serial.println("Decreased distance is:" + String(wd)); //debug
    Serial.println("Counter is:" + String(i) + "of" + String(fscounter)); //debug
    i++;
  }
  }

  //if while reaches a false or was false from the start, having no work to do, always make sure pump is of.
  if(digitalRead(relay) == LOW) {
     Serial.println("cutting off pump");
     digitalWrite(relay, HIGH);
  }

  //lets say the sensor falls off the bowl or something.  water is running but distance isn't changing.
  //this traps that scenario so water doesn't run forever.
  if (i >= fscounter) {
    while (true) {  //stop forever
      Serial.println("Failsafe flood protection activated. Restart the system to deactivate.");
      delay (5000);
    }
  }

  //check water level again in
  Serial.println("Check complete.  Will run again in " + String(checkfreq) + " secs.");
  delay(checkfreqms);
}

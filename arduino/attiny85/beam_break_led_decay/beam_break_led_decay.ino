/*

When somebody walks by or opens a cabinet door, notice. Light an LED for X time while dimming it in intervals like a car overhead light. 

BOOTLOADER, ETC: 
#https://engineersportal.com/blog/2018/5/19/attiny85-arduino-board-how-to-flash-the-arduino-bootloader-and-run-a-simple-sketch

LED:
short/input = 330 resistor 5/pb0 = led
long/ground = ground

ATTINY:
1/pb5 = nothing
2/pb3 = nothing
3/pb4 = nothing
4/gnd = gnd on power supply

5/pb0 = resistor + led
6/pb1 = trig on ultrasonic
7/pb2 = echo on ultrasonic
8/vcc = vcc

Ultrasonic:
ground = ground
echo = 7/pb2
trig = 6/pb1
vcc = vcc
#https://stackoverflow.com/questions/41501360/getting-distance-in-inches-and-cm-from-ultrasonic-sensor-in-arduino
 */ 

//times won't be exact due to non-float calcs and the time it takes to measure the distance.
const int led = 0;
const int trig = 1;
const int echo = 2;
//how frequently to look for a change
const int checksec = 1; 
int checkmsec = checksec * 1000;
//how long total should the light be on
const int lightsec = 30;
int lightmsec = lightsec * 1000;
//how many times should the light get dimmer
const int decaysteps = 4;
int lightpause = lightmsec / decaysteps;
int brightness = 255 / decaysteps;
//when an object gets closer/further than this many inches from the ultrasonic sensor, react.
const int trippointin = 6;
//purpose.  check if closer (door alarm)  or further (cabniet door opened)
const char test = "near";
// check every 1 sec. if object is closer than 6in.  light led + 4 brightness adjustments that happen every 7 secs for about 30 seconds of light as you walk by the nightlight.


void setup() {
  pinMode(led, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  analogWrite(led, 0);
}

void loop() {
  float duration, distancecm, distancein;
  //off on needed to do measurment.
 digitalWrite(trig, LOW); 
 delayMicroseconds(2); 
 digitalWrite(trig, HIGH);
 delayMicroseconds(10); 
 digitalWrite(trig, LOW);
 duration = pulseIn(echo, HIGH);
 distancecm = (duration/2) / 29.1;
 distancein = distancecm * 0.393701; 

 if (test == "near"){
 /*debug
  digitalWrite(led, HIGH);
  delay(3000);
  digitalWrite(led,LOW);
  delay(3000);
  */
  if (distancein <= trippointin){
   lightdecay();   
  }
 }

 if (test == "far"){
  /*debug
  digitalWrite(led, HIGH);
  delay(5000);
  digitalWrite(led,LOW);
  delay(5000);
  */
  if (distancein >= trippointin){
   lightdecay();   
  }
 }
 
  analogWrite(led, 0);
  delay(checkmsec);
}

void lightdecay()
{
    int counter = decaysteps;
    while (counter > 0){
      analogWrite(led, brightness * counter);
      delay(lightpause);
      counter--;
    }
}

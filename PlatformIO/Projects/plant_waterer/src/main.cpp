#include <Arduino.h>
/*
//test cases
//if soil is wet enough - do nothing. just keep looping and checking.
    percentdry stays lower than pctdrytrigger
       comment analogReads, wms,percentdry and set percentdry = 0
    one failsafe red blink every 10 secs to show its checking conditions
//if soil is nearly wet enough - add water up to pctdrytrigger, cut off pump, normal loops to follow
    percentdry starts higher than pctdrytrigger then decrements until it reaches pctdrytrigger
       comment analogReads,wms,percentdry and set percentdry = 90 and pctdrytrigger = 87 and percentdry-- inside loop
    blink one failsafe red led for 1 secs to signify loop was entered
    turn on green status light and pump for about 3 secs -- watering plant -- wet enough
    turn off green light.  
    wait 10 secs. do it again
//if soil stays dry and water pump runs too long - jail 
     percentdry stays higher than pctdrytrigger
       comment analogReads, wms,percentdry and set percentdry = 100
     blink one failsafe red led for 1 secs to signify loop was entered
     turn on green status light and pump for about 5 secs -- watering plant -- enter jail
     turn off green status light and pump.  turn on red failsafe light showing we are in jail
     stay in this state until rebooted


#attiny Pinout: 
https://github.com/MCUdude/MicroCore

#transistor as switch
triggering the base connects the gnds. emitter (circuit) and collector (device) completing the circuit
base must be driven into saturation and stay roughly there without sending too much additional current
transistor must be capable of handling C-E volts and amps your load will have
mcu pin must be able to supply enough volts and current to saturate the base.
https://www.youtube.com/watch?v=sRVvUkK0U80 - other resistor calcs are easier
https://www.youtube.com/watch?v=WxKBI96CY1Y - no diode hookup.  resistor terminology wrong.
https://www.bc-robotics.com/tutorials/controlling-a-solenoid-valve-with-arduino/ - good pics
https://electronics.stackexchange.com/a/64603 - schmatic
https://www.electronics-tutorials.ws/transistor/tran_2.html - pinout
https://teachmetomake.wordpress.com/how-to-use-a-transistor-as-a-switch/ - good calc writeups


#pump
https://www.amazon.com/Submersible-2-5-6V-Brushless-Circulation-System/dp/B07PCM2QZN
https://www.youtube.com/watch?v=pCxeZkLhqSE - suggests low value resistor
Mini Micro Submersible Water Pump DC 2.5-6V Low Noise Brushless Motor Pump
DC Voltage: 2.5-6V
Working current: 130-220mA

#transistor datasheet
https://alltransistors.com/transistor.php?transistor=52891
https://www.onsemi.com/pub/Collateral/MPSA42-D.PDF
Maximum Collector Current |Ic max|: 0.5 A   -- above 220mA pump
Maximum Collector-Emitter Voltage |Vce|: 305 V -- way above 6v pump
Base-emitter saturation voltage VBE(sat)IC= 20mA, IB=2mA
Arduino can drive 40mA from a pin.  We can saturate and have info for our next section.

#base resistor calc
https://forum.arduino.cc/index.php?topic=105497.0
For a normal bipolar transistor (not a darlington), Vce(sat) is typically quoted with Ib = 0.1 * Ic. Therefore, to achieve the value of Vce(sat) on the datasheet, you should plan the base current to be 1/10 of the collector current you want to switch. For example, if you are switching 60mA, design for a base current of 6mA. The base voltage will be around 0.7v and the voltage from the Arduino will be nearly 5V (assuming a 5V Arduino). Therefore a suitable base drive resistor would be around 4.3/0.006 = 717 ohms (nearest standard value 680 ohms).
The 1/10 rule works based on the VBE(sat)IC= 20mA, IB=2mA spec from above.
~300mA from pump *.1 to saturate base = 30mA = .03A
5v pin - .7 diode v drop = 4.3v
4.3v / .03A = 143ohms = using 100 ohm resistor


#diode datasheet
GND of device becomes positive power source when cut off. Power flows from "gnd" of device through diode back to "vcc" on device, protecting the rest of the circuit.
https://www.youtube.com/watch?v=haUWO7BGYTE - explains it well
https://www.khanacademy.org/science/electrical-engineering/ee-semiconductor-devices/ee-diode/a/ee-diode-circuit-element - cathode / anode schematic and real pic
https://www.mouser.com/ProductDetail/ON-Semiconductor-Fairchild/1N4148?qs=i4Fj9T%2FoRm8RMUhj5DeFQg%3D%3D
https://ourpastimes.com/function-1n4148-diode-8507537.html
Peak Reverse Voltage: 100 V -- way more than pump 6v

#sensor
https://www.youtube.com/watch?v=9h3JKwUsn2A
https://how2electronics.com/interface-capacitive-soil-moisture-sensor-with-arduino/
https://wiki.dfrobot.com/Capacitive_Soil_Moisture_Sensor_SKU_SEN0193


Pump:
VCC = direct from secondary 5v supply (not through arduino). 
      cathode(line) on diode > anode (non-line) on diode >  GND on pump > 3/collector on transistor
GND = 3/collector on transistor
      anode (non-line) on diode > cathode(line) on diode > VCC on pump > VCC from secondary 5v supply (not arduino)

Capacitive Water sensor:
GND = GND
VCC = VCC from arduino
AOUT = 2/pb3/a3 on attiny

A42 Transistor:
1/emitter = GND
2/base = 100 ohm resistor > 5/pb0 on attiny
3/collector = GND on pump
              anode (non-line) on diode > cathode(line) on diode > VCC on pump > VCC from secondary 5v supply (not arduino)


Secondary power:
vcc = VCC on pump > cathode(line) on diode > anode (non-line) on diode > gnd on pump > 3/collector on transistor
gnd = gnd from arduino


ATTINY 13a:
1/pb5 = nothing
2/pb3/a3 = aout on water sensor
3/pb4/a2 = 1k resistor > failsafe red led > gnd
4/gnd = gnd 
        failsafe led
        status led
        1/emitter on transistor
        moisture sensor
        Secondary VCC gnd (non-arduino)
5/pb0 = 100 ohm resistor > 2/base on transistor
6/pb1 = 1k resistor > pump status green LED > gnd
7/pb2 = nothing
8/vcc = vcc on sensor
*/

//vars
const uint8_t pp = 0; // pump pin
const uint8_t sp = 3; //sensor pin
const uint8_t plp = 1; //led that will show if the pump should be running
const uint8_t fsp = 4; //led that will show if program entered jail

//hook your sensor up to an arduino with a serial console. get various readings via analogRead and Serial.println. try to get a feel for its range and stepping patterns.
//the lower the number the more moist the soil is and the more conductive it is. it also means the less likely it is that you need to add water.
//play around with it a bit while watching the serial console and give yourself some breathing room. 
//for example i got dry at 670, so i'm going to say anything above 600 is dry.  for in a cup of water i got 270 so i'm going to say anything less than 300 is very wet.
const int dry = 600; //when the sensor is completey dry
const int wet = 300; //when the sensor is wet up the line
int wm; //will be used to store the current water moisture raw value
int percentdry; //the code will map the current reading between the dry and wet vaules mentioned above and then convert that to a 0-100 dry range.  the HIGHER this number is the more likely it is that your plant NEEDS MORE water.
uint8_t pctdrytrigger = 80; //how dry you want the soil to be. the LOWER this number the MORE WET your plant will stay.  When percentdry is higher than this water will be added (and percentdry decremented) until this is reached.


//every cfms a loop kicks off where the the sensor and pump operate in near real time to decide what action to take over and over again
//to do this cycles are made.  each cycle in the parent loop includes measuring moisture level and possibly running the pump. 
//after the work in each cycle, a cpms pause is taken to give the sensor and pump a break before running the next cycle
//cycles will continue to occur indefinitely inside of EACH parent loop until either your plant is in compliance to specs or until fscount is reached
//since you don't want a pump running forever (could flood the floor or burn up), you must set the max time you'd want it to run in fsms
//fscount is calculated by fsms / cpms.  
//if fscount is reached inside of a parent loop the program enters a jail and you must reboot your device to have it work again.
//if after some cycles in a parent loop your plant is now compliant with specs, a pause of cfms happens and another parent loop kicks off.



const int cfms = 10000; //how frequently in ms to check if the plant needs more water. - 10s
const int cpms = 500; //pauses between measurement and adjustment actions - .5s
const int fsms = 5000; //waterpump should NEVER run more than this many ms seconds straight. - 5s 
int fscount = fsms / cpms; //calculation of above for jailing the program if pump runs too long



void setup() {
  pinMode(pp,OUTPUT); //pump is output
  pinMode(plp,OUTPUT); //pump status led is output
  pinMode(fsp,OUTPUT); //jail led is output
  pinMode(sp,INPUT); //sensor is input
  digitalWrite(pp, LOW); //make sure pump is off at boot
}

void loop() {
  digitalWrite(fsp, HIGH); //DEBUG placement watermark
  delay(1000); //DEBUG placement watermark
  digitalWrite(fsp, LOW); //DEBUG placement watermark
  delay(1000); //DEBUG placement watermark
  int i = 0; //initialize counter
  wm = analogRead(sp); //read the current moisture level  //DEBUG
  wm = constrain(wm, wet, dry);//anything lower than wet or higher than dry will be set to those values. needed for callibrated-ish calculations. //DEBUG
  percentdry = map(wm, wet, dry, 0, 100); //convert the above into a percentage.  The higher the number the more your plant may need water. //DEBUG
  //percentdry = 0; //DEBUG soil is wet enough use case
  //percentdry = 100; //DEBUG jail case
  //percentdry = 90; //DEBUG add water case
  //pctdrytrigger = 87; //DEBUG add water case
  if (percentdry > pctdrytrigger){ //if soil is drier than desired
    while ((percentdry > pctdrytrigger) && (i < fscount)) { //if soil is drier than desired and the pump hasn't been running too long
      if (digitalRead(pp) == LOW) { //if pump isn't running
        digitalWrite(pp, HIGH); //run it
        digitalWrite(plp,HIGH);// turn on the pump status light
      }
      delay(cpms); //sensors and pumps can't move too fast
      wm = analogRead(sp); //read the current moisture level  //DEBUG
      wm = constrain(wm, wet, dry);//anything lower than wet or higher than dry will be set to those values. needed for callibrated-ish calculations. //DEBUG
      percentdry = map(wm, wet, dry, 0, 100); //convert the above into a percentage.  The higher the number the more your plant may need water. //DEBUG
      //percentdry--; //DEBUG add water case
      i++; //restart while loop
    }
  }
//if we didn't need to add water on initial check OR if the soil isn't too dry now that the while loop is through OR if the pump has been running too long
  if(digitalRead(pp) == HIGH) { //shutoff the pump and pump status led
    digitalWrite(pp, LOW);
    digitalWrite(plp, LOW);
  }

//if the pump has been running too long, enter jail.  you'll need to restart the device to get out of this loop
  if (i >= fscount) {
    while (true) {  //stop forever
      digitalWrite (fsp,HIGH);
      delay (5000);
    }
  }
//we made it through the loop and either noticed that water wasn't needed or we added water up to the specifications within a failsafe amount of time
  delay(cfms); //wait this long before doing the main check again.
}
/*

IR remote controled relay

BOOTLOADER, ETC: 
#https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/
#https://i.imgur.com/zEoWhQk.png
#http://www.circuitbasics.com/arduino-ir-remote-receiver-tutorial/
#https://github.com/crankyoldgit/IRremoteESP8266
#https://howtomechatronics.com/tutorials/arduino/control-any-electronics-with-a-tv-remote-arduino-ir-tutorial/




IR sensor:
G = gnd
R = vcc
Y = d1/gpio5

Relay:
vcc = vcc
gnd = gnd
in = d2/gpio4

nodemcu:
3v = nothing
gnd = nothing
tx/gpio1 = nothing
rx/gpi03 = nothing
d8/gpio15 = nothing
d7/gpio13 = nothing
d6/gpio12 = nothing
d5/gpio14 = nothing
gnd = nothing
3v = nothing
d4/gpio2 = nothing
d3/gpio3 = nothing
d2/gpio4 = relay in
d1/gpio5 = ir in
d0/gpio16 = nothing

a0 = nothing
rsv = nothing
rsv = nothing
sd3/gpio10 = nothing
sd2/gpio9 = notihng
sd1/mosi = notihng
cmd/cs = nothing
sd0/miso = nothing
clk/sclk = nothing
gnd = gnd
3.3 = 3.3
en = nothing
rst = nothing
gnd = nothing
vin = nothing


 */ 

//ir
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
const uint16_t kRecvPin = 5;
IRrecv irrecv(kRecvPin);
decode_results results;

//relay
const uint8_t relay = 4;

//vars
const uint8_t strobe = 100;

void setup() {
 Serial.begin(9600);
 //ir
 pinMode(kRecvPin, OUTPUT);
 irrecv.enableIRIn();  // Start the receiver
 //relay
 pinMode(relay, OUTPUT);
 digitalWrite(relay, HIGH);    // turn off RELAY.  Logic reversed
}

void loop() {
  if (irrecv.decode(&results)){
      serialPrintUint64(results.value, HEX);
      Serial.println("");
       
       switch(results.value){

          case 0xFFA25D: //Keypad button "power"
          digitalWrite(relay, !digitalRead(relay)); //flip the current state
          delay(strobe);
          }

        irrecv.resume();
    }
     delay(strobe);
}

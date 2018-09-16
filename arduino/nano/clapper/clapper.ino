

const int sensor = 3;
const int led = 2;
// const int sensor = A0;


// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(led, OUTPUT);
  pinMode(sensor, INPUT);
  Serial.begin(9600);  
}

// the loop function runs over and over again forever
void loop() {
  int sensorstate;
  static bool ledstate = false;
  //sensorstate = analogRead(sensor);
  sensorstate = digitalRead(sensor);

//threshold
//if (sensorstate >= 810) {
if ( sensorstate == HIGH ) {
  if (ledstate) { //on to off
    ledstate = false;
    digitalWrite(led, LOW);
    Serial.print ("on to off: ");
 //   Serial.println (sensorstate);
//delay(3000);
  }

  else {
    ledstate = true;  //off to on
    digitalWrite(led, HIGH);  
    Serial.print ("off to on: ");
 //   Serial.println (sensorstate);
   // delay(3000);
  }
}
else {
  Serial.println("nothing above noise floor: ");
  //Serial.println (sensorstate);
}

delay(150);

}


  


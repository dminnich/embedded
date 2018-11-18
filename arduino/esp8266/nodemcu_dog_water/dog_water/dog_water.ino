const int relay = D2;
const int sensor = A0;
int moisture;
//if moisture is less than equal to trigger turn on relay. you'll need to do some callibration checks.
int trigger = 400;
int checkfreq = 15;
int checkfreqms = checkfreq * 1000;

void setup() {
  pinMode(relay, OUTPUT);
  pinMode(sensor, INPUT);
  Serial.begin(9600);
  //pump on when relay is LOW off when HIGH
  digitalWrite(relay, HIGH);
}


void loop() {
  moisture = analogRead(sensor);
  
  Serial.println("Initial moisture is:" + String(moisture));
  while (moisture <= trigger) {
    //run the pump
    if(digitalRead(relay) == HIGH) {
      Serial.println("running pump in while loop");
      digitalWrite(relay, LOW);
    }
    delay (300);
    moisture = analogRead(sensor);
    Serial.println("Incremented moisture is:" + String(moisture));
  }
   //cut off the pump
   Serial.println("cutting off the pump");
   digitalWrite(relay, HIGH);  
   delay(checkfreqms);
}

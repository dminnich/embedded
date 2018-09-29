const int relay = D2;
const int sensor = A0;
int moisture;
//if moisture is less than equal to trigger turn on relay. you'll need to do some callibration checks.
int trigger = 400;
//turn on pump for pumpcycle seconds when needed
int pumpcyle = 3;
int pumpcyclems = pumpcyle * 1000;
//check water level every checkfreq seconds
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
  
  Serial.println("moisture is:" + String(moisture));
  if (moisture <= trigger) {
    Serial.println("water needed running pump");
    digitalWrite(relay, LOW);
    delay(pumpcyclems);
    digitalWrite(relay, HIGH);  
  }
  else {
    Serial.println("no water needed");
    if(digitalRead(relay) == LOW) {
      digitalWrite(relay, HIGH);
    }
  }
  delay(checkfreqms);
}

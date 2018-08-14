const int buzzer=9;
const int pir=10;
int pirState= LOW;
int state = 0;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(buzzer, OUTPUT);
pinMode(pir, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

state = digitalRead(pir);
if (state == HIGH) {
  if (pirState == LOW) {
    Serial.println("motion detected");
    tone(buzzer, 500);
    delay(1000);
    noTone(buzzer);
    pirState = HIGH;
  }
} else {
    if (pirState == HIGH){
      pirState = LOW;
      //delay(10000);
    }
   }
}

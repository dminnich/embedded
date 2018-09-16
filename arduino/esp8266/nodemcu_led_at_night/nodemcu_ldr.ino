const int led = D0;
const int ldr = D2;

//when ldr is high its dark so light needs to be on
//high turns light on 


void setup() {
  // put your setup code here, to run once:
pinMode(led, OUTPUT);
pinMode(ldr, INPUT);
digitalWrite(led, LOW);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int so = digitalRead(ldr);
  Serial.println(so);
  
  if (so == HIGH) {
    Serial.println("it is night time.  turning light on");  
    digitalWrite(led, HIGH);  
  }
  else {
    Serial.println("it is day time.  turning light off");  
    digitalWrite(led, LOW);  
  }
  delay(5000);
  
}

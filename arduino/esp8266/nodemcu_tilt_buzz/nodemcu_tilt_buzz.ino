
void setup() {
  pinMode(D2, OUTPUT);     
  pinMode(D3, INPUT);
}

void loop() {
  if (digitalRead(D3) == HIGH){
  tone(D2, 1000);  
  }
  else {
    noTone(D2);
  }
  delay(1000);                     
}

int PiN1 = 2;
int PiN2 = 3;
int PiN3 = 4;
int PiN4 = 5;
int PiN5 = 6;
int PiN6 = 7;
int PiN7 = 8;
int PiN8 = 9;

void setup() {
  Serial.begin(9600);  // start serial for output
  pinMode(PiN1, INPUT);
  pinMode(PiN2, INPUT);
  pinMode(PiN3, INPUT);
  pinMode(PiN4, INPUT);
  pinMode(PiN5, INPUT);
  pinMode(PiN6, INPUT);
  pinMode(PiN7, INPUT);
  pinMode(PiN8, INPUT);
}

void loop() {
  if(digitalRead( PiN1)){
      Serial.println( "1");
  }
  if(digitalRead( PiN2)){
      Serial.println( "2");
  }
  if(digitalRead( PiN3)){
    Serial.println( "3");
  }
  if(digitalRead( PiN4)){
    Serial.println( "4");
  }
  if(digitalRead( PiN5)){
    Serial.println( "5");
  }
  if(digitalRead( PiN6)){
    Serial.println( "6");
  }
  if(digitalRead( PiN7)){
    Serial.println( "7");
  }
  if(digitalRead( PiN8)){
    Serial.println( "8");
  }
  delay(1000);
}
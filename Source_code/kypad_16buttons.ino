int SCLpin = 13; 
int SDOpin = 12;

byte key;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode( SCLpin, OUTPUT);
  pinMode( SDOpin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  key = getKeyStatus();

  if(key){
    Serial.println(key);
  }

  delay(100);
}

byte getKeyStatus() {
  byte Count;
  byte Key = 0;

  for( Count = 1; Count <=16; Count++){
    digitalWrite( SCLpin, LOW);
    if( !digitalRead( SDOpin)){
      Key = Count;
    }
    digitalWrite( SCLpin, HIGH);
  } 
  
  return Key;
}

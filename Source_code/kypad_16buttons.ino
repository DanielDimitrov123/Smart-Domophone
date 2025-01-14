void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  getKeyStatus();
  delay(1000);
}

int pinTouchData = 12; 
int pinTouchClock = 13; 

// key configuration
uint8_t keys[16] = {0};

// simple 8229 IC protocol, pinging the clock line 16 times to get all key states
void getKeyStatus() {
  uint8_t num = 0;
  for (uint8_t i = 0; i < 16; i++) {
    digitalWrite(pinTouchClock, LOW); // pull clock low
    if (digitalRead(pinTouchData) == 0) // read bit for key state
      keys[i] = 1; // key touched
    else
      keys[i] = 0; // not touched
    digitalWrite(pinTouchClock, HIGH); // pull clock high to signal done
  }   
}

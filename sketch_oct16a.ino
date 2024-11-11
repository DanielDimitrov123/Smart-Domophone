// Arduino master sketch

int Light_pin = 13;
bool Light = 1;
bool Dark = 0;

int dot_delay = 500;
int dash_delay = 1000;
int blank_delay = 500;

void setup(){
  Serial.begin(9600);
  pinMode( Light_pin, OUTPUT);
}

void loop(){
	// read potentiometer position
  for( int i = 0; i < 10; i++){  
    blink();
    blink();
    Serial.print( i, " - ");
    dot();
    dot();
    dot();

    dash();
    dash();
    dash();

    dot();
    dot();
    dot();
    Serial.println();
  }
}

void blink(){
  digitalWrite( Light_pin, Dark);
  delay( blank_delay);
}

void dot(){
  digitalWrite( Light_pin, Light);
  Serial.print( ".");
  delay( dot_delay);
  blink();
}

void dash(){
  digitalWrite( Light_pin, Light);
  Serial.print( "-");
  delay( dash_delay);
  blink();
}
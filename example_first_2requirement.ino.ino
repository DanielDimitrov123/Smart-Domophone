#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); // RX, TX

//const byte interruptPin = 2;
char incoming_char=0;



void setup()
{

  //pinMode(interruptPin, INPUT);
  attachInterrupt(2, check, LOW);

  
  // Open serial communications and wait for port to open:
  Serial.begin(9600); // for serial monitor
  mySerial.begin(9600); // for GSM shield
  delay(10000);
  mySerial.print("AT+CMGF=1\r");  // set SMS mode to text
  delay(1000);
  mySerial.print("AT+CNMI=2,2,0,0,0\r"); 
  // blurt out contents of new SMS upon receipt to the GSM shield's serial out
  delay(100);

    // set the data rate for the SoftwareSerial port
  
  
}

void loop() // run over and over
{
  
}

void check()
{
  //Serial.println("test");
  if(mySerial.available() >0)
  {
    incoming_char=mySerial.read(); //Get the character from the cellular serial port.
    Serial.print(incoming_char); //Print the incoming character to the terminal.
  }
}
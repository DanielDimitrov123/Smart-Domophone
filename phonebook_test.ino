#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM800L
SoftwareSerial sim800l(11, 10); //SIM800L Tx & Rx is connected to Arduino #11 & #10

void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM800L
  sim800l.begin(9600);

  Serial.println("Initializing..."); 
  delay(1000);

  sim800l.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();

  sim800l.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  // sim800l.println("AT+CPBS=?");
  // updateSerial();
  // sim800l.println("AT+CPBS=”SM”");
  // updateSerial();
  sim800l.println("AT+CPBR=?");
  updateSerial();
  sim800l.println("AT+CPBR=1,250");
  updateSerial();
  sim800l.println("AT+CPBW=,”6187759088″,129,”Adam”");
  updateSerial();
  sim800l.write(26);
}

void loop()
{
}

void updateSerial()  
{
  delay(500);
  while (Serial.available()) 
  {
    sim800l.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(sim800l.available()) 
  {
    Serial.write(sim800l.read());//Forward what Software Serial received to Serial Port
  }
}

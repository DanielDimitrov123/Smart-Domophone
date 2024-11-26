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
  sim800l.println("AT+CMGS=\"+359884266977\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  sim800l.print("Hello from Robot Amateur!!!"); //text content
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
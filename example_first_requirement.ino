#include <GSMSim.h>
#include <SoftwareSerial.h>
#include <string.h>



#define RX 7
#define TX 8
#define RESET 2
#define BAUD 9600

char outArray;
char inData[20]; 
char inChar=-1; 
byte index = 0; 
char * pch;
char* substring(char*, int, int);

GSMSim gsm;
  SoftwareSerial ATDevice =  SoftwareSerial(RX, TX);
/*
 * Also you can this types:
 * GSMSim gsm(RX, TX);
 * GSMSim gsm(RX, TX, RESET);
 * GSMSim gsm(RX, TX, RESET, LED_PIN, LED_FLAG);
 */

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  ATDevice.begin(9600);

  command("AT+CLIP=1",1000);
  delay(1000);


}


  String command(const char *toSend, unsigned long milliseconds) {
    String result;
    ATDevice.println(toSend);
    unsigned long startTime = millis();
    Serial.print("Return: ");
    while (millis() - startTime < milliseconds) {
      if (ATDevice.available()) {
        char c = ATDevice.read();
        result += c;  // append to the result string
      }
    }
  Serial.println();  // new line after timeout.
  return result;
  }


void loop() {

      command("+CLIP",1000);



     delay(2000);



}
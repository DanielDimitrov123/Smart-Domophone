#include <SoftwareSerial.h>

SoftwareSerial SIM800L(10, 11); // RX, TX pins for SIM800L

void setup() {
  Serial.begin(9600);       // Monitor communication
  SIM800L.begin(9600);      // SIM800L communication
  Serial.println("Initializing...");

  delay(1000);

  // Check communication with SIM800L
  SIM800L.println("AT");
  delay(500);
  if (SIM800L.available()) {
    Serial.println("SIM800L is ready.");
  } else {
    Serial.println("Failed to communicate with SIM800L.");
  }

  // Enable DTMF detection
  SIM800L.println("AT+DDET=1"); // Enables DTMF detection
  delay(500);
}

void loop() {
  // Check for incoming call
  SIM800L.println("AT+CLCC"); // List current calls
  delay(1000);

  while (SIM800L.available()) {
    String response = SIM800L.readString();
    Serial.println(response);

    if (response.indexOf("RING") != -1) { // Incoming call detected
      Serial.println("Incoming call detected. Answering...");
      SIM800L.println("ATA"); // Answer the call
      delay(1000);
    }

    // Check for DTMF tones
    if (response.indexOf("+DTMF:") != -1) {
      char dtmfTone = response.charAt(response.indexOf("+DTMF:") + 6); // Extract DTMF tone
      Serial.print("DTMF Tone Received: ");
      Serial.println(dtmfTone);

      // Perform actions based on DTMF tone
      if (dtmfTone == 1) {
        Serial.println("Action for '1': Option X selected.");
      } else if (dtmfTone == 2) {
        Serial.println("Action for '2': Option Y selected.");
      } else if (dtmfTone == 3) {
        Serial.println("Action for '3': Option Z selected.");
      } else {
        Serial.println("Invalid option.");
      }
    }
  }
}

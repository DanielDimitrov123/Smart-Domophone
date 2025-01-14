#include <SoftwareSerial.h>

// GSM Module communication
SoftwareSerial gsm(7, 8); // RX, TX

void setup() {
  Serial.begin(9600);  // Debugging
  gsm.begin(9600);     // SIM800L communication

  // Initialize GSM module
  gsm.println("AT");
  delay(1000);
  if (gsm.find("OK")) {
    Serial.println("GSM Module Ready");
  } else {
    Serial.println("GSM Module Failed");
    while (1);
  }
}

void loop() {
  // Example: Answer an incoming call
  if (gsm.available()) {
    String response = gsm.readString();
    Serial.println(response);

    // Detect incoming call
    if (response.indexOf("RING") != -1) {
      Serial.println("Incoming call detected!");
      gsm.println("ATA");  // Answer the call
      delay(1000);
    }
  }
}

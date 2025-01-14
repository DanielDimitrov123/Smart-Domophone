#include <SoftwareSerial.h>

// Pins for GSM Module
SoftwareSerial gsm(10, 11); // RX, TX

// Pin for Relay
const int relayPin = 7;

// Whitelisted numbers
String allowedNumbers[] = {"+359879896662"}; // Replace with valid numbers
int allowedCount = 1; // Total numbers in the list

void setup() {
  Serial.begin(9600);     // Debugging
  gsm.begin(9600);        // GSM Module communication
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Ensure the relay is off initially

  Serial.println("Initializing...");
  gsm.println("AT"); // Check GSM module
  delay(1000);

  if (gsm.find("OK")) {
    Serial.println("GSM Module Connected.");
  } else {
    Serial.println("GSM Module Failed to Connect.");
    while (1);
  }

  // Enable caller ID notification
  gsm.println("AT+CLIP=1");
  delay(1000);
}

void loop() {
  // Check for incoming data from GSM module
  if (gsm.available()) {
    String response = gsm.readString();
    Serial.println("GSM Response: " + response);

    // Check for an incoming call
    if (response.indexOf("+CLIP:") != -1) {
      String callerNumber = extractPhoneNumber(response);
      Serial.println("Incoming call from: " + callerNumber);

      // Validate the caller's number
      if (isAllowedNumber(callerNumber)) {
        Serial.println("Number authorized. Turning on the relay.");
        digitalWrite(relayPin, HIGH); // Turn on relay
        delay(5000);                 // Keep relay on for 5 seconds
        digitalWrite(relayPin, LOW); // Turn off relay
        Serial.println("Relay turned off.");
      } else {
        Serial.println("Unauthorized number. Ignoring the call.");
      }

      // End the call
      gsm.println("ATH");
      delay(1000);
    }
  }
}

// Function to extract phone number from the GSM response
String extractPhoneNumber(String response) {
  int startIndex = response.indexOf("\"") + 1; // Start of the phone number
  int endIndex = response.indexOf("\"", startIndex); // End of the phone number
  return response.substring(startIndex, endIndex);
}

// Function to check if the number is in the allowed list
bool isAllowedNumber(String number) {
  for (int i = 0; i < allowedCount; i++) {
    if (allowedNumbers[i] == number) {
      return true;
    }
  }
  return false;
}

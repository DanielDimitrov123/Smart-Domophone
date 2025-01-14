#include <SoftwareSerial.h>

// GSM Module Serial
SoftwareSerial gsm(7, 8); // RX, TX

// Array to store allowed numbers
String allowedNumbers[10]; // Adjust size as needed
int allowedCount = 0;

void setup() {
  Serial.begin(9600);     // Debugging
  gsm.begin(9600);        // GSM Module communication

  Serial.println("Initializing...");
  gsm.println("AT"); // Check GSM module
  delay(1000);

  if (gsm.find("OK")) {
    Serial.println("GSM Module Connected.");
  } else {
    Serial.println("GSM Module Failed to Connect.");
    while (1);
  }

  // Select SIM phonebook storage
  gsm.println("AT+CPBS=\"SM\"");
  delay(1000);

  // Read phonebook entries
  readPhoneBook();
}

void loop() {
  // Main loop - can be used for other operations
}

// Function to read phonebook entries from SIM
void readPhoneBook() {
  // Get phonebook range
  gsm.println("AT+CPBR=?");
  delay(1000);

  String response = "";
  while (gsm.available()) {
    response += gsm.readString();
  }

  Serial.println("Phonebook Range Response: " + response);

  // Extract range from the response
  int startIdx = response.indexOf("(") + 1;
  int endIdx = response.indexOf("-", startIdx);
  int startRange = response.substring(startIdx, endIdx).toInt();

  endIdx = response.indexOf(")", endIdx);
  int endRange = response.substring(response.indexOf("-", startIdx) + 1, endIdx).toInt();

  Serial.println("Phonebook Range: " + String(startRange) + " to " + String(endRange));

  // Read each entry in the range
  for (int i = startRange; i <= endRange; i++) {
    gsm.print("AT+CPBR=");
    gsm.println(i);
    delay(500);

    String entry = "";
    while (gsm.available()) {
      entry += gsm.readString();
    }

    if (entry.indexOf("+CPBR:") != -1) {
      Serial.println("Phonebook Entry: " + entry);
      String number = extractPhoneNumber(entry);
      if (number != "") {
        allowedNumbers[allowedCount++] = number;
        Serial.println("Stored Number: " + number);
      }
    }
  }

  Serial.println("Allowed Numbers Loaded:");
  for (int i = 0; i < allowedCount; i++) {
    Serial.println(allowedNumbers[i]);
  }
}

// Function to extract phone number from +CPBR response
String extractPhoneNumber(String response) {
  int startIdx = response.indexOf("\"") + 1; // Start of phone number
  int endIdx = response.indexOf("\"", startIdx); // End of phone number

  if (startIdx != -1 && endIdx != -1) {
    return response.substring(startIdx, endIdx);
  }

  return ""; // Return empty if no valid number is found
}

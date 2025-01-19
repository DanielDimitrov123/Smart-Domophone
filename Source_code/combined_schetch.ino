#include <SoftwareSerial.h>

// Pins for GSM Module
SoftwareSerial gsm(10, 11); // RX, TX

// Pin for Relay
const int relayPin = 13;

String* allowedNumbers;
String range;
int startIdx;
int middleIdx;
int endIdx;
int startRange;
int endRange;
int allowedNumbersCount;

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

  // Select SIM phonebook storage
  gsm.println("AT+CPBS=\"SM\"");
  delay(1000); 

  String range = getPBRange();
  Serial.println("Phonebook Range Response: " + range);

  startIdx = getStartIdx( range);
  middleIdx = getMiddleIdx( range, startIdx);
  endIdx = getEndIdx( range, middleIdx);

  startRange = extractStartRange( range, startIdx, middleIdx);
  endRange = extractEndRange( range, middleIdx, endIdx);
  Serial.println("Phonebook Range: " + String(startRange) + " to " + String(endRange));

  // Whitelisted numbers
  allowedNumbersCount = 0; 
  readPhoneBook();

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
        // wait for a response maybe add them
        Serial.println("Unauthorized number. Ignoring the call.");
      }

      // End the call
      gsm.println("ATH");
      delay(1000);
    }
  }
}

// Function to check if the number is in the allowed list
bool isAllowedNumber(String number) {
  for (int i = 0; i < allowedNumbersCount && allowedNumbers[i] != ""; i++) {
    if (allowedNumbers[i] == number) {
      return true;
    }
  }
  return false;
}

// Function to read phonebook entries from SIM
void readPhoneBook(){
  while( allowedNumbers[allowedNumbersCount] != "") {
    String entry = getEntry( allowedNumbersCount);
    if (entry.indexOf("+CPBR:") != -1) {
      Serial.println("Phonebook Entry: " + entry);
      String number = extractPhoneNumber(entry);
      allowedNumbers[allowedNumbersCount++] = number;
      Serial.println("Stored Number: " + number);
    }
  }
}

String getPBRange(){
  gsm.println("AT+CPBR=?");
  delay(1000);
  return getResponse();
}

int getStartIdx(String range){
  return range.indexOf("(") + 1;
}

int getMiddleIdx(String range, int startIdx){
  return range.indexOf("-", startIdx);
}

int getEndIdx(String range, int middleIdx){
  return range.indexOf(")", middleIdx);
}

int extractStartRange( String range, int startIdx, int middleIdx){
  return range.substring(startIdx, middleIdx).toInt();
}

int extractEndRange( String range, int middleIdx, int endIdx){
  return range.substring(middleIdx + 1, endIdx).toInt();
}

String getEntry( int i){
  gsm.print("AT+CPBR=");
  gsm.println(i);
  delay(500);
  return getResponse();
}

String getResponse(){
  String response = "";
  while (gsm.available()) {
    response += gsm.readString();
  }
  return response;
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
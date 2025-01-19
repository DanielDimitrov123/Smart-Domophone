#include <SoftwareSerial.h>
#include <SD.h>
#include <TMRpcm.h>
#include <SPI.h>

SoftwareSerial SIM800L(10, 11); // RX, TX for SIM800L
const int CSpin = 4;       // SD card CS pin
TMRpcm audio;

// Pins for GSM Module
SoftwareSerial gsm(10, 11); // RX, TX

// Pin for Relay
const int relayPin = 7;

String* allowedNumbers;
String range;
int startIdx;
int middleIdx;
int endIdx;
int startRange;
int endRange;
int allowedNumbersCount;

int SCLpin = 13; 
int SDOpin = 12;

byte key;

int trbsCount = 0;

void setup() {
  Serial.begin(9600);     // Debugging
  gsm.begin(9600);        // GSM Module communication
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Ensure the relay is off initially
  pinMode( SCLpin, OUTPUT);
  pinMode( SDOpin, INPUT);

  // Initialize SD card
  while (!Serial);
  Serial.println(F("Initializing SD card..."));
  if (!SD.begin(CSpin)) {
    Serial.println(F("initialization failed. Things to check:"));
    Serial.println(F("1. is a card inserted?"));
    Serial.println(F("2. is your wiring correct?"));
    Serial.println(F("3. did you change the chipSelect pin to match your shield or module?"));
    Serial.println(F("Note: press reset button on the board and reopen this Serial Monitor after fixing your issue!"));
    while (true);
  }
  Serial.println(F("initialization done."));

  // Output audio on pin 9 (PWM)
  audio.speakerPin = 9;

  // Check GSM module
  commandReply( F("AT"), F("Initializing..."), 1000);

  if (gsm.find("OK")) {
    Serial.println(F("GSM Module Connected."));
  } else {
    Serial.println(F("GSM Module Failed to Connect."));
    while (1);
  }

  // Select SIM phonebook storage
  commandReply( F("AT+CPBS=\"SM\""), F("Selecting SIM phonebook storage"), 1000);

  String range = getPBRange();
  Serial.print(F("Phonebook Range Response: "));
  Serial.println( range);

  startIdx = getStartIdx( range);
  middleIdx = getMiddleIdx( range, startIdx);
  endIdx = getEndIdx( range, middleIdx);

  startRange = extractStartRange( range, startIdx, middleIdx);
  endRange = extractEndRange( range, middleIdx, endIdx);
  Serial.print(F("Phonebook Range: "));
  Serial.print(startRange);
  Serial.print( F(" to "));
  Serial.println(endRange);

  // Whitelisted numbers
  allowedNumbersCount = 0; 
  readPhoneBook();

  // Check communication with SIM800L
  commandReply( F("AT"), F("Check connection with sim"), 500);

  // "Ennable full discription"
  commandReply( F("AT+CREG=2"), F("Ennable full discription"), 500);

  // Enable DTMF detection
  commandReply( F("AT+DDET=1"), F("Enables DTMF detection"), 500);

  // Disable call forwarding
  commandReply( F("AT+CCFC=0"), F("Disable call forwarding"), 500);
  
  // Disable call waiting
  commandReply( F("AT+CCWA=0"), F("Disable call waiting"), 500);

  troubleShoot();

  // Enable caller ID notification
  commandReply( F("AT+CLIP=1"), F("Enabling caller ID notification"), 1000);
}

void loop() {
  // Check for incoming call
  if( trbsCount >= 5){
    trbsCount = -1;
    troubleShoot();
  }
  trbsCount++;

  String response = commandReply( F("AT+CLCC"), F("Current Call Status:"), 1000);
  if( response != ""){
    if (response.indexOf("RING") != -1) { // Incoming call detected
      commandReply( F("ATA"), F("Incoming call detected. Answering..."), 1000);
    }

    // Check for DTMF tones
    if (response.indexOf("+DTMF:") != -1) {
      char dtmfTone = response.charAt(response.indexOf("+DTMF:") + 7); // Extract DTMF tone
      Serial.print(F("DTMF Tone Received: "));
      Serial.println(dtmfTone);

      // Perform actions based on DTMF tone
      if (dtmfTone == '1') {
        Serial.println(F("Action for '1': Option X selected."));
      } else if (dtmfTone == '2') {
        Serial.println(F("Action for '2': Option Y selected."));
      } else if (dtmfTone == '3') {
        Serial.println(F("Action for '3': Option Z selected."));
      } else {
        Serial.println(F("Invalid option."));
      }
    }
  }

  key = getKeyStatus();

  if(key){
    Serial.println(key);
  }

  delay(100);

  // Check for incoming data from GSM module
  if (gsm.available()) {
    String response = gsm.readString();
    Serial.println("GSM Response: " + response);

    // Check for an incoming call
    if (response.indexOf("+CLIP:") != -1) {
      String callerNumber = extractPhoneNumber(response);
      Serial.print( F("Incoming call from: "));
      Serial.println( callerNumber);

      // Validate the caller's number
      if (isAllowedNumber(callerNumber)) {
        Serial.println( F("Number authorized. Turning on the relay."));
        digitalWrite(relayPin, HIGH); // Turn on relay
        delay(5000);                 // Keep relay on for 5 seconds
        digitalWrite(relayPin, LOW); // Turn off relay
        Serial.println( F("Relay turned off."));
      } else {
        // wait for a response maybe add them
        Serial.println( F("Unauthorized number. Ignoring the call."));
      }

      // End the call
      commandReply( F("ATH"), F("Ending the call"), 1000);
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
      Serial.print( F("Phonebook Entry: "));
      Serial.println( entry);
      String number = extractPhoneNumber(entry);
      allowedNumbers[allowedNumbersCount++] = number;
      Serial.print( F("Stored Number: "));
      Serial.println(number);
    }
  }
}

String getPBRange(){
  return commandReply( F("AT+CPBR=?"), F("Getting PhoneBook range"), 1000); // maybe use gsm.println("AT+CPBR=?"); delay(1000); return getResponse();
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
  return commandReply( F("AT+CPBR="), "Entry " + String(i), 500); // maybe use get responce
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

byte getKeyStatus() {
  byte Count;
  byte Key = 0;

  for( Count = 1; Count <=16; Count++){
    digitalWrite( SCLpin, LOW);
    if( !digitalRead( SDOpin)){
      Key = Count;
    }
    digitalWrite( SCLpin, HIGH);
  } 
  
  return Key;
}

void playWavFile( String fileName) {
  audio.play( fileName.c_str());
  delay(2000);
}

void logToSD(String log, String fileName) {
  File dataFile = SD.open(fileName, FILE_WRITE);
  if (dataFile) {
    dataFile.println(log);
    dataFile.close();
    Serial.println("Log saved: " + log);
  } else {
    Serial.println("Error writing to log file!");
  }
}

String ReadFromSD( String fileName) {
  File myFile = SD.open(fileName);
  if (myFile) {
    Serial.println(fileName);
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    Serial.println("error opening " + fileName);
  }
}

void troubleShoot() {
  String connectionresponse = commandReply( F("AT"), F("Ensuring connection before throubleshoot"), 500);
  Serial.print( F(" OK "));
  Serial.println(connectionresponse);
  if( connectionresponse.indexOf("OK") != -1){
    Serial.println( F("=== Troubleshooting SIM800L ==="));

    // Check Network Registration
    commandReply( F("AT+CREG?"), F("Network Registration Status:"), 1000);

    // List Supported CREG Options
    commandReply( F("AT+CREG=?"), F("Supported Network Registration Modes:"), 1000);

    // Scan for Available Operators
    Serial.println( F("Scanning for available operators (this may take time)..."));
    commandReply( F("AT+COPS=?"), F("Available Operators:"), 5000);

    // Check Current Operator
    commandReply( F("AT+COPS?"), F("Current Operator:"), 1000);

    // Check Signal Quality
    commandReply( F("AT+CSQ"), F("Signal Quality:"), 1000);

    // List Current Calls
    commandReply( F("AT+CLCC"), F("Current Call Status:"), 1000);

    Serial.println( F("=== Troubleshooting Complete ==="));
  }else{
    Serial.println( F(" SIM800L bad connection"));
  }
}

String commandReply( String ATcommand, String purpose, int delayTime){
  SIM800L.println(ATcommand);
  delay(delayTime);
  if (SIM800L.available()) {
    Serial.println(purpose);
    String response = SIM800L.readString();
    Serial.println(response);
    return response;
  }
}

//String getResponse(){
//  String response = "";
//  while (gsm.available()) {
//    response += gsm.readString();
//  }
//  return response;
//}
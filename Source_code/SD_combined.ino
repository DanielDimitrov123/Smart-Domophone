#include <SoftwareSerial.h>
#include <SD.h>
#include <TMRpcm.h>
#include <SPI.h>

SoftwareSerial SIM800L(10, 11); // RX, TX for SIM800L
const int CSpin = 4;       // SD card CS pin
TMRpcm audio;

void setup() {
  Serial.begin(9600);
  SIM800L.begin(9600);

  // Initialize SD card
  while (!Serial);
  Serial.println("Initializing SD card...");
  if (!SD.begin(CSpin)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset button on the board and reopen this Serial Monitor after fixing your issue!");
    while (true);
  }
  Serial.println("initialization done.");

  audio.speakerPin = 9; // Output audio on pin 9 (PWM)

  // Initialize SIM800L SMS function (SELECT SMS MESSAGE FORMAT)
  SIM800L.println("AT+CMGF=1");
  delay(500);

  // Enable caller ID notification
  SIM800L.println("AT+CLIP=1");
  delay(1000);
}

void loop() {
  // Check for incoming data from GSM module
  if (SIM800L.available()) {
    String response = SIM800L.readString();
    Serial.println("GSM Response: " + response);

    // Check for an incoming call
    if (response.indexOf("+CLIP:") != -1) {
      String callerNumber = extractPhoneNumber(response);
      String message = "Incoming call from: " + callerNumber;
      Serial.println(message);
      logToSD( message, "logs.txt");

      // End the call
      SIM800L.println("ATH");
      delay(1000);
    }
  }
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

// Function to extract phone number from +CPBR response
String extractPhoneNumber(String response) {
  int startIdx = response.indexOf("\"") + 1; // Start of phone number
  int endIdx = response.indexOf("\"", startIdx); // End of phone number

  if (startIdx != -1 && endIdx != -1) {
    return response.substring(startIdx, endIdx);
  }

  return ""; // Return empty if no valid number is found
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

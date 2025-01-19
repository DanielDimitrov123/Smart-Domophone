#include <SoftwareSerial.h>

SoftwareSerial SIM800L(10, 11); // RX, TX pins for SIM800L
int trbsCount = 0;

void setup() {
  Serial.begin(9600);       // Monitor communication
  SIM800L.begin(9600);      // SIM800L communication
  Serial.println("Initializing...");

  // Check communication with SIM800L
  commandReply( "AT", "Check connection with sim", 500);

  // "Ennable full discription"
  commandReply( "AT+CREG=2", "Ennable full discription", 500);

  // Enable DTMF detection
  commandReply( "AT+DDET=1", "Enables DTMF detection", 500);

  // Disable call forwarding
  commandReply( "AT+CCFC=0", "Disable call forwarding", 500);
  
  // Disable call waiting
  commandReply( "AT+CCWA=0", "Disable call waiting", 500);

  troubleShoot();
}

void loop() {
  // Check for incoming call
  if( trbsCount >= 5){
    trbsCount = -1;
    troubleShoot();
  }
  trbsCount++;

  String response = commandReply( "AT+CLCC", "Current Call Status:", 1000);
  if( response != ""){
    if (response.indexOf("RING") != -1) { // Incoming call detected
      Serial.println("Incoming call detected. Answering...");
      SIM800L.println("ATA"); // Answer the call
      delay(1000);
    }

    // Check for DTMF tones
    if (response.indexOf("+DTMF:") != -1) {
      char dtmfTone = response.charAt(response.indexOf("+DTMF:") + 7); // Extract DTMF tone
      Serial.print("DTMF Tone Received: ");
      Serial.println(dtmfTone);

      // Perform actions based on DTMF tone
      if (dtmfTone == '1') {
        Serial.println("Action for '1': Option X selected.");
      } else if (dtmfTone == '2') {
        Serial.println("Action for '2': Option Y selected.");
      } else if (dtmfTone == '3') {
        Serial.println("Action for '3': Option Z selected.");
      } else {
        Serial.println("Invalid option.");
      }
    }
  }
}

void troubleShoot() {
  String connectionresponse = commandReply("AT", "Ensuring connection before throubleshoot", 500);
  Serial.println(" OK " + connectionresponse);
  if( connectionresponse.indexOf("OK") != -1){
    Serial.println("=== Troubleshooting SIM800L ===");

    // Check Network Registration
    commandReply( "AT+CREG?", "Network Registration Status:", 1000);

    // List Supported CREG Options
    commandReply( "AT+CREG=?", "Supported Network Registration Modes:", 1000);

    // Scan for Available Operators
    Serial.println("Scanning for available operators (this may take time)...");
    commandReply( "AT+COPS=?", "Available Operators:", 5000);

    // Check Current Operator
    commandReply( "AT+COPS?", "Current Operator:", 1000);

    // Check Signal Quality
    commandReply( "AT+CSQ", "Signal Quality:", 1000);

    // List Current Calls
    commandReply( "AT+CLCC", "Current Call Status:", 1000);

    Serial.println("=== Troubleshooting Complete ===");
  }else{
    Serial.println(" SIM800L bad connection");
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

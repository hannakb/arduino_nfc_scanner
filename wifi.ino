#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#define PN532_IRQ   9
Adafruit_PN532 nfc(PN532_IRQ, 100);
//Enter here your data
const char wifi_ssid[] = "iPhone Serafim";
const char wifi_password[] = "voxtelqw";

const char server[] = "http://api.thingspeak.com";
const char server_port[] = "80";
char line[] = "update?api_key=DVRDJML808Q0KXOQ&field1=";

int8_t answer;
char response[300];
char aux_str[90];
char num[30];

#define DATA_BUFFER      1024

String inputLine = ""; // a string to hold incoming line

void setup()
{
  //Write here you correct baud rate
  Serial.begin(115200);
  nfc.begin();
  int versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find RFID/NFC reader");
    while(1) {
    }
  }

  //wifireset(); 
  wificonfig(); 

  // reserve DATA_BUFFER bytes for the inputString:
  inputLine.reserve(DATA_BUFFER);
  Serial.println("Found RFID/NFC reader");
      // настраиваем модуль
  nfc.SAMConfig();
  Serial.println("Waiting for a card ...");
}

void loop()
{
  uint8_t success;
  uint8_t uid[16];
  uint8_t uidLength;
  uint32_t datal = 0;
  uint32_t szPos;
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    
    Serial.println("Found a card");
    Serial.print("Card ID Value: ");
    for (szPos=0; szPos < uidLength; szPos++)
    {
      datal = datal + int(uid[szPos]);
    }
    Serial.println(datal);
    char GET[100];
    char bufferk[5];
    strcpy(GET, line);
    
    String str;
    str = String(datal);
    str.toCharArray(bufferk,5);
    
    strcat(GET, bufferk);
    sendGET(GET);
    delay(100);
  }
  //Serial.println(F("Sending HTTP GET"));
  
  
  //delay(1000);
}


//**********************************************
void wifireset() {
  
  Serial.println(F("Setting Wifi parameters"));
  sendCommand("AT+iFD\r","I/OK",2000);
  delay(1000);  
  sendCommand("AT+iDOWN\r","I/OK",2000);
  delay(6000);  
}


//**********************************************
void wificonfig() {
  
  Serial.println(F("Setting Wifi parameters"));
  sendCommand("AT+iFD\r","I/OK",2000);
  delay(2000);  

  snprintf(aux_str, sizeof(aux_str), "AT+iWLSI=%s\r", wifi_ssid);
  answer = sendCommand(aux_str,"I/OK",10000);

  if (answer == 1){

    snprintf(aux_str, sizeof(aux_str), "Joined to \"%s\"", wifi_ssid);
    Serial.println(aux_str);
    delay(5000);
  }

  else {
    snprintf(aux_str, sizeof(aux_str), "Error joining to: \"%s\"", wifi_ssid);
    Serial.println(aux_str);
    delay(1000);
  }

  snprintf(aux_str, sizeof(aux_str), "AT+iWPP0=%s\r", wifi_password);
  sendCommand(aux_str,"I/OK",20000);
  delay(1000);

  if (answer == 1){

    snprintf(aux_str, sizeof(aux_str), "Connected to \"%s\"", wifi_ssid);
    Serial.println(aux_str);
    delay(5000);
  }

  else {
    snprintf(aux_str, sizeof(aux_str), "Error connecting to: \"%s\"", wifi_ssid);
    Serial.println(aux_str);
    delay(1000);
  }

  sendCommand("AT+iDOWN\r","I/OK",2000);
  delay(6000); 


}


//**********************************************
void sendGET(char* GET) {

  snprintf(aux_str, sizeof(aux_str), "AT+iRLNK:\"%s:%s/%s\"\r", server, server_port, GET);
  sendCommand(aux_str,"I/OK",5000); 
  Serial.print("GET response:");
  delay(500);
  handleSerial();
}


//**********************************************
int8_t sendCommand(const char* Command, const char* expected_answer, unsigned int timeout){

  uint8_t x=0,  answer=0;
  unsigned long previous;

  memset(response, 0, 300);    // Initialize the string

  delay(100);

  while( Serial.available() > 0) Serial.read();    // Clean the input buffer

  Serial.println(Command);    // Send Command 

    x = 0;
  previous = millis();

  // this loop waits for the answer
  do{
    if(Serial.available() != 0){    
      // if there are data in the UART input buffer, reads it and checks for the asnwer
      response[x] = Serial.read();
      x++;
      // check if the desired answer  is in the response of the module
      if (strstr(response, expected_answer) != NULL)    
      {
        answer = 1;
      }
    }
  }
  // Waits for the asnwer with time out
  while((answer == 0) && ((millis() - previous) < timeout));    

  return answer;
}


//**********************************************

void handleSerial()
{
  inputLine = "";
  while (Serial.available())
  {
    // get the new byte:
    char inChar = (char)Serial.read();

    // latency delay for 115200 (worst condition)
    delayMicroseconds(105);

    // add it to the inputString:
    inputLine += inChar;

  }
   Serial.print(inputLine);
   inputLine = "";
   delay(100);
} 

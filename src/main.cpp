#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <EEPROM.h>


#define EEPROM_SIZE 8

const char* ssid     = "Internet";
const char* password = "00000000";

const char* botToken = "XXXXXXXXXX:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
const char* chatID   = "000000000";   


const int analog_ip = A0; 
int Adc_Bat  = 0;
int V_Bat  = 0;

#define httpPort      80

#define  rLED         13
#define  gLED         14
#define  bLED         12
#define  WK           16
#define  AC_DETECT    4

const char* udpAddress = "192.168.1.255";
const int udpPort = 5123;
void deepSleepMinutes(int minutes);
int sendHttpRequest(const char* message);

WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  Serial.println("\n\r\n\rRUN..\n\r");
  
  
  //int valueToWrite = 123;
  //EEPROM.write(0, valueToWrite);
  //EEPROM.commit();
  //int valueRead = EEPROM.read(0); 
  
  pinMode(WK, OUTPUT);
  pinMode(AC_DETECT, INPUT);
  digitalWrite(WK, HIGH);
  pinMode(rLED, OUTPUT);
  pinMode(gLED, OUTPUT);
  pinMode(bLED, OUTPUT);

  digitalWrite(rLED, HIGH);
  digitalWrite(gLED, HIGH);
  digitalWrite(bLED, HIGH);
  Adc_Bat = analogRead(analog_ip) * 10.625;

  WiFi.begin(ssid, password);
  

  //while (true){
  //  digitalWrite(rLED, digitalRead(AC_DETECT));
  //  
  //  delay(100);
  //  digitalWrite(bLED, LOW);
  //  delay(20);
  //  digitalWrite(bLED, HIGH);
  // }



  int attempsToConn = 20;
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        digitalWrite(bLED, LOW);
        delay(20);
        digitalWrite(bLED, HIGH);
        Serial.print(". "+ String(attempsToConn));
        attempsToConn--;
        if(attempsToConn ==0){
          deepSleepMinutes(10);
        }
    }

  Serial.println();
  Serial.println("Wi-Fi підключено");
  Serial.print("IP-адреса: ");
  Serial.println(WiFi.localIP());
  


  if (strcmp(chatID, "000000000") != 0) {
    sendHttpRequest(WiFi.localIP().toString().c_str());
  }
  
}

void loop() {
   Serial.println(WiFi.localIP());
   Serial.println(WiFi.status());

  
  char udpMessage[50];
  snprintf(udpMessage, sizeof(udpMessage), "Vbat= %d; %s", Adc_Bat, WiFi.localIP().toString().c_str());
  udp.beginPacket(udpAddress, udpPort);
  udp.write(udpMessage);
  udp.endPacket();
  delay(1500);

  

  deepSleepMinutes(45);

}


void deepSleepMinutes(int minutes) {
    minutes = minutes > 71 ? 71 : minutes;
    minutes = minutes < 1 ? 1 : minutes;
    uint64_t sleepTime = (uint64_t)minutes * 60 * 1000000;
    ESP.deepSleep(sleepTime);
}

int sendHttpRequest(const char* message) {
    int result = 0;
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
        client->setInsecure();
        String url = "https://api.telegram.org/bot" + String(botToken) + "/sendMessage?text=" + String(message) + "&chat_id=" + String(chatID);
        Serial.println(url);
        http.begin(*client, url);
        int httpCode = http.GET();
        if (httpCode > 0) {
            String payload = http.getString();
            result=0;
        } else {
            result=10;
        }
        http.end();
    } else {
        result=11;
    }
    return result;
}
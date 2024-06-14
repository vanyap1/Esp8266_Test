#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>




const char* ssid     = "Internet";
const char* password = "vp198808";
const int analog_ip = A0; 
int Adc_Bat  = 0;
int V_Bat  = 0;


int rLED = 13;
int gLED = 14;
int bLED = 12;
int WK = 16;

const char* udpAddress = "192.168.1.255";
const int udpPort = 5123;
void deepSleepMinutes(int minutes);

WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\r\n\rRUN..\n\r");
  pinMode(WK, OUTPUT);
  digitalWrite(WK, HIGH);
  pinMode(rLED, OUTPUT);
  pinMode(gLED, OUTPUT);
  pinMode(bLED, OUTPUT);

  digitalWrite(rLED, HIGH);
  digitalWrite(gLED, HIGH);
  digitalWrite(bLED, HIGH);
  Adc_Bat = analogRead(analog_ip) * 10.625;

  WiFi.begin(ssid, password);
  
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

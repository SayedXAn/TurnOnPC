#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Future";
const char* password = "Bond$tein23";

unsigned long lastTime = 0;
unsigned long timerDelay = 3000;

int restartTime = 120000;

//String serverName = "https://turn-pc-on.wskoly.xyz/api/node_mcu_get_pc_status";
String serverName = "https://sing-boot-pc.wskoly.xyz/get_pc_status_esp/2/";

//22,21,23,18

//4, 5, 18, 19, 21, 22, 23, 25, 26, 27, 32, and 33

//0, 2, 3
int Relays[8] = {18, 21, 22, 23, 19, 25, 26, 27};
int timeCount = 0;
int pcid[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
void setup(){
  for(int i=0; i<8; i++)
    {
      digitalWrite(Relays[i], HIGH); 
      pinMode(Relays[i], OUTPUT);
      digitalWrite(Relays[i], HIGH); 
    }
    Serial.begin(115200);
    delay(1000);    
    ReConnectWifi();
}


void loop() {
  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status()== WL_CONNECTED){
      Serial.println("Loop a dhukse, API call hobe ekhon");
      HTTPClient http;
      String serverPath = serverName;
      http.begin(serverPath.c_str());
      if(WiFi.status() != WL_CONNECTED)
      {
        Serial.println(WiFi.status());
        ReConnectWifi();
      }
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        parsePCList(payload);
        Serial.println(payload);
      }
      else {
        ResetArray();
        Serial.print("Ekhane theke Error code ashtese: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
      ResetArray(); 
      ReConnectWifi();
    }
    lastTime = millis();
  }
}

void APIHandel()
{
  for(int i=0; i<8; i++)
  {
    if(pcid[i] >= 0)
    {
      TurnOnPC(i);
    }
  }
  ResetArray();  
}

void TurnOnPC(int k)
{
  Serial.print("Turn on a ashche: ");
  Serial.print("Relay On Hocche -- ");
  Serial.println(k);
  digitalWrite(Relays[k], LOW);
  delay(1500);
  digitalWrite(Relays[k], HIGH);
  
}

void ResetArray()
{
  for(int i=0; i<8; i++)
  {
    pcid[i] = -1;
  }
}

void parsePCList(String response) {
  response.trim();
  response = response.substring(1, response.length() - 1); 
  int index = 0;
  int start = 0;
  for (int i = 0; i < response.length(); i++) {
    if (response[i] == ',' || i == response.length() - 1) {
      int recievedPin = response.substring(start, (i == response.length() - 1) ? i + 1 : i).toInt();
      pcid[recievedPin] = 1;
      start = i + 1;
    }
  }

  // Print the parsed list of PCs
  Serial.println("Parsed PCs to turn on:");
  for (int i = 0; i < index; i++) {
    Serial.print(pcid[i]);
    Serial.print(" ");
  }
  APIHandel();
}

void ReConnectWifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Dhoirzo! ");
    Serial.println(timeCount);
    delay(100);
    timeCount += 100;
    if(timeCount > restartTime)
    {
      Serial.print("System Reboot by Sayed");
      RestartDevice();
    }
  }
  timeCount = 0;

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void RestartDevice()
{
  ESP.restart();
}
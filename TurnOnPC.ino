#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Future";
const char* password = "Bond$tein23";

unsigned long lastTime = 0;
unsigned long timerDelay = 3000;

int restartTime = 120000;

String serverName = "https://turn-pc-on.wskoly.xyz/api/node_mcu_get_pc_status";

//GPIOs like 16, 17, 18, 19, 21, 22, 23 are generally safe to use.
int Relays[4] = {22,21,23,18};
int timeCount = 0;
int pcid[4];
void setup(){
  for(int i=0; i<4; i++)
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
      HTTPClient http;
      String serverPath = serverName;
      http.begin(serverPath.c_str());

      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        parsePCList(payload);
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
      ReConnectWifi();
    }
    lastTime = millis();
  }
}

void APIHandel()
{
  for(int i=0; i<4; i++)
  {
    if(pcid[i] > 0)
    {
      TurnOnPC(pcid[i]-1);
    }
  }
  
}

void TurnOnPC(int k)
{
  Serial.print("Turn on a ashche: ");
  Serial.println(k);
  digitalWrite(Relays[k], LOW);
  delay(1500);
  digitalWrite(Relays[k], HIGH);
}

void parsePCList(String response) {
  response.trim();
  response = response.substring(1, response.length() - 1); 
  int index = 0;
  int start = 0;
  for (int i = 0; i < response.length(); i++) {
    if (response[i] == ',' || i == response.length() - 1) {
      pcid[index++] = response.substring(start, (i == response.length() - 1) ? i + 1 : i).toInt();
      start = i + 1;
    }
  }

  // Print the parsed list of PCs
  Serial.println("Parsed PCs to turn on:");
  for (int i = 0; i < index; i++) {
    Serial.print(pcid[i]);
    Serial.print(" ");
  }
  Serial.println();
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
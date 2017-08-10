
#include <GarageServer.h>
#include <OTAService.h>
#include <ESP8266mDNS.h>

//global variables
String BSSID = "esp_garage";
String OTA_PASSWORD = "password";
String Networks[][2] = {
};
GarageServer *server;
void OnEvent(System_Event_t*se);


void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  for(int i=0;i<10;i++){
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
  }
  Serial.println("Reading Chip Id");
  long chipId = ESP.getChipId();
  BSSID = BSSID + "_" + chipId;
  
  Serial.print("Chip Id: ");
  Serial.println(chipId);
  Serial.println("Connecting to network");
  InitServer();
  OTAService.begin(8266,BSSID,OTA_PASSWORD);
  
  //setup mdns responder
  if (!MDNS.begin(BSSID.c_str())) {
    Serial.println("Error setting up MDNS responder!");
  } else {
	  //mdns to let devices know we have a http server
	  MDNS.addService("http", "tcp", 80);
  }
  
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  yield();
  server->HandleClient();
  OTAService.handle();
}

void InitServer() {
  Serial.println("InitServer()");

  server =  new GarageServer();
  server->Initialize(D1,D2);
  
  //register callback events
  Serial.println("registering callback events");
  wifi_set_event_handler_cb(OnEvent);
  
  server->OnConnect = OnConnect;
  server->OnDisconnect = OnDisconnect;
  server->OnClientConnect = OnClientConnect;
  server->OnClientDisconnect = OnClientDisconnect;
  server->OnIpAssigned = OnIpAssigned;

  // connect to network
  Serial.println("connecting to network");
  int i=0;
  int iMax = sizeof Networks / sizeof Networks[0];
  bool connected = false;
  String ssid,psk;
  while(!connected) {
    ssid= Networks[i][0];
    psk= Networks[i][1];
    Serial.println("connecting to \"" + ssid + "\" with psk \"" + psk + "\"");
    connected = server->Connect(ssid,psk,BSSID);
    if(!connected) {
      Serial.println("Failed to connect to " + ssid);
      if(++i >= iMax) {
        ESP.restart();
      }
    }
  }
  Serial.println("connecting to network complete");
  Serial.println("Connected to \"" + ssid + "\" with psk \"" + psk + "\"");

  Serial.println("InitServer() complete");
}

void OnEvent(System_Event_t*se) {
  server->OnEvent(se);
}

void OnConnect() { 
  Serial.println("We are Connected"); 
};
void OnDisconnect() { 
  Serial.println("We have Disconnected"); 
};
void OnClientConnect() { 
  Serial.println("Client Connected"); 
};
void OnClientDisconnect() { 
  Serial.println("Client Disconnected"); 
};
void OnIpAssigned(IPAddress ip){ 
  Serial.println("Local IP Address: " + ipToString(ip));
};


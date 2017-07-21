
#include <GarageServer.h>


//global variables
String BSSID = "WeMOS_Garage";
String Networks[][2] = {
};


GarageServer *server;
void OnEvent(System_Event_t*se);


void setup() {
  Serial.begin(115200);
  Serial.println("");
  InitServer();
  
}

void loop() {
  delay(10);
  server->HandleClient();
}

void InitServer() {
  Serial.println("InitServer()");

  server =  new GarageServer();
  server->Initialize(D1);
  
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
        i=0;
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


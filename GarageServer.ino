
#include <GarageServer.h>
#include <WiFiUdp.h>

//global variables
String BSSID = "Garage";
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
  broadcast();
  multicast();
}

void InitServer() {
  Serial.println("InitServer()");

  server =  new GarageServer();
  server->Initialize(D1,D4);
  
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

//Check if header is present and correct
/*
bool is_authenticated(){
  Serial.println("Enter is_authenticated");
  if (server.hasHeader("Cookie")){
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authenticated Successful");
      return true;
    }
  }
  Serial.println("Authenticated Failed");
  return false;
}
*/
WiFiUDP udp;
long lastBroadcastTime = 0;
long broadcastDelay = 20000;
void broadcast() {
  long currentTime = millis();
  if(lastBroadcastTime + broadcastDelay >= currentTime) {
    return;
  }
  lastBroadcastTime = currentTime;
  
  Serial.println("Broadcasting Message");
  IPAddress ipBroadCast(192, 168, 1, 255);
  unsigned int udpRemotePort=2391;
  unsigned int udplocalPort=2390;
  char udpBuffer[96];
  String message =  "{\"name\": \"Garage Server\",\"ip\": \"" + ipToString(server->LocalIP) + "\"}" ; 
  
  strcpy(udpBuffer, message.c_str()); 
  udp.beginPacket(ipBroadCast, udpRemotePort);
  udp.write(udpBuffer, sizeof(udpBuffer));
  udp.endPacket();
  Serial.println("Broadcast Complete");
}

long lastMulticastTime = 0;
int multicastDelay = 10000;
void multicast() {
  long currentTime = millis();
  if(lastMulticastTime + multicastDelay >= currentTime) {
    return;
  }
  lastMulticastTime = currentTime;
  
  Serial.println("Sending Multicast Message");
  IPAddress ipMulticast(224, 0, 0, 250);
  unsigned int port = 8266;
  String message =  "{\"device\": \"ESP8266\",\"name\": \"Garage Server\",\"ip\": \"" + ipToString(server->LocalIP) + "\"}" ; 
  
  udp.beginPacket(ipMulticast, port);
  udp.write(message);
  udp.endPacket();
  
  Serial.println("Multicast Complete");
}

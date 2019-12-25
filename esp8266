#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>
#include "FS.h"

//for LED status
#include <ArduinoJson.h>
#include <Ticker.h>
Ticker ticker;

const char* mqtt_server = "sportworldcup.com";
String mqqtUserName = "wekex";
String mqqtPassword = "Wekex@mqtt";
int port = 1883;

long lastMsg = 0;
char msg[50];
int value = 0;

String pChannel = "";
String sChannel = "";

WiFiClient espClient;
PubSubClient client(espClient);

int pinD0 = 16; 
int pinD1 = 5; 
int pinD2 = 4; 
int pinD3 = 0;
int pinD4 = 2;//IR Led
int pinD5 = 14;
int pinD6 = 12;
int pinD7 = 13;//Dimmer Sync
int pinD8 = 15;//Dimmer Control 
int pinA0 = 17; 


StaticJsonDocument<512> createJSON;

 void makeJson(int id, JsonObject obj){
  
       createJSON["data"][id] = obj;
       //serializeJsonPretty(createJSON, Serial);

        if(saveConfig())
          Serial.println("Saved");
        else
          Serial.println("Failed");
          currentChange(id,obj);
}

void currentChange(int id, JsonObject obj){
  StaticJsonDocument<225> resData;
   resData["mac"] = macAds();
   resData["time"] = 13518;
   resData["id"] = id;
   resData["ip"] = myIP();
   resData["cdata"] = obj;
   char buffer[1000];
  
  size_t n = serializeJson(resData, buffer);
  if(client.publish(pChannel.c_str(), buffer,1)){
    Serial.println("published");
  }
  delay(1000);
    

  // client.publish("outTopic", "hey ");

}

bool saveConfig(){
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
 }
 
  serializeJson(createJSON, configFile);
  return true;
}

bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
 }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, buf.get());

   
  JsonObject obj = doc.as<JsonObject>();
  if(!obj.containsKey("data")){
    return false;
  }
  
  createJSON = doc;
  JsonArray array = doc["data"];
  
    for(JsonObject obj : array) {
       command(obj,false);
    }
    
  return true;
}

void  makeJsonBegin(){
 
   JsonArray data = createJSON.createNestedArray("data");
   for(int i=0;i<10;i++){
    JsonObject object = data.createNestedObject();
    object["p"] = i;
    object["s"] = 0;
     //data.add(48.756080+1);
    }
   serializeJsonPretty(createJSON, Serial);
}

void deviceStatus(){
   createJSON["mac"] = macAds();
   createJSON["time"] = 13518;
   createJSON["ip"] = myIP();
   createJSON["dtype"] = 1;
   createJSON["version"] = 1;
  char buffer[1000];
  
  size_t n = serializeJson(createJSON, buffer);
  if(client.publish(pChannel.c_str(), buffer,1)){
    Serial.println("published");
  }
  delay(1000);
    

  // client.publish("outTopic", "hey ");

}

String myIP(){
  return WiFi.localIP().toString();
}

void executeIt(int pin, int cstate){
    Serial.print(pin);
    Serial.print(" ");
   Serial.println(cstate);
  if (cstate == 0) {
    digitalWrite(pin, LOW);  
  } else {
    digitalWrite(pin, HIGH);  
  }
}

void controlIt(int pin, int cstate){
    Serial.print(pin);
    Serial.print(" ");
   Serial.println(cstate);
//   light.setBrightness(cstate);
  
}

void controlIR(int pin, int cstate){
    Serial.print(pin);
    Serial.print(" ");
    Serial.println(cstate);
    String myString = String(cstate);
      if (cstate != 0) {
          uint32_t code = strtoul(myString.c_str(), NULL, 10);
//          irsend.sendNEC(code, 32);
      }
}

////For Connectiona and Callback////////
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  //Serial.println(value);
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload,length); 
  JsonObject obj = doc.as<JsonObject>();
  if(obj.containsKey("status"))
    deviceStatus();
  else
    command(obj,true);

}

void command(JsonObject obj,bool make){
   int pin = obj[String("p")];
   int cstate = obj[String("s")];
   Serial.println(pin);
   Serial.println(cstate);

  switch(pin){
    case 0:   executeIt(pinD0, cstate);break;
    case 1:   executeIt(pinD1, cstate);break;
    case 2:   executeIt(pinD2, cstate);break;
    case 3:   executeIt(pinD3, cstate);break;
    case 4:   executeIt(pinD4, cstate);break;
    case 5:   executeIt(pinD5, cstate);break;
    case 6:   executeIt(pinD6, cstate);break;
   // case 7:   executeIt(pinD7, cstate);break;
    case 8:   controlIt(pinD8, cstate);break;
    case 9:   controlIt(pinA0, cstate);break;
  }
  if(make)
    makeJson(pin,obj);

}

void tick(){
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

String macAds(){
    String str = WiFi.macAddress();
    int str_len = str.length() + 1; 
    char char_array[str_len];
    str.toCharArray(char_array, str_len);
  
    String mac = "";
    for(int i=0;i<str_len;i++){
      if(char_array[i] == ':')continue;
      mac += char_array[i];
    }
    pChannel = "P-"+mac;
    sChannel = "D-"+mac;
    return mac;

}

  void reconnect() {
    macAds();
  // Loop until we're reconnected

  
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection..."+pChannel);
    // Create a random client ID
    String clientId = macAds();
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //client.setWill(pChannel.c_str(),"offline");
   StaticJsonDocument<225> resData;
   resData["mac"] = macAds();
   resData["status"] = false;
   char buffer[225];
  
  size_t n = serializeJson(resData, buffer);
  //,pChannel.c_str(),0,1,buffer,true
    if (client.connect(clientId.c_str(),"wekex","Wekex@mqtt")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish(pChannel.c_str(), "hello world");
      deviceStatus();
      // ... and resubscribe
      Serial.println(sChannel);
      Serial.println(pChannel);
      client.subscribe(sChannel.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

void setUpConfig(){
        Serial.println("Mounting FS...");
    
      if (!SPIFFS.begin()) {
        Serial.println("Failed to mount file system");
        return;
      }
    
    if (!loadConfig()) {
        makeJsonBegin();
        Serial.println("Failed to load config");
      } else {
        Serial.println("Config loaded");
      }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(BUILTIN_LED, OUTPUT);
  ticker.attach(0.6, tick);
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }
  setUpConfig();
  Serial.println("connected...yeey :)");
  ticker.detach();
  //keep LED on
  digitalWrite(BUILTIN_LED, LOW);
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
    reconnect();
  }
  client.loop();
//
//  long now = millis();
//  if (now - lastMsg > 2000) {
//    lastMsg = now;
//    ++value;
//    snprintf (msg, 50, "hello world #%ld", value);
//    Serial.print("Publish message: ");
//    Serial.println(msg);
//    Serial.println(pChannel);
//    client.publish(pChannel.c_str(), msg);
//  }
}

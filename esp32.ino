#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#include <ArduinoJson.h>
#include "FS.h"
#include <PubSubClient.h>
#include "SPIFFS.h"
#include <time.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

WebServer server(80);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
String timeStamp = "NA";

const char* ssid;
const char* password;

const char *Hssid = "Home Automation : 16";
const char *Hpassword = "admin123";

const char* mqtt_server = "sportworldcup.com";
String mqqtUserName = "wekex";
String mqqtPassword = "Wekex@mqtt";
int port = 1883;

//Mqtt CLient
long lastMsg = 0;
char msg[50];
int value = 0;

String pChannel = "";
String sChannel = "";

WiFiClient espClient;
PubSubClient client(espClient);

StaticJsonDocument<512> createJSON;
StaticJsonDocument<512> CONFIGS;

//variable_type variable_name = GPIx
int pinD0 = 15; 
int pinD1 = 2; // Inbuilt Blue Led
int pinD2 = 4; 
int pinD3 = 16;
int pinD4 = 17;//IR Led
int pinD5 = 5;
int pinD6 = 18;
int pinD7 = 19;//Dimmer Sync
int pinD8 = 22;//Dimmer Control 
int pinD9 = 3; //Rx
int pinD10 = 1; //Tx
int pinD11 = 22; 
int pinD12 = 23; 
int pinD13 = 13; 
int pinD14 = 12; 
int pinD15 = 14; 
int pinD16 = 27; 
int pinD17 = 26;
int pinD18 = 26; 
int pinD19 = 25; 
int pinD20 = 33; 
int pinD21 = 32; 
int pinD22 = 35;

void setUpWifi();

void initPins(){
  pinMode(pinD0, OUTPUT); 
  digitalWrite(pinD0, HIGH);
  pinMode(pinD1, OUTPUT); 
  digitalWrite(pinD1, HIGH);
  pinMode(pinD2, OUTPUT); 
  digitalWrite(pinD2, HIGH);
  pinMode(pinD3, OUTPUT); 
  digitalWrite(pinD3, HIGH);
  pinMode(pinD4, OUTPUT); 
  digitalWrite(pinD4, HIGH);
  pinMode(pinD5, OUTPUT); 
  digitalWrite(pinD5, HIGH);
  pinMode(pinD6, OUTPUT); 
  digitalWrite(pinD6, HIGH);
  pinMode(pinD7, OUTPUT); 
  digitalWrite(pinD7, HIGH);
  pinMode(pinD8, OUTPUT);
  digitalWrite(pinD8, HIGH);
  pinMode(pinD9, OUTPUT);  
  digitalWrite(pinD9, HIGH);
  pinMode(pinD9, OUTPUT);  
  digitalWrite(pinD9, HIGH);
  pinMode(pinD10, OUTPUT);  
  digitalWrite(pinD10, HIGH);
  pinMode(pinD11, OUTPUT);  
  digitalWrite(pinD11, HIGH);
  pinMode(pinD12, OUTPUT);  
  digitalWrite(pinD12, HIGH);
  pinMode(pinD13, OUTPUT);  
  digitalWrite(pinD13, HIGH);
  pinMode(pinD14, OUTPUT);  
  digitalWrite(pinD14, HIGH);
  pinMode(pinD15, OUTPUT);  
  digitalWrite(pinD15, HIGH);
  pinMode(pinD16, OUTPUT);  
  digitalWrite(pinD16, HIGH);
  pinMode(pinD17, OUTPUT);  
  digitalWrite(pinD17, HIGH);
  pinMode(pinD18, OUTPUT);  
  digitalWrite(pinD18, HIGH);
  pinMode(pinD19, OUTPUT);  
  digitalWrite(pinD19, HIGH);
  pinMode(pinD20, OUTPUT);  
  digitalWrite(pinD20, HIGH);
  pinMode(pinD21, OUTPUT);  
  digitalWrite(pinD21, HIGH);
  pinMode(pinD22, OUTPUT);  
  digitalWrite(pinD22, HIGH);
}



String head="<!DOCTYPE html> <html> <head> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <style> body { margin: 0; font-family: Arial, Helvetica, sans-serif;   } .navbar { overflow: hidden; background-color: #333; position: fixed; bottom: 0; width: 100%; } .navbar a { float: left; display: block; color: #f2f2f2; text-align: center; padding: 14px 16px; text-decoration: none; font-size: 17px; } .navbar a:hover { background: #f1f1f1; color: black; } .navbar a.active,.btn { background-color: #4CAF50; color: white; } .main { padding: 16px; margin-bottom: 30px; } </style> </head> <body>";
String middleauth ="<div class=\"main\"> <center><h1>Authentication</h1> <hr> <form> <br> <form action='/login' method='POST'> User Name : <br> <input type=\"text\" name=\"USERNAME\" > <br> <br> Password:<br> <input type=\"text\" name=\"PASSWORD\"> <br><br> <input type=\"submit\" class=\"btn\" value=\"Submit\"> </form> </center> </div>";
String htmlend ="</body> </html>";
String wifiConfig ="<div class=\"main\"> <center><h1>Wifi Configuration</h1></center> <br> <hr><form action='/configwifi' method='post'> SSID : <br> <input type=\"text\" name=\"ssid\" placeholder='SSID' > <br> <br> Password:<br> <input type=\"text\" name=\"password\" placeholder='Password' > <br><br> <input type=\"submit\" class=\"btn\" value=\"Submit\"> </form> </div>";

//Check if header is present and correct
bool is_authenticated() {
  Serial.println("Enter is_authenticated");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentication Successful");
      return true;
    }
  }
  Serial.println("Authentication Failed");
  return false;
}

//login page, also called for disconnect
void handleLogin() {
  String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == "admin") {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
      return;
    }
    msg = "Wrong username/password! try again.";
    Serial.println("Log in Failed");
  }

  String content = head+middleauth+htmlend;
  server.send(200, "text/html", content);
}

String getNavBar(String type){
   String nav ="<div class=\"navbar\"> <a href=\"/home\" class=\"active\" >HOME</a> <a href=\"/wifi\">WIFI</a> <a href=\"/advance\">ADVANCE</a><a href=\"/login?DISCONNECT=YES\">LOGOUT</a> </div>";
   if(type == "wifi")
    nav ="<div class=\"navbar\"> <a href=\"/home\" >HOME</a> <a href=\"/wifi\" class=\"active\">WIFI</a> <a href=\"/advance\">ADVANCE</a><a href=\"/login?DISCONNECT=YES\">LOGOUT</a> </div>";
   if(type == "advance")
    nav ="<div class=\"navbar\">  <a href=\"/home\" >HOME</a> <a href=\"/wifi\" >WIFI</a> <a href=\"/advance\" class=\"active\">ADVANCE</a><a href=\"/login?DISCONNECT=YES\">LOGOUT</a> </div>";
  
  return nav;
}

void resetRoot(){
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
}

void handleRoot() {
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authenticated()) {
    resetRoot();
    return;
  }
  String nav = getNavBar("home");
 
 String nv = head +nav+ htmlend; 
 
  String content;
  if (server.hasHeader("User-Agent")) {
    content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
  }
  content += "You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>";
  server.send(200, "text/html", nv);
}

void handleWifi(){
  
  Serial.println("Enter handleWifi");
  String header;
  if (!is_authenticated()) {
    resetRoot();
    return;
  }
  String nav = getNavBar("wifi");
 
 String nv = head +nav+ wifiConfig +htmlend; 
  String content;
  if (server.hasHeader("User-Agent")) {
    content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
  }
  content += "You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>";
  server.send(200, "text/html", nv);
}

void saveWifi(){
  Serial.println("Enter handleWifi");
  String header;
  if (!is_authenticated()) {
    resetRoot();
    return;
  }
  String data = "No";
  if (server.hasArg("ssid") && server.hasArg("password")) {
       data = server.arg("ssid")+" "+server.arg("password");
       
       CONFIGS["ssid"] = server.arg("ssid");
       CONFIGS["password"] = server.arg("password");
       serializeJson(CONFIGS, Serial);
       saveDeviceConfig();
   }
 String nav = getNavBar("wifi");
 
 String nv = head + nav + wifiConfig + htmlend; 
 server.send(200, "text/html", nv);
}

bool saveDeviceConfig() {
  File configFile = SPIFFS.open("/network.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }
  serializeJson(CONFIGS, configFile);
  
  serializeJson(CONFIGS, Serial);
  return true;
}

bool loadDeviceConfig() {
  Serial.println("ddshfn");
  File configFile = SPIFFS.open("/network.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
      setUpWifi(true);
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
      setUpWifi(true);
   return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
  auto error = deserializeJson(CONFIGS, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    setUpWifi(true);
    return false;
  }

  ssid = CONFIGS["ssid"];
  password = CONFIGS["password"];

  // Real world application would store these values in some variables for
  // later use.
  Serial.print("Loaded serverName: ");
  Serial.println(ssid);
  Serial.print("Loaded accessToken: ");
  Serial.println(password);
  setUpWifi(false);
  return true;
}

void handleAdvance(){
  Serial.println("Enter handleWifi");
  String header;
  if (!is_authenticated()) {
    resetRoot();
    return;
  }
  String nav = getNavBar("advance");
 
 String nv = head +nav+ wifiConfig +htmlend; 
  String content;
  if (server.hasHeader("User-Agent")) {
    content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
  }
  content += "You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>";
  server.send(200, "text/html", nv);
}
//no need authentication
void handleReset(){
  
}

void handleNotFound() {

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void log(String log){
  Serial.println(log);
}
  
void initFunctions(){
  log("Mounting FS...");
  if (!SPIFFS.begin(true)) {
    log("Failed to mount file system");
  }else{
    log("Mounted");
       loadDeviceConfig();
    if (!loadConfig()) {
        makeJsonBegin();
        Serial.println("Failed to load config");
      } else {
        Serial.println("Config loaded");
      }
  }
  
}

void routes(){
  
}

void setUpWifi(bool hotspot){
  WiFi.mode(WIFI_AP_STA);
  if(!hotspot){
  log("Ssid");
 // String ss = String(ssid);
  log(sizeof(&ssid));
  log("Password");
  log(password);
//  if(sizeof(ssid) > 2 && sizeof(password) > 5 ){
    WiFi.begin(ssid,  password);
    Serial.println("");
   
    // Wait for connection
    int t = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      if(t==20){
        t =0;
        hotspot = true;
        break;
      }
      t++;
    }
//  }else{
//     hotspot = true;
//  }

  }

  if(!hotspot){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    client.setServer(mqtt_server, port);
    client.setCallback(callback);
    timeClient.begin();
    timeClient.setTimeOffset(0);
  }else{
    Serial.print("Configuring access point...");
    WiFi.softAP(Hssid, Hpassword);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    
  }


  server.on("/", handleRoot);
  server.on("/home", handleRoot);
  server.on("/login", handleLogin);
  server.on("/wifi", handleWifi);
  server.on("/configwifi", saveWifi);
  server.on("/advance", handleAdvance);
  server.on("/reset", handleReset);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works without need of authentication");
  });

  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);
  server.begin();
  Serial.println("HTTP server started");
}

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
   resData["time"] = timeStamp;
   resData["id"] = id;
   resData["ip"] = myIP();
   resData["cdata"] = obj;
   char buffer[1000];
  
  size_t n = serializeJson(resData, buffer);
  if(client.publish(pChannel.c_str(), buffer,1)){
    Serial.println("published");
  }
  delay(1000);
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
   for(int i=0;i<22;i++){
    JsonObject object = data.createNestedObject();
    object["p"] = i;
    object["s"] = 1;
     //data.add(48.756080+1);
    }
   serializeJsonPretty(createJSON, Serial);
}

void deviceStatus(){
  
   StaticJsonDocument<225> resData;
   resData["mac"] = macAds();
   resData["status"] = false;
   resData["version"] = 1;
   resData["dtype"] = 16;
   resData["version"] = 1;
   resData["time"] = timeStamp;
   
   char buffer[225];
  
  size_t n = serializeJson(resData, buffer);
  
  serializeJson(createJSON, Serial);
  if(client.publish(pChannel.c_str(), buffer,n)){
    Serial.println("published");
  }
  delay(1000);

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
    case 7:   executeIt(pinD7, cstate);break;
    case 8:   executeIt(pinD8, cstate);break;
    case 9:   executeIt(pinD9, cstate);break;
    case 10:   executeIt(pinD10, cstate);break;
    case 11:   executeIt(pinD11, cstate);break;
    case 12:   executeIt(pinD12, cstate);break;
    case 13:   executeIt(pinD13, cstate);break;
    case 14:   executeIt(pinD14, cstate);break;
    case 15:   executeIt(pinD15, cstate);break;
    case 16:   executeIt(pinD16, cstate);break;
    case 17:   executeIt(pinD17, cstate);break;
    case 18:   executeIt(pinD18, cstate);break;
    case 19:   executeIt(pinD19, cstate);break;
    case 20:   executeIt(pinD20, cstate);break;
    case 21:   executeIt(pinD21, cstate);break;
    case 22:   executeIt(pinD22, cstate);break;
  }
  if(make)
    makeJson(pin,obj);

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
  
   StaticJsonDocument<225> resData;
   resData["mac"] = macAds();
   resData["status"] = false;
   char buffer[225];
  
  size_t n = serializeJson(resData, buffer);
  
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection... "+pChannel);
    // Create a random client ID
    String clientId = macAds();
    clientId += String(random(0xffff), HEX);
    // Attempt to connect

  //,pChannel.c_str(),0,1,buffer,true
    if (client.connect(clientId.c_str(),"wekex","Wekex@mqtt",pChannel.c_str(),0,0,buffer,true)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      
      //client.publish(pChannel.c_str(), "{"status":"true"}");
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

void setup(void) {
  Serial.begin(115200);
  initFunctions();
  initPins();
  
}

void loop(void) {
  server.handleClient();
  if(WiFi.status() == WL_CONNECTED){
    
    if (!client.connected()) {
      reconnect();
    }
    while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

  timeStamp = timeClient.getFullFormattedTime();
  }
  client.loop();
  
}

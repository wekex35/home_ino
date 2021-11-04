#include <ESP8266WiFi.h>
#include <Espalexa.h>

Espalexa espalexa;

// WiFi Credentials
const char* ssid = "Your SSIS";
const char* password = "Your Password";
boolean wifiConnected = false;

//
#define VccPin 17 //A0 => VCC

#define RelayPin1 16 //D0  
#define RelayPin2 14 //D5 
#define RelayPin3 12 //D6 
#define RelayPin4 13 //D7 //Dimmer Sync

#define GNDPin 15 //D8 => GND



//#define RelayPin0 16 //D0
//#define RelayPin1 5 //D1
//#define RelayPin2 4 //D2
//#define RelayPin3 0 //D3
//#define RelayPin4 14 //D5
//#define RelayPin5 12 //D6
//#define RelayPin6 17 //A0

//#define RelayPin7  13; //D7 //Dimmer Sync
//#define RelayPin8  15; //D8 //Dimmer Control
//#define RelayPin9 2; //D4 //IR Led
#define wifiLed 2   //D2

String Device_1_Name = "MainLight";
String Device_2_Name = "TubeLight";
String Device_3_Name = "FAN";
String Device_4_Name = "Combo Light";

String Device_5_Name = "PinkLight 1";
String Device_6_Name = "PinkLight 2";
String Device_7_Name = "Fan";

// prototypes
boolean connectWifi();

boolean connectWifi()
{
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {

    digitalWrite(wifiLed, HIGH); //Turn off WiFi LED
    delay(500);
    digitalWrite(wifiLed, LOW);
    delay(500);
    Serial.print(".");
    if (i > 20) {
      state = false; break;
    }
    i++;
  }
  Serial.println("");
  if (state) {
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("Connection failed.");
  }
  return state;
}

//callback functions
void firstLightChanged(uint8_t brightness);
void secondLightChanged(uint8_t brightness);
void thirdLightChanged(uint8_t brightness);
void fourthLightChanged(uint8_t brightness);

//void fifthLightChanged(uint8_t brightness);
//void sixthLightChanged(uint8_t brightness);
//void senventhLightChanged(uint8_t brightness);

//our callback functions
void firstLightChanged(uint8_t brightness)
{
  //Control the device
  if (brightness == 255)
  {
    digitalWrite(RelayPin1, LOW);
    Serial.println("Device1 ON");
  }
  else
  {
    digitalWrite(RelayPin1, HIGH);
    Serial.println("Device1 OFF");
  }
}

void secondLightChanged(uint8_t brightness)
{
  //Control the device
  if (brightness == 255)
  {
    digitalWrite(RelayPin2, LOW);
    Serial.println("Device2 ON");
  }
  else
  {
    digitalWrite(RelayPin2, HIGH);
    Serial.println("Device2 OFF");
  }
}

void thirdLightChanged(uint8_t brightness)
{
  //Control the device
  if (brightness == 255)
  {
    digitalWrite(RelayPin3, LOW);
    Serial.println("Device3 ON");
  }
  else
  {
    digitalWrite(RelayPin3, HIGH);
    Serial.println("Device3 OFF");
  }
}

void fourthLightChanged(uint8_t brightness)
{
  //Control the device
  if (brightness == 255)
  {
    digitalWrite(RelayPin4, LOW);
    Serial.println("Device4 ON");
  }
  else
  {
    digitalWrite(RelayPin4, HIGH);
    Serial.println("Device4 OFF");
  }
}





void addDevices() {
  // Define your devices here.
  espalexa.addDevice(Device_1_Name, firstLightChanged); //simplest definition, default state off
  espalexa.addDevice(Device_2_Name, secondLightChanged);
  espalexa.addDevice(Device_3_Name, thirdLightChanged);
  espalexa.addDevice(Device_4_Name, fourthLightChanged);
  // espalexa.addDevice(Device_5_Name, fifthLightChanged);
  // espalexa.addDevice(Device_6_Name, sixthLightChanged);
  // espalexa.addDevice(Device_7_Name, seventhLightChanged);

  espalexa.begin();
}



void setup()
{
  Serial.begin(115200);

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);
  //  pinMode(RelayPin5, OUTPUT);
  //  pinMode(RelayPin6, OUTPUT);
  pinMode(VccPin, OUTPUT);

  pinMode(wifiLed, OUTPUT);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, HIGH);
  digitalWrite(RelayPin2, HIGH);
  digitalWrite(RelayPin3, HIGH);
  digitalWrite(RelayPin4, HIGH);
  //  digitalWrite(RelayPin5, HIGH);
  //  digitalWrite(RelayPin6, HIGH);
  //  digitalWrite(RelayPin0, HIGH);
  digitalWrite(VccPin, HIGH);


  // Initialise wifi connection
  wifiConnected = connectWifi();

  if (wifiConnected)
  {
    addDevices();
  }
  else
  {
    Serial.println("Cannot connect to WiFi. So in Manual Mode");
    delay(1000);
  }
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    //Serial.print("WiFi Not Connected ");
    digitalWrite(wifiLed, HIGH); //Turn off WiFi LED
  }
  else
  {
    //Serial.print("WiFi Connected  ");
    digitalWrite(wifiLed, LOW);
    //Manual Switch Control
    //WiFi Control
    if (wifiConnected) {
      espalexa.loop();
      delay(1);
    }
    else {
      wifiConnected = connectWifi(); // Initialise wifi connection
      if (wifiConnected) {
        addDevices();
      }
    }
  }
}
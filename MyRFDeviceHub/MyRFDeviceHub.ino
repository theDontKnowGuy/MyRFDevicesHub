/*
 * MyRFDevicesHub  by theDontKnowGuy
 * 
 * MyRFDevicesHub aims to collect all of your old 
 * 
 */


int DEBUGLEVEL = 1;
//////////////////////////Define Here New Device///////////////////////////////////////////////////////
typedef struct { String Name; String location; String ID; String type; int messasgeLength; int protocol;} RFDevices;
#define NoOfDevices 5
RFDevices Devices[] =         {{"Salon Curtain Motion Detector", "SalonDoor",    "0xcfee1","PIR",        74,1},
                               {"Switch on garden lights",       "GardenLights", "0x8cba3","RemoteBtn",  50,2},
                               {"Switch off garden lights",      "SomeWhere",    "0x7345c","RemoteBtn",  50,2},                               
                               {"Another Motion Detector",       "Somewhere",    "0xc2107","PIR2",       50,3},
                               {"Kitchen termometer",            "Kitchen",      "0x880",  "Termometer", 66,4}};
                               
//////////////////////////Define Here New Protocols Treshold and MessageLength///////////////////////////////////////////////////////
typedef struct {int LongShortTH; int messageLength; int deviceNameLength;}  RFProtocols;
#define NoOfSupportedProtocols 4
RFProtocols SupportedProtocols[] = {{550,   50,   20},
                                    {550,   74,   20},
                                    {1500,  66,   12},
                                    {550,   00,   20}};//// last one if for unknowns. add new ones above this line.

//////////////////////////Define Here Action Parameters ///////////////////////////////////////////////////////

const char* ssid =     "GIDEONI";
const char* password = "qazWSX123";

const int httpsPort = 443;

//////////////////////////Define Here possible actions (for different devices) ///////////////////////////////////////////////////////
typedef struct {String actionType; String actionParam1; String actionParam2; String actionParam3; String successValidator;}  Actions;
#define NoOfDevicesMessageLengths 3
Actions myActions[] = {{"httpPostLocal", "192.168.1.210", "ExternalDeviceName","status","HTTP/1.1 202 ACCEPTED"},
                       {"httpGetSTCloud","graph-na02-useast1.api.smartthings.com", "/api/token/5ef187b3-6598-404f-a189-46e47cda06ab/smartapps/installations/935f9443-08e5-4987-8bb5-25cb320de9b5/execute/",":1af1cf9ca03dc287250433db7ee703ca:","{!result!:!OK!"},
                       {"IFTTT",          "host3",           "ExternalDeviceName","temperature","successValidator"},
                       {"whateverelse",   "host4",           "ExternalDeviceName","status","successValidator"}};

//////////////////////////Define Here associations between devices to the required action. index refers ///////////////////////////////////////////////////////
#define NoOfDeviceActions 4
typedef struct {String deviceType; int actionIdx;}  DevicesAction;
DevicesAction myDevicesActions[] = {{"PIR",       0}, /// <----- this code refers to myActions above
                                    {"RemoteBtn", 1},
                                    {"Termometer",0},
                                    {"PIR2",      0}};

//////////////////////////Define a device runtime status and available capabilties ///////////////////////////////////////////////////////

typedef struct {int idx; String ID; String deviceStatus; int messageLength; int protocolIdx;
                bool IsMove = false; bool IsTamper = false; bool IsLowBat = false; bool IsPing = false; 
                bool IsUnkown = false; int temperature = 999;} RFDevice;

int DeviceMessageLength = -1;

//////////////////////////in high debug level get heapsize and more///////////////////////
extern "C" {
#include "user_interface.h"
}

#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
#define DEBUG_MSG(...)
#endif

#include <RFControl.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#define CollectingCalls 80
int timingsBins[CollectingCalls];
int BinsStream[80];
int MaxValidLengthTH = 2000;
unsigned long LiveSignalPreviousMillis = millis();
int LivePulseLedStatus = 0; 
 bool isDeviceExist;
//////////////////////////Define here control lights //////////////////////
#define red D2
#define yellow D3
#define green D1

void setup() {
    pinMode(red, OUTPUT);pinMode(yellow, OUTPUT);pinMode(green, OUTPUT);               
    digitalWrite(red, HIGH);delay(200); digitalWrite(red, LOW);         

    Serial.begin(115200); Serial.println("\nStarting..."); 
    
    RFControl::startReceiving(D5);    //wemos D1R2 GPIO14-->D5. default casued boot problems.
    
    if (DEBUGLEVEL > 3) {Serial.println("Avail heap mem: " + String(system_get_free_heap_size()));}

    if (initiateNetwork()>0) {networkReset();}
    digitalWrite(green, HIGH); // system running indicator
}
 
void loop() {
   
  if(RFControl::hasData()) {

                int myInxDevice = -1;
                RFDevice myDevice;
                getDeviceDetails(&myDevice);
                getDeviceStatus(&myDevice);
           
                if (isDeviceExist) {
                    AnalyzeDeviceStatus(&myDevice);
                    int r = eventAction(myDevice);}
                else // device not recognized
                    ReportUnkownDevice(myDevice);
                }
  RFControl::continueReceiving();
  blinkLiveLed();
}

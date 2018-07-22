void blinkLiveLed(){
  
  if(millis() - LiveSignalPreviousMillis > 1000){
    digitalWrite(blue,!(LivePulseLedStatus));
    LivePulseLedStatus  = !(LivePulseLedStatus);
    LiveSignalPreviousMillis = millis();
    }
 }

void loadDevicesValues(){

  if(dataUpdateHost=="") return; /// no update. using hardcoded data.
  if(!(dataUpdateHost.startsWith("192", 0)) || (dataUpdateHost.startsWith("10.", 0)))  {
    Serial.println("Placing update file on external server exposes your network to secuiry risks. Ignoring and using local data."); //delete this line to override
    return;
  }
  
  digitalWrite(yellow,HIGH);
   
   WiFiClient client;

   if (DEBUGLEVEL>1) {Serial.print("connecting to reload data to ");Serial.println(String(dataUpdateHost));}  
    
   if (!client.connect(dataUpdateHost, dataUpdatePort)) {
          Serial.println("connecting to reload data failed. Continue with existing data.");
          return;
     }
        
    client.print(String("GET ") + dataUpdateURI + "? HTTP/1.1\r\n" +
               "Host: " + dataUpdateHost + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      if (DEBUGLEVEL>2) Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('^');

//  line.replace(char(34), char(33));
  
  if (DEBUGLEVEL>4) Serial.println(line);
  
  digitalWrite(yellow,LOW);

  //if(DEBUGLEVEL>2) {Serial.println("\nValues from file to parse: "); Serial.println(JSONMessage);}

  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
 
  DynamicJsonBuffer JSONBuffer(capacity);

  //JsonObject& parsed = JSONBuffer.parseObject(JSONMessage);  
  JsonObject& parsed = JSONBuffer.parseObject(line);  
  
  NoOfDevices = (parsed["RFDevices"].size()<MaxNoOfDevices) ? parsed["RFDevices"].size() : MaxNoOfDevices;
  
  if (!parsed.success()) {Serial.println("Parsing failed. Continue with existing data."); return;}
  
  if (DEBUGLEVEL>0) Serial.println("Network configuration loaded and parsed succesfully.");

  for(int i=0; i < NoOfDevices ; i++){
    Devices[i].Name = parsed["RFDevices"][i]["Name"].as<String>();
    Devices[i].location = parsed["RFDevices"][i]["location"].as<String>();
    Devices[i].ID = parsed["RFDevices"][i]["ID"].as<String>();
    Devices[i].type = parsed["RFDevices"][i]["type"].as<String>();
    Devices[i].messasgeLength = parsed["RFDevices"][i]["messasgeLength"].as<int>();
    Devices[i].protocol = parsed["RFDevices"][i]["protocol"].as<int>();
    }
}

String getTimeStamp() {
  return year() + getDigits(month()) + getDigits(day()) + " " + getDigits(hour()) + getDigits(minute()) + getDigits(second());
}

String getDigits(int digits) {
  return (digits < 10) ? "0" + String(digits) : String(digits);
}

void extractTime(String line) {

  if (DEBUGLEVEL > 3) Serial.println(line);

  String strMnt = line.substring(14, 17);
  int intMnt = 0;
  if (strMnt == "Jan") intMnt = 1;
  if (strMnt == "Feb") intMnt = 2;
  if (strMnt == "Mar") intMnt = 3;
  if (strMnt == "Apr") intMnt = 4;
  if (strMnt == "May") intMnt = 5;
  if (strMnt == "Jun") intMnt = 6;
  if (strMnt == "Jul") intMnt = 7;
  if (strMnt == "Aug") intMnt = 8;
  if (strMnt == "Sep") intMnt = 9;
  if (strMnt == "Oct") intMnt = 10;
  if (strMnt == "Nov") intMnt = 11;
  if (strMnt == "Dec") intMnt = 12;


  //setTime(int hr,int min,int sec,int dy, int mnth, int yr)
  // Date: Sun, 15 Jul 2018 18:37:06 GMT
  
  setTime(line.substring(23, 25).toInt(),
          line.substring(26, 28).toInt(),
          line.substring(29, 31).toInt(),
          line.substring(11, 13).toInt(),
          intMnt,
          line.substring(18, 22).toInt());

   if (DEBUGLEVEL>2) Serial.println("\nTime set to (UTC): " + getTimeStamp() );

   if (DEBUGLEVEL>4){
          Serial.println(line.substring(23, 25) + ":" + line.substring(26, 28) + ":" + line.substring(29, 31) + ":" +
          line.substring(11, 13) + ":" + intMnt + ":" + line.substring(18, 22));
   }
}

void boardpanic(){
   Serial.println("Reseting for panic !!!!!!!!!!!!!!!!!!!!!!!!!!!");
   ESP.reset();
}

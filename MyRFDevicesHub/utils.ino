void blinkLiveLed() {

  if (millis() - LiveSignalPreviousMillis > 500) {
    digitalWrite(blue, !(LivePulseLedStatus));
    LivePulseLedStatus  = !(LivePulseLedStatus); 
    totalLifes += 1; 
    LiveSignalPreviousMillis = millis();
  }

  if(logBuffer.length() > 0) if(networklogThis(logBuffer) == 0) logBuffer = ""; 

  if((totalLifes > 60*60*24*2) && (hour()==maintenanceRebootHour)){
    logThis("Rebooting for maintenance...");
    networklogThis(logBuffer);
    ESP.restart();
  }
}

void loadRemoteConfiguration() {

  if (dataUpdateURI == "") return; /// no update. using hardcoded data.
  if (!(strncmp(dataUpdateHost,"192",3)==0) || (strncmp(dataUpdateHost,"10.",3)==0))  {
    logThis("Placing update file on external server exposes your network to security risks. Ignoring and using local data."); //delete this line to override
    return;
  }

  NetworkResponse myNetworkResponse = httpRequest(dataUpdateHost, dataUpdatePort, "GET", dataUpdateURI, "", "}", 0);
  
  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, myNetworkResponse.body);
  
  if (error) {
    logThis("Parsing failed. Continue with existing data.");
    logThis(error.c_str());
    return;
  }
  
  JsonObject root = doc.as<JsonObject>();

  NoOfDevices = (root["RFDevices"].size() < MaxNoOfDevices) ? root["RFDevices"].size() : MaxNoOfDevices;

  int newDebugLevel = root["GeneralConfiguration"]["DEBUGLEVEL"].as<int>();
  if (!(DEBUGLEVEL == newDebugLevel)) {
     logThis("Setting DEBUGLEVEL to " + String(newDebugLevel));
     DEBUGLEVEL = newDebugLevel;
  }
  
  for (int i = 0; i < NoOfDevices ; i++) {
    Devices[i].Name = root["RFDevices"][i]["Name"].as<String>();
    Devices[i].location = root["RFDevices"][i]["location"].as<String>();
    Devices[i].ID = root["RFDevices"][i]["ID"].as<String>();
    Devices[i].type = root["RFDevices"][i]["type"].as<String>();
    Devices[i].messasgeLength = root["RFDevices"][i]["messasgeLength"].as<int>();
    Devices[i].protocol = root["RFDevices"][i]["protocol"].as<int>();
  }
  logThis(1,"Network configuration loaded and parsed succesfully.",2);
}

String getTimeStamp() {
  
  return year() + getDigits(month()) + getDigits(day()) + " " + getDigits(hour()) + getDigits(minute()) + getDigits(second());
}

String getDigits(int digits) {
  return (digits < 10) ? "0" + String(digits) : String(digits);
}

void extractTime(String line) {

  logThis(3,line);

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

  logThis(2,"\nTime set to (UTC): " + getTimeStamp() );

    logThis(4,line.substring(23, 25) + ":" + line.substring(26, 28) + ":" + line.substring(29, 31) + ":" +
                   line.substring(11, 13) + ":" + intMnt + ":" + line.substring(18, 22));
}

void boardpanic() {

  Serial.println("Reseting for panic !!!!!!!!!!!!!!!!!!!!!!!!!!!");
  ESP.restart();

}

void logThis(String strMessage) {
  logThis(0, strMessage, 2);
  return;
}

void logThis(String strMessage, int newLineHint) {
  logThis(0, strMessage, newLineHint);
  return;
}

void logThis(int debuglevel, String strMessage) {
  logThis(debuglevel, strMessage, 2);
  return;
}

void logThis(int debuglevel, String strMessage, int newLineHint) {
  // newLineHint: 0- nothing 1- before 2- after 3- before and after

  if (DEBUGLEVEL < debuglevel) return;

  if (year()>2000) strMessage = getTimeStamp() + " " + strMessage;

  if ((newLineHint == 1) || (newLineHint == 3)) strMessage = "\n" + strMessage;
  if ((newLineHint == 2) || (newLineHint == 3)) strMessage = strMessage + "\n";

  Serial.print(strMessage);

  logBuffer += strMessage;

  return;
}

int networklogThis(String message){

  if (logTarget == "") return 0;   //value empty - network logging off

  message.replace("\n","|");
  message.replace(char(34),char(33));

  NetworkResponse myNetworkResponse = httpRequest(dataUpdateHost, dataUpdatePort, "POST", logTarget, "msg=" + message, "Logged successfully", 0);

  if (!(myNetworkResponse.resultCode == 0)) {
      Serial.println("FAILED LOGGING TO NETWORK");
      digitalWrite(red, HIGH);
      return 1;
  }  
  return 0;
}

int convertBin2Dec (unsigned long num){
    int dec=0, b=1, rem=1;
    while (num > 0)
    {  rem = num % 10;
       dec = dec + rem * b;
       b *= 2;
       num /= 10;
    }
    return dec;
}

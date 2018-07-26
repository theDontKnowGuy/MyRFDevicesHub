void blinkLiveLed() {

  if (millis() - LiveSignalPreviousMillis > 500) {
    digitalWrite(blue, !(LivePulseLedStatus));
    LivePulseLedStatus  = !(LivePulseLedStatus);
    LiveSignalPreviousMillis = millis();
  }
  if (logBuffer == "") return;

  if (networklogThis(logBuffer) == 0) logBuffer = ""; 
}

void loadDevicesValues() {

  if (dataUpdateURI == "") return; /// no update. using hardcoded data.
  if (!(dataUpdateHost.startsWith("192", 0)) || (dataUpdateHost.startsWith("10.", 0)))  {
    logThis("Placing update file on external server exposes your network to security risks. Ignoring and using local data."); //delete this line to override
    return;
  }

  digitalWrite(yellow, HIGH);

  WiFiClient client;
  logThis(1,"connecting to reload data to " + dataUpdateHost,2);

  if (!client.connect(dataUpdateHost, dataUpdatePort)) {
    logThis("connecting to reload data failed. Continue with existing data.",3);
    return;
  }

  client.print(String("GET ") + dataUpdateURI + "? HTTP/1.1\r\n" +
               "Host: " + dataUpdateHost + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      if (DEBUGLEVEL > 2) Serial.println("headers received");
      break;
    }
  }

  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, client);
  if (error) {
    Serial.println("Parsing failed. Continue with existing data.");
    Serial.println(error.c_str());
    return;
  }

  client.stop(); digitalWrite(yellow, LOW);
  
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
  if (DEBUGLEVEL > 0) Serial.println("Network configuration loaded and parsed succesfully.");
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

  if (DEBUGLEVEL > 2) Serial.println("\nTime set to (UTC): " + getTimeStamp() );

  if (DEBUGLEVEL > 4) {
    Serial.println(line.substring(23, 25) + ":" + line.substring(26, 28) + ":" + line.substring(29, 31) + ":" +
                   line.substring(11, 13) + ":" + intMnt + ":" + line.substring(18, 22));
  }
}

void boardpanic() {

  Serial.println("Reseting for panic !!!!!!!!!!!!!!!!!!!!!!!!!!!");
  ESP.reset();
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
  logThis(debuglevel, strMessage, 0);
  return;
}

void logThis(int debuglevel, String strMessage, int newLineHint) {
  // newLineHint: 0 - nothing 1 - before 2- after 3- before and after

  if (DEBUGLEVEL < debuglevel) return;

  String msg = strMessage;
  if ((newLineHint == 1) || (newLineHint == 3)) msg = "\n" + msg;
  if ((newLineHint == 2) || (newLineHint == 3)) msg = msg + "\n";

  Serial.print(msg);

  logBuffer += msg;

  return;
}

int networklogThis(String message){

  if (logTarget == "") return 0;   //value empty - network logging off

  message.replace("\n","|");
  
  int result = httpPostRequest(dataUpdateHost, dataUpdatePort , logTarget,  "msg=" + message , "OK");
  
  if (!(result == 0)) {
      Serial.println("FAILED LOGGIN TO NETWORK");
      digitalWrite(red, HIGH);
  }  
}


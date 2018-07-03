
int initiateNetwork(){
  digitalWrite(yellow,HIGH);
  
  if(DEBUGLEVEL>1) {Serial.print("connecting to ");Serial.println(ssid);}
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if(DEBUGLEVEL>1) {Serial.print(".");}
   }
  
  if (DEBUGLEVEL>1) {Serial.print("WiFi connected. IP address: "); Serial.println(WiFi.localIP());}

 // int r = httpTestRequest();
  digitalWrite(yellow,LOW);

return 0;
}

int httpGetRequest(String host, String postData){
   // Use WiFiClientSecure class to create TLS connection

   WiFiClientSecure client;

   if (DEBUGLEVEL>0) {Serial.print("connecting to ");Serial.println(String(host));}  
    
   if (!client.connect(host, httpsPort)) {
          Serial.println("connection failed");
          digitalWrite(red,HIGH);
          return -1;
     }
     
  if (DEBUGLEVEL>0) {Serial.print("requesting URL: "); Serial.println(postData);}

    client.print(String("GET ") + postData + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      if (DEBUGLEVEL>1) Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('}');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was: \n ==========");
  Serial.println(line);
  Serial.println("========== \n closing connection");
  digitalWrite(yellow,LOW);

}

void networkReset(){
  digitalWrite(red,HIGH); // network problem
  
  if (DEBUGLEVEL>0) {  Serial.println("PERFORMING NETWORK RESET!");}
  delay(1000);
  if (initiateNetwork()==0) {  
    Serial.println("DONE AFTER FIRST ATTEMPT");
    digitalWrite(red,LOW); 
    return;
  }
  delay(120000);
  if (initiateNetwork()==0) {  
    Serial.println("DONE AFTER SECOND ATTEMPT");
    digitalWrite(red,LOW); 
    return;
  }
  boardpanic();
}


void boardpanic(){
   Serial.println("Reseting for panic !!!!!!!!!!!!!!!!!!!!!!!!!!!");
   ESP.reset();
}

int httpPostRequest(String host, String postData){
    WiFiClient client;
    
    int result = 0;
         
    if(DEBUGLEVEL>0) Serial.println(postData);

    if (client.connect(host, 39500)) {     
 
            client.println("POST /posts HTTP/1.1");
            client.print("Host: " + host);
            client.println("Cache-Control: no-cache");
            client.println("Content-Type: application/json");
           // client.println("Content-Type: application/x-www-form-urlencoded");
            client.print("Content-Length: ");
            client.println(postData.length());
            client.println();
            client.println(postData);
          

    
    long interval = 2000;
    unsigned long currentMillis = millis(), previousMillis = millis();
  
    while(!client.available()){
      if( (currentMillis - previousMillis) > interval ){
        Serial.println("Timeout");
        client.stop();
        digitalWrite(red,HIGH);     
        return 1;
      }
      currentMillis = millis();
    }

    while (client.connected())
    { 
      if ( client.available() )
      {char str=client.read();
       Serial.print(str);
       result = 1;}      
    }
    }
 return result; 
}

int httpTestRequest(){//httpPostRequest();
  return 0;}

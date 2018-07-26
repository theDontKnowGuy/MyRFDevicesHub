
int initiateNetwork(){
  int result;
  digitalWrite(yellow,HIGH);
  
  if(DEBUGLEVEL>2) {Serial.print("connecting to ");Serial.println(ssid);}

  long interval = 15000;
  unsigned long currentMillis = millis(), previousMillis = millis();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while ((WiFi.status() != WL_CONNECTED) && ((currentMillis - previousMillis) < interval)) {
    delay(500);
    if(DEBUGLEVEL>1) Serial.print(".");
    currentMillis = millis();
    }
  if (!(currentMillis - previousMillis < interval)){
        Serial.println("Timeout waiting for network");
        digitalWrite(red,HIGH);
        return 1;
      }  

  Serial.print("WiFi connected. IP address: "); Serial.println(WiFi.localIP());
 
  result = httpTestRequest();
  
  if (result==0) {digitalWrite(red,LOW);}
    else
    {digitalWrite(red,HIGH); 
     logThis("Network problem error code" + String(result),2);}
  
  digitalWrite(yellow,LOW); 
     
return result;
}

int httpGetRequest(String host, String URI, String successValidator){
   
   digitalWrite(yellow,HIGH);
   int result;
   WiFiClientSecure client;

   if (DEBUGLEVEL>2) {Serial.print("connecting to ");Serial.println(host);}  
    
   if (!client.connect(host, httpsPort)) {
          Serial.println("connection failed");
          digitalWrite(red,HIGH);
          return 1;
     }
   
  if (DEBUGLEVEL>2) {Serial.print("requesting URI: "); Serial.println(URI);}

    client.print(String("GET ") + URI + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
    

  while (client.connected()) {    
    String line = client.readStringUntil('\n');  
    if (DEBUGLEVEL>4) Serial.println(line);
    if (line.startsWith("Date")) extractTime(line);
    if (line == "\r") {
      if (DEBUGLEVEL>2) Serial.println("headers received");
      break;
    }
  }  

  String line = client.readStringUntil('}');
  
  client.stop();      
  
  line.replace(char(34), char(33));
  if (!(line.indexOf(successValidator) == 1)) {
        if(DEBUGLEVEL>2) Serial.println("Valid Reponse received.");
        digitalWrite(yellow,LOW);
        digitalWrite(red,LOW);  
        return 0;}
    else {Serial.println("Unanticipated Reponse received.");
        if (DEBUGLEVEL>2) Serial.println(line);
        digitalWrite(yellow,LOW);
        digitalWrite(red,HIGH);        
        return 2;}   
}

int httpPostRequest(String host, int port, String URI, String postData, String successValidator){
    WiFiClient client;
    
    int result = 0;
    digitalWrite(yellow,HIGH);
         
    logThis(4,postData,2);

    if (!(client.connect(host, port))) 
    {   logThis("Failed to connect");
        return 1;}
    else
    {       client.println("POST " + URI + " HTTP/1.1");
            client.println("Host: " + host);
            client.println("User-Agent: BuildFailureDetectorESP8266");
            client.println("Cache-Control: no-cache");
          //  client.println("Content-Type: application/json");
            client.println("Content-Type: application/x-www-form-urlencoded;");    
            client.print("Content-Length: ");
            client.println(postData.length());
            client.println();
            client.println(postData);     
                       
    long interval = 8000;
    unsigned long currentMillis = millis(), previousMillis = millis();
  
    while(!client.available()){
      if( (currentMillis - previousMillis) > interval ){
        logThis("Timeout");
        client.stop();
        digitalWrite(red,HIGH);     
        return 1;
      }
      currentMillis = millis();
    }
    }

    String line = ""; 
    char str;
    while (client.connected()) {if ( client.available() ) str = client.read(); line += str;}

    client.stop();
        
    line.replace(char(34), char(33));
    if (!(line.indexOf(successValidator) == -1)) {
            if(DEBUGLEVEL>2) logThis("Valid Reponse received.");
            digitalWrite(red,LOW);
     } else {
            logThis("Unanticipated Reponse received.");
            logThis(line);
            digitalWrite(red,HIGH);
     }
            
  digitalWrite(yellow,LOW);   
    
 return result; 
}

int httpTestRequest(){

  String host = "www.google.com";
  int port = 443;
  String URI = "/";  
  String successValidator = "<!doctype html><html dir";
  
  int result = httpGetRequest(host, URI,successValidator);
  logThis(3,"Result of httpTestRequest: " + String(result));
  return result;
}

void networkReset(){
  digitalWrite(red,HIGH); // network problem
  WiFi.disconnect(); 
  logThis("PERFORMING NETWORK RESET!");
  delay(1000);
  if (initiateNetwork()==0) {  
    logThis("DONE AFTER FIRST ATTEMPT");
    digitalWrite(red,LOW); 
    return;
  }
  delay(120000);
  if (initiateNetwork()==0) {  
    logThis("DONE AFTER SECOND ATTEMPT");
    digitalWrite(red,LOW); 
    return;
  }
  boardpanic();
}




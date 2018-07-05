
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
        networkReset();
      }  
          
  if (DEBUGLEVEL>0) {Serial.print("WiFi connected. IP address: "); Serial.println(WiFi.localIP());}
  
  if (httpTestRequest()==0) {digitalWrite(red,LOW);}
    else
    {digitalWrite(red,HIGH); Serial.println("Network problem error code" + String(result));}
  
  digitalWrite(yellow,LOW); 
     
return result;
}

int httpGetRequest(String host, String URI, String successValidator){
   
   digitalWrite(yellow,HIGH);
   
   WiFiClientSecure client;

   if (DEBUGLEVEL>2) {Serial.print("connecting to ");Serial.println(String(host));}  
    
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
    if (line == "\r") {
      if (DEBUGLEVEL>2) Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('}');
  line.replace(char(34), char(33));
  if (!(line.indexOf(successValidator) == 1)) {
    if(DEBUGLEVEL>2) Serial.println("Valid Reponse received.");
  } else {Serial.println("Unanticipated Reponse received.");}
  
  if (DEBUGLEVEL>2) Serial.println(line);
  
  digitalWrite(yellow,LOW);
}

int httpPostRequest(String host, String postData, String successValidator){
    WiFiClient client;
    
    int result = 0;
    digitalWrite(yellow,HIGH);
         
    if(DEBUGLEVEL>1) Serial.println(postData);

    if (!(client.connect(host, 39500))) 
    {   Serial.println("Failed to connect");
        networkReset();
        httpPostRequest(host, postData, successValidator);}
    else
    {       client.println("POST /posts HTTP/1.1");
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
    }

    String line = ""; 
    char str;
    while (client.connected()) {if ( client.available() ) str = client.read(); line += str;}
    line.replace(char(34), char(33));
    if (!(line.indexOf(successValidator) == -1)) {
            if(DEBUGLEVEL>2) Serial.println("Valid Reponse received.");
            digitalWrite(red,LOW);
     } else {
            Serial.println("Unanticipated Reponse received.");
            Serial.println(line);
            digitalWrite(red,HIGH);
     }
            
  digitalWrite(yellow,LOW);   
    
 return result; 
}

void boardpanic(){
   Serial.println("Reseting for panic !!!!!!!!!!!!!!!!!!!!!!!!!!!");
   ESP.reset();
}

int httpTestRequest(){

  String host = "graph-na02-useast1.api.smartthings.com";
  String URI = "/assets/apps/main/main-app-5811442e0bc96e2d7374677668e7edc4.js";  // just to get a response with a }
  String successValidator = "(function(n,S)";
  
  int result = httpGetRequest(host,URI,successValidator );

  
  return 0;}


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




int initiateNetwork(){
  int result;
  digitalWrite(yellow,HIGH);
  
  logThis(1,"connecting to " + String(ssid),1);

  long interval = 9000;
  unsigned long currentMillis = millis(), previousMillis = millis();
  
  WiFi.begin(ssid, password);
  while ((WiFi.status() != WL_CONNECTED) && ((currentMillis - previousMillis) < interval)) {
    delay(300); if(DEBUGLEVEL>1) Serial.print(".");
    currentMillis = millis();
    }
  if (!(currentMillis - previousMillis < interval)){
        Serial.println("Timeout waiting for network");
        digitalWrite(red,HIGH);
        return 1;
      }  

  Serial.print("\nConneted to Wifi. IP Address: "); Serial.println(WiFi.localIP());

  result = httpTestRequest();
  
  if (result==0) {digitalWrite(red,LOW);}
    else
    {digitalWrite(red,HIGH); 
     logThis("Network problem error code" + String(result),2);}
  
  digitalWrite(yellow,LOW); 
     
return result;
}

int httpGetRequest(char* host, String URI, String successValidator){
   
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
  return 0;  

 String line;
  while (client.connected()) {line += client.readStringUntil('~');}   
  int idxBodyStart = line.indexOf("\r\n\r\n") + 4; 

  if (DEBUGLEVEL>4) Serial.println(line);

  if (line.indexOf("Date: ")>0) extractTime(line.substring(line.indexOf("Date: "),line.indexOf("Date: ")+37));
  
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

int httpPostRequest(char* host, int port, String URI, String postData, String successValidator){

    WiFiClient client;
    
    int result = 0;
    digitalWrite(yellow,HIGH);
         
    logThis(4,postData,2);

    if (!(client.connect(host, port))) 
      {logThis("Failed to connect");
        return 1;}
      else
      {
      client.println("POST " + URI + " HTTP/1.1");
            client.print("Host: ");
            client.println(host);
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
  
  /*  while(!client.available()){
      if( (currentMillis - previousMillis) > interval ){
        logThis("Timeout");
        client.stop();
        digitalWrite(red,HIGH);     
        return 1;
      }
      currentMillis = millis();
    }
    */
      }
  String line;
  while (client.connected()) {line += client.readStringUntil('~');}   
  int idxBodyStart = line.indexOf("\r\n\r\n") + 4; 

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

  char* host = "www.google.com";
  int port = 443;
  String URI = "/";  
  String successValidator = "<!doctype html>";

  NetworkResponse myNetworkResponse = httpRequest(host, port, "GET", URI, "", successValidator, 0);

//typedef struct {int resultCode; String header; String body; int headerLength; int bodyLength;} NetworkResponse;

  if (myNetworkResponse.resultCode==0) {
        logThis(1,"Internet connection test completed successfully");
        }else{
        logThis(0,"Internet connection test failed");
        digitalWrite(red,HIGH);
        }
        
  return myNetworkResponse.resultCode;
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

NetworkResponse httpRequest(char* host, int port, String requestType, String URI, String postData, String successValidator, bool quicky){
  
  NetworkResponse myNetworkResponse;
  
  if(!((requestType == "POST")||(requestType == "GET"))) {logThis("Unsupported call type"); 
                                                        myNetworkResponse.resultCode=2;
                                                        return myNetworkResponse;}
  logThis(4,"requesting URI: " + URI);

  bool SecureConnection;
  if (port == 443)  SecureConnection = true; else SecureConnection=false;

  String httpComm = requestType + " " + //(SecureConnection ? "https://":"http://") + 
              //     String(host) + 
              URI + " HTTP/1.1\r\n" +
                   "Host: " + String(host) + "\r\n" +
                   "User-Agent: BuildFailureDetectorESP8266\r\n" +
                   "Cache-Control: no-cache \r\n" + 
                   "Content-Type: application/x-www-form-urlencoded;\r\n" ;
                   
  if (requestType == "POST"){httpComm += "Content-Length: " + postData.length() + String("\r\n\r\n") + postData;}   

  if (requestType == "GET"){httpComm += "Connection: close\r\n\r\n";}

  if (SecureConnection) myNetworkResponse = secureHttpRequestExecuter(host , port, httpComm, myNetworkResponse);
    else                myNetworkResponse =       httpRequestExecuter(host , port, httpComm, myNetworkResponse);
 
  myNetworkResponse.headerLength = myNetworkResponse.header.length();
  myNetworkResponse.bodyLength = myNetworkResponse.body.length();
 
  if (myNetworkResponse.headerLength < 100) {     logThis("Extremely short headers: " +myNetworkResponse.header ); 
                                                  myNetworkResponse.resultCode=4; 
                                                  digitalWrite(yellow,LOW);
                                                  digitalWrite(red,HIGH);
                                                  return myNetworkResponse;}
  
  logThis(5,"Headers: " + myNetworkResponse.header);
  logThis(4,"Body: " + myNetworkResponse.body);
 
  if (myNetworkResponse.body.indexOf(successValidator) == -1) {
                                              logThis("Unanticipated Reponse received.");
                                              myNetworkResponse.resultCode=5; 
                                              digitalWrite(yellow,LOW);
                                              digitalWrite(red,HIGH);
                                              return myNetworkResponse;}
  
  logThis(3,"Anticipated Reponse received.");
  myNetworkResponse.resultCode = 0;
  
return myNetworkResponse;
}

NetworkResponse httpRequestExecuter(char* host ,int port,String httpComm, NetworkResponse myNetworkResponse){
  
  int resultCode=0;
   digitalWrite(yellow,HIGH);
  // WiFiClientSecure client; 
   WiFiClient client; 
 
   logThis(2,"connecting to " + String(host));  
    
   if (!client.connect(host, port)) {
          logThis("connection failed");
          client.stop();
          digitalWrite(red,HIGH);
          myNetworkResponse.resultCode=3; 
          return myNetworkResponse;
     }
  
    client.print(httpComm);
Serial.println(httpComm);//    logThis(5,httpComm);

  long interval = 12000;
  unsigned long currentMillis = millis(), previousMillis = millis();
  String line;
      while (client.connected()) {
      line = client.readStringUntil('~');    //assumed symbol never exists
      line += line;  // assumed symbol doesn't exist.
    }

      int idxHeader = line.indexOf("\r\n\r\n");
      
      myNetworkResponse.header = line.substring(0,idxHeader);
      myNetworkResponse.body = line.substring(idxHeader+4);

      myNetworkResponse.resultCode = 0;
 
  /*    if((currentMillis - previousMillis) > interval ){
                                                          logThis("Connection Timeout");
                                                          myNetworkExecResponse.resultCode=5; 
                                                          client.stop();
                                                          digitalWrite(red,HIGH); 
                                                          digitalWrite(yellow,LOW);     
                                                          return myNetworkExecResponse;}
      currentMillis = millis();
  }  */  
    
  client.stop(); 
  digitalWrite(yellow,LOW);     
  return myNetworkResponse;
  }

NetworkResponse secureHttpRequestExecuter(char* host ,int port,String httpComm, NetworkResponse myNetworkResponse){
     
   int resultCode=0;
   digitalWrite(yellow,HIGH);
   WiFiClientSecure client; 
   //WiFiClient client; 
 
   logThis(2,"connecting to " + String(host));  
    
   if (!client.connect(host, port)) {
          logThis("connection failed");
          client.stop();
          digitalWrite(red,HIGH);
          myNetworkResponse.resultCode=3; 
          return myNetworkResponse;
     }
  
    client.println(httpComm);
    logThis(5,httpComm);


  long interval = 12000;
  unsigned long currentMillis = millis(), previousMillis = millis();
  String line;
      while (client.connected()) {
      line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
      myNetworkResponse.header += line;  // assumed symbol doesn't exist.
    }
          
      while (client.available()) {
          line = client.readStringUntil('\n'); 
          if (line == "\r") {
            break;
          }
          myNetworkResponse.body += line;  
    }

      
  /*    if((currentMillis - previousMillis) > interval ){
                                                          logThis("Connection Timeout");
                                                          myNetworkExecResponse.resultCode=5; 
                                                          client.stop();
                                                          digitalWrite(red,HIGH); 
                                                          digitalWrite(yellow,LOW);     
                                                          return myNetworkExecResponse;}
      currentMillis = millis();
  }  */  

      myNetworkResponse.resultCode = 0;

  client.stop(); 
  digitalWrite(yellow,LOW);     
  return myNetworkResponse;
  }

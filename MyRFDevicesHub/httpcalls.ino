int initiateNetwork(){
  int result;
  digitalWrite(yellow,HIGH);

  logThis(1,"connecting to " + String(ssid),1);

  long interval = 9000;
  unsigned long currentMillis = millis(), previousMillis = millis();
  int countConnect = 0 ;
  WiFi.disconnect();
  delay(1000);
  WiFi.begin(ssid, password);  delay(1000);
  while ((WiFi.status() != WL_CONNECTED) && (countConnect < 20)) {
    delay(300);  Serial.print(".");
    countConnect ++;
    }
  if (countConnect == 20){
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

  String httpComm = requestType + " " + URI;

                   if (requestType == "GET"){httpComm = httpComm + "?" + postData;}                 
                   
         httpComm = httpComm + " HTTP/1.1\r\n" +
                   "Host: " + String(host) + "\r\n" +
                   "User-Agent: BuildFailureDetectorESP8266\r\n" +
                   "Cache-Control: no-cache \r\n" + 
                   "Content-Type: application/x-www-form-urlencoded;\r\n" ;

  if (requestType == "POST"){httpComm = httpComm + "Content-Length: " + postData.length() + String("\r\n\r\n") + postData + String("\r\n\r\n");} 

  if (requestType == "GET"){httpComm = httpComm + "Connection: close\r\n\r\n";}

  if (SecureConnection) myNetworkResponse = secureHttpRequestExecuter(host , port, httpComm);
    else                myNetworkResponse =       httpRequestExecuter(host , port, httpComm);
 
  myNetworkResponse.headerLength = myNetworkResponse.header.length();
  myNetworkResponse.bodyLength = myNetworkResponse.body.length();
 
  if (myNetworkResponse.headerLength < 38) {     logThis("Extremely short headers: " + String(myNetworkResponse.headerLength) + "\n " + String(myNetworkResponse.header)); 
                                                  myNetworkResponse.resultCode=4; 
                                                  digitalWrite(yellow,LOW);
                                                  digitalWrite(red,HIGH);
                                                  return myNetworkResponse;}
  
  logThis(5,"Headers: " + myNetworkResponse.header);
  logThis(4,"Body: " + myNetworkResponse.body);
 
  if ((myNetworkResponse.body.indexOf(successValidator) == -1) && 
      (myNetworkResponse.header.indexOf(successValidator) == -1))                        
                                              {
                                              logThis("Unanticipated Reponse received.");
                                              logThis(1,myNetworkResponse.header);
                                              logThis(1,myNetworkResponse.body);
                                              myNetworkResponse.resultCode=5; 
                                              digitalWrite(yellow,LOW);
                                              digitalWrite(red,HIGH);
                                              return myNetworkResponse;}
  
  logThis(3,"Anticipated Reponse received.");
  myNetworkResponse.resultCode = 0;
  
return myNetworkResponse;
}

NetworkResponse httpRequestExecuter(char* host ,int port,String httpComm){

  NetworkResponse myNetworkResponse;
    myNetworkResponse.header = "";
    myNetworkResponse.body = "";
  
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
    logThis(5,httpComm);

   // String response="";
    unsigned long timeout = millis();
    bool isHeader = true;
    while (client.available() == 0) {
        if (millis() - timeout > 15000) {
                        logThis("connection Timeout");
                        client.stop();
                        digitalWrite(red,HIGH);
                        myNetworkResponse.resultCode=5; 
                        return myNetworkResponse;
                      }
     }
    
    while(client.available() > 0) {
        String line = client.readStringUntil('\n');
        if (line == "\r") isHeader=false;
        if (!(line == "")) {
          if (isHeader) {myNetworkResponse.header = myNetworkResponse.header + line; }
              else      {myNetworkResponse.body   = myNetworkResponse.body   + line; }
             
          }
    }

    myNetworkResponse.headerLength = myNetworkResponse.header.length();
    myNetworkResponse.bodyLength   = myNetworkResponse.body.length();

    if(DEBUGLEVEL>5){
        Serial.println("\nrequest");
        Serial.println(httpComm);
        Serial.println("headers");
        Serial.println(myNetworkResponse.headerLength);
        Serial.println(myNetworkResponse.header);
        Serial.println("body");
        Serial.println(myNetworkResponse.body.length());
        Serial.println(myNetworkResponse.body);
        Serial.println("end");
        
    }

 myNetworkResponse.resultCode = 0;  
 client.stop(); 
 digitalWrite(yellow,LOW);     
return myNetworkResponse;
}

NetworkResponse secureHttpRequestExecuter(char* host ,int port,String httpComm){
 NetworkResponse myNetworkResponse;
    myNetworkResponse.header = "";
    myNetworkResponse.body = "";
    
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

    client.print(httpComm);
  
   // String response="";
    unsigned long timeout = millis();
    bool isHeader = true;
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
                        logThis("connection Timeout");
                        client.stop();
                        digitalWrite(red,HIGH);
                        myNetworkResponse.resultCode=5; 
                        return myNetworkResponse;
                      }
     }
    
    while(client.available()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") isHeader=false;
        if (!(line == "")) {
          if (isHeader) {myNetworkResponse.header = myNetworkResponse.header + line; }
              else      {myNetworkResponse.body   = myNetworkResponse.body   + line; }
             
          }
    }

    myNetworkResponse.headerLength = myNetworkResponse.header.length();
    myNetworkResponse.bodyLength   = myNetworkResponse.body.length();

    if(DEBUGLEVEL>5){
        Serial.println("\nrequest");
        Serial.println(httpComm);
        Serial.println("headers");
        Serial.println(myNetworkResponse.headerLength);
        Serial.println(myNetworkResponse.header);
        Serial.println("body");
        Serial.println(myNetworkResponse.body.length());
        Serial.println(myNetworkResponse.body);
        Serial.println("end");
        
    }

 myNetworkResponse.resultCode = 0;  
 client.stop(); 
 digitalWrite(yellow,LOW);     
return myNetworkResponse;
}

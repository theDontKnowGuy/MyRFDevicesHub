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

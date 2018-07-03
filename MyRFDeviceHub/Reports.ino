String reportDeviceStatus(RFDevice myDevice){

 String          DeviceReport = "DeviceID: " + myDevice.ID;
                DeviceReport = DeviceReport + " MsgLen: " + String(myDevice.messageLength);
//              DeviceReport = DeviceReport + " FreeHeapMem: " + system_get_free_heap_size();
                DeviceReport = DeviceReport + " DeviceType: " + Devices[myDevice.idx].type;
                DeviceReport = DeviceReport + " DeviceLocation: " + Devices[myDevice.idx].location;                
                if(myDevice.IsMove) DeviceReport = DeviceReport +  "IsMove ";
                if(myDevice.IsTamper)  DeviceReport = DeviceReport + "IsTamper ";
                if(myDevice.IsPing) DeviceReport = DeviceReport +  "IsPing ";
                if(myDevice.temperature != 999) DeviceReport = DeviceReport + "temperature: " +  String(myDevice.temperature);  
                if(myDevice.IsUnkown) DeviceReport = DeviceReport + "IsUnkown ";
                DeviceReport = DeviceReport + " DeviceStatus: " + myDevice.deviceStatus +" ";

 String  postData = "<html>";
         postData += "<deviceID>" +       myDevice.ID + "</deviceID>";
         postData += "<deviceLocation>" + Devices[myDevice.idx].location + "</deviceLocation>";
         postData += "<deviceType>" +     Devices[myDevice.idx].type + "</deviceType>";
         postData += "<deviceStatus>" +   myDevice.deviceStatus + "</deviceStatus>";
         
         switch (Devices[myDevice.idx].protocol) {
            case 1:   //Visonic
                   if (myDevice.IsMove==1)  postData += "<motion>"+ String(1) + "</motion>";
                   if (myDevice.IsPing==1)  postData += "<motion>"+ String(0) + "</motion><tamper>"+ String(0) + "</tamper>";
                   if (myDevice.IsTamper==1)postData += "<tamper>"+ String(1) + "</tamper>";
            break;
            case 4:   //termostat
                   if ((myDevice.temperature < 999) && (myDevice.temperature >= 0)) postData += "<temperature>"+ String(myDevice.temperature) + "</temperature>";
            break;
         
         
         
         } /// end switch case 
         postData += "</html>";
         
         if (DEBUGLEVEL > 0) Serial.println((postData));
  return postData;       
}

void ReportUnkownDevice(RFDevice myDevice){
                String DeviceReport =  "DeviceID: " + myDevice.ID;
                DeviceReport = DeviceReport + " MsgLen: " + String(myDevice.messageLength);
                DeviceReport = DeviceReport + " ShortLongTH_DefaultUsed: 550 ";
                //default to something if device length not recognized
                DeviceReport = DeviceReport + " DeviceType: ***Unkown***";              
                DeviceReport = DeviceReport + " DeviceBitStream: " ;

                String UnknwonStream = "";
                for (int i=0 ; i < DeviceMessageLength ; i++)  {UnknwonStream=UnknwonStream+ timingsBins[i]+ " ";}                
                DeviceReport = DeviceReport +  UnknwonStream;             

                if (DEBUGLEVEL > 0) Serial.println((DeviceReport));
}

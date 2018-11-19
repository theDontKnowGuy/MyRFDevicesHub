String reportDeviceStatus(RFDevice myDevice){

 if (DEBUGLEVEL > 0){
      String    DeviceReport =  " DeviceID: " + myDevice.ID;
                DeviceReport = DeviceReport + " MsgLen: " + String(myDevice.messageLength);
     //           if (DEBUGLEVEL > 4) DeviceReport = DeviceReport + " FreeHeapMem: " + system_get_free_heap_size();
                DeviceReport = DeviceReport + " DeviceType: " + Devices[myDevice.idx].type;
                DeviceReport = DeviceReport + " DeviceLocation: " + Devices[myDevice.idx].location;                
                if(myDevice.IsMove) DeviceReport = DeviceReport +  "IsMove ";
                if(myDevice.IsTamper)  DeviceReport = DeviceReport + "IsTamper ";
                if(myDevice.IsPing) DeviceReport = DeviceReport +  "IsPing ";
                if(myDevice.temperature != 999) DeviceReport = DeviceReport + "temperature: " +  String(myDevice.temperature);  
                if(myDevice.IsUnkown) DeviceReport = DeviceReport + "IsUnkown ";
                DeviceReport = DeviceReport + " DeviceStatus: " + myDevice.deviceStatus +" ";

         logThis(0,DeviceReport,2);
  }

String  postData = ""; 

        postData += "deviceID=" +       myDevice.ID + "&";
        postData += "deviceLocation=" + Devices[myDevice.idx].location + "&";
        postData += "deviceType=" +     Devices[myDevice.idx].type + "&";
        postData += "deviceStatus=" +   myDevice.deviceStatus;

 

 // String  postData = "<html>";
 //        postData += "<timeStamp>" +      getTimeStamp() + "</timeStamp>";
  //       postData += "<deviceID>" +       myDevice.ID + "</deviceID>";
  //       postData += "<deviceLocation>" + Devices[myDevice.idx].location + "</deviceLocation>";
  //       postData += "<deviceType>" +     Devices[myDevice.idx].type + "</deviceType>";
  //       postData += "<deviceStatus>" +   myDevice.deviceStatus + "</deviceStatus>";

         switch (Devices[myDevice.idx].protocol) {
            case 1:   //Visonic
                   if (myDevice.IsMove==1)  postData += "&motion=1";
                   if (myDevice.IsPing==1)  postData += "&ping=1&tamper=0";
                   if (myDevice.IsTamper==1)postData += "&tamper=1";
            break;
            case 4:   //termostat
                   if ((myDevice.temperature < 999) && (myDevice.temperature >= 0)) postData += "&temperature="+ String(myDevice.temperature);
            break;

         } /// end switch case 
  
         
         logThis(3,postData,2);
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

                logThis(DeviceReport);
}

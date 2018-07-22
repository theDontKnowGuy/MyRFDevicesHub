void getDeviceDetails(RFDevice* myDevice){
   int result = 0;
   Length2bin();            // convert data stream to workable information. clean up streams...

   myDevice->messageLength = DeviceMessageLength;
    
    // which protocol is the device ?
  
   for (int i=0 ; i < NoOfSupportedProtocols; i++){if(DeviceMessageLength == SupportedProtocols[i].messageLength){myDevice->protocolIdx = i;}}

    // extract deviceID

   for (unsigned int i = 0; i < SupportedProtocols[myDevice->protocolIdx].deviceNameLength ; i++)
      {    result = result << 1;
           result = result | (BinsStream[i] & 1);
      }
      myDevice->ID = "0x" + String(result, HEX);

    //  do i know this DeviceID ?   get device index in the db
    
    isDeviceExist = false;
    int idxDevice = -1;
    while ((idxDevice < NoOfDevices) && !(isDeviceExist)) { 
          idxDevice++;
          if (Devices[idxDevice].ID == myDevice->ID)  isDeviceExist = true; 
     }  
    myDevice->idx = (isDeviceExist) ? idxDevice : -1;
 }

void getDeviceStatus(RFDevice* myDevice){
  String deviceStatus = "";
  for (int i = SupportedProtocols[myDevice->protocolIdx].deviceNameLength ; i< (SupportedProtocols[myDevice->protocolIdx].messageLength/2-1) ; i++) 
        {deviceStatus = deviceStatus +  String(BinsStream[i]);}
        
   myDevice->deviceStatus = deviceStatus;
}

void AnalyzeDeviceStatus(RFDevice* myDevice){
     
    switch (Devices[myDevice->idx].protocol) {
        case 1:   //Visonic
                 myDevice->IsMove = (myDevice->deviceStatus[6] == '1' ?1:0); 
                 myDevice->IsTamper = (myDevice->deviceStatus[5] == '1' ?1:0); 
                 myDevice->IsPing = (myDevice->deviceStatus[8] == '1' ?1:0); 
                 myDevice->IsUnkown =  ((String(myDevice->deviceStatus[0])+ 
                                        String(myDevice->deviceStatus[1])+ 
                                        String(myDevice->deviceStatus[2])+
                                        String(myDevice->deviceStatus[3])+
                                        String(myDevice->deviceStatus[4])+
                                        String(myDevice->deviceStatus[7])+
                                        String(myDevice->deviceStatus[9])+
                                        String(myDevice->deviceStatus[10])+
                                        String(myDevice->deviceStatus[11])) == "00101001000") ? 0 : 1;   

        break;
        case 4:  //thermometer
              
              String s="";
              unsigned long bin=0;
              for(int i = 1 ; i <12 ; i++){s = s + ((myDevice->deviceStatus[i]=='1') ? '1':'0');}
              bin = s.toInt();
              myDevice->temperature = convertBin2Dec(bin)/10;
              if(DEBUGLEVEL>2){ Serial.println(bin);Serial.println(myDevice->temperature);}                
                               
        break;
        
    }   ///SWITCH CASE
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


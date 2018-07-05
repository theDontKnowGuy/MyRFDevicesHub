int* GetRawData(){
    
    unsigned int *timings;
    unsigned int timings_size;
    unsigned int pulse_length_divider = RFControl::getPulseLengthDivider();   
    RFControl::getRaw(&timings, &timings_size);
    DeviceMessageLength = timings_size;   

    for(int i=0; i < timings_size; i++) {   
        unsigned long timing = timings[i] * pulse_length_divider;
        timingsBins[i] = timing;
      }
    return timingsBins;
}

void Length2bin(){

   int* timingsBins = GetRawData(); 
   int LongShortTH = IdentityDeviceByMessageLength();  
      
   switch (DeviceMessageLength) {
      
        case 50:        // usual remote control. regular interpetation
                for (int i=0 ; i < DeviceMessageLength/2 ; i++)  {      
                if (timingsBins[i*2] < LongShortTH) BinsStream[i]=0;
                else if (timingsBins[i*2] < MaxValidLengthTH) BinsStream[i]=1;
                else BinsStream[i]= -1;
                }
        break;
        case 74:        // visonic. 1 and 0 replaced.
                for (int i=0 ; i < DeviceMessageLength/2; i++)  {      
                if (timingsBins[i*2] < LongShortTH) BinsStream[i]=1;
                else if (timingsBins[i*2] < MaxValidLengthTH) BinsStream[i]=0;
                else BinsStream[i]= -1;
                }
        break;
        case 66:        //kitchen termometer - start one byte later
                for (int i=0 ; i < DeviceMessageLength/2 ; i++)  {      
                if (timingsBins[i*2+1] < LongShortTH) BinsStream[i]=0;
                else if (timingsBins[i*2+1] < MaxValidLengthTH) BinsStream[i]=1;
                else BinsStream[i]= -1; 
                }           
        break;

        }  // switch case

    if (DEBUGLEVEL > 3){
      for(int i=0;i<DeviceMessageLength;i++){Serial.print(timingsBins[i]);Serial.print(",");}Serial.print("\n");
      for(int i=0;i<DeviceMessageLength/2;i++){Serial.print(BinsStream[i]);Serial.print(", ,");}Serial.print("\n");
    }
}

int IdentityDeviceByMessageLength(){
    int LongShortTH = 0;  
    for (int i=0 ; i < NoOfDevicesMessageLengths; i++){
        if(DeviceMessageLength == SupportedProtocols[i].messageLength) LongShortTH = SupportedProtocols[i].LongShortTH;                 
     } 
    if (LongShortTH == 0) {
                             LongShortTH = 550; //default value  for unknown
                             if (DEBUGLEVEL > 0) {Serial.print("**Device message length not recognized. using Treshhold default level**");}
    }
    return LongShortTH;
}
      

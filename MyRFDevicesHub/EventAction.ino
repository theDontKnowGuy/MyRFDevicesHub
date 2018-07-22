
int eventAction(RFDevice myDevice){

    int actionIdx, result =0;
    String postData, host, successValidator;

    postData = reportDeviceStatus(myDevice);
    
    for (int i=0; i < NoOfDeviceActions ; i++)
        {if (Devices[myDevice.idx].type == myDevicesActions[i].deviceType) actionIdx = myDevicesActions[i].actionIdx;}
        
    switch(actionIdx){
      case 0:       ////    PIR, "httpPostLocal", host1,"SalonMotionSensor","status"
                 host = myActions[actionIdx].actionParam1;
                 successValidator = myActions[actionIdx].successValidator;
                     
                 result = httpPostRequest(host, postData, successValidator);
      break;
 
      
      case 1:       ///// remote button to call webcore piston
                host = myActions[actionIdx].actionParam1;
                postData = myActions[actionIdx].actionParam2 + myActions[actionIdx].actionParam3;
                successValidator = myActions[actionIdx].successValidator;
                
                result = httpGetRequest(host, postData, successValidator); 
      break;
      
    } /// switch case

    return 0;
}





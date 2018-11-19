
int eventAction(RFDevice myDevice){

    int actionIdx, port, result =0;
    String postData, successValidator;
    char* host;
    NetworkResponse myNetworkResponse;
    
    postData = reportDeviceStatus(myDevice);
    
    for (int i=0; i < NoOfDeviceActions ; i++)
        {if (Devices[myDevice.idx].type == myDevicesActions[i].deviceType) actionIdx = myDevicesActions[i].actionIdx;}
        
    switch(actionIdx){
      case 0:       ////    PIR, "httpPostLocal", host1,"SalonMotionSensor","status"
               host = myActions[actionIdx].actionParam1;
               port = myActions[actionIdx].actionParam2.toInt();
               successValidator = myActions[actionIdx].successValidator;
               myNetworkResponse = httpRequest(host, port, "POST", "/posts", postData, successValidator, 0);
               result = myNetworkResponse.resultCode;
            //    result = httpPostRequest(host, port, "posts", postData, successValidator);
      break; 
            
      case 1:       ///// for example: get request to call webcore piston

                host = myActions[actionIdx].actionParam1;
                port = myActions[actionIdx].actionParam2.toInt();
                successValidator = myActions[actionIdx].successValidator;
                myNetworkResponse = httpRequest(host, port, "GET",  myActions[actionIdx].actionParam3, postData, successValidator, 0);
                result = myNetworkResponse.resultCode;
      break;
      
    } /// switch case

    return 0;
}

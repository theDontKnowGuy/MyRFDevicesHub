# myRFDevicesHub
Connect all my RF 433 Devices messages and convert them to actions like connect to Smartthings

 * Version 0.5 beta - Support logging over http. Adoptation to ArduinoJson lib 6.2.3. 
 * Version 0.4 beta - Support set clock and add timestamp to reports and logs
 * Version 0.3 beta - Support dynamic load of new devices
 * Version 0.2 beta
 * credit to the writers of RFControl, ArduinoJson and ESP8266WiFi stacks.
--------------------------------------------------------------------------------------------


So I have so many semy old to dead old 433Mhz RF  devices: from aliexpress 5$ sensors to fancy VISONIC PIR, all sort of remote controls that start stuff, and my kitchen kings meat thermometer which sends signals to its display device (https://www.aliexpress.com/item/Digital-Wireless-LCD-BBQ-Grill-Meat-Oven-Food-Remote-Thermometer-Kitchen-Tool/32758598223.html?spm=a2g0s.9042311.0.0.64f94c4dRSp3qR) which I love.

I wanted to reutilize the Visonic sensors to my Smartthings hub as Visonic's hub is a stupid box, but their sensors are bloody good.
Reversed engineered the protocol, implemented it in Arduino and was about to close the shop but then decided to add the remote control, then something else than the meat thermometer as I decided that one cannot live his life peacefully without KODI to play a special announcement once the meat gets to the medium-rare point.
Then things started to get out of control. I really wanted to make it generic so little work required, without coding, to add the next stupid device I will find / will must buy on Aliexpress. 

I implemented integration to Smartthings via two ways: One using external, webcore trigger execution URL (via HTTP GET request) and another with posting directly to Smartthings hub on the local network. These are just examples of course which can be expanded.

The hub designed to be aware of error and problems (network) and try to recover from them. A lot to do to bring it to completion.

I am using WEMOS D1R2 but any ESP8266 should work and any Arduino with network access should be OK with some minor code change on the network stack.

I am not a programmer. I can write few lines in C but that is about it.  So please forgive any programming flaws and feel free to let me know or to fix them yourself so we will update this project together.


*Installation:* 
1. Download the myRFDevicesHub into your Arduino library (lots of files, one project with tabs so you don't have to scroll like maniacs).
2. Make sure to install RFControl from here https://github.com/pimatic/RFControl
3. As an optional step, place the MyRFDevicesHubLogger.php on a (preferebly local) php web server to save the logs on the network, again so you don't need to go and connect to the hub when you want to know what is going on. Configure the path where logs are saved to on the php file. Configure the path to the php file in the logTarget parameter.
4. As an optional step, place RFDevices.json on the same local network http web server and configure the path to it in the dataUpdateHost, port and URI parameters. This will override the list of devices and protocol, as well as debuglevel, so you don't need to recompile to add a new device or to incease debuglevel.
3. Configure your new life. All configurations are in the upper part of the main tab (myRFDevicesHub) and some are overrided in the json file - so change it there first .
5. Actions: configure the host and URL parameters you want to use. In the out of the box  examples put your secret key, account hash and piston# and it will work.
6. To add new RF devices first try to run them as is. They will be unknown, but if you get a deviceID in the log, the protocol is recognized. ye. add their device ID to the Devices database in the json and you are good to go (for the most part. parameters can be different).
6. If the protocol is not recognized, enlarge the DEBUGLEVEL on the json file and get a printout of the codes received. If you don't get a good split of 0's and 1's in the output, you may want to change the treshhold of the digital reading of the actual radio transmission to a number somewhere between the low numbers and the high number (microseconds of up and down radio signals).


Would love to get your feedback.
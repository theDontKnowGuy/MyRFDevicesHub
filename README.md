# myRFDevicesHub
Connect all my RF 433 Devices messages and convert them to actions like connect to Smartthings

* V0.4 beta - support set clock and add timestamp to reports and logs
* V0.3 beta - support dynamic load of new devices - ***** No Need to recompile to add new devices ***** Configuration done with a json file on another server. See attached json for example.
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
3. Configure your new life. All configurations are in the upper part of the main tab (called after the project myRFDevicesHub).
4. Actions: configure the host and URL parameters you want to use. In the out of the box  examples put your secret key, account hash and piston# and it will work.
5. To add new RF devices first try to run them as is. They will be unknown, but if you get a deviceID in the log, the protocol is recognized. ye. add their device ID to the Devices database and you are good to go (for the most part. parameters can be different).
6. If the protocol is not recognized, enlarge the DEBUGLEVEL and get a printout of the codes received. If you don't get a good split of 0's and 1's in the output, you may want to change the treshhold of the digital reading of the actual radio transmission to a number somewhere between the low numbers and the high number (microseconds of up and down radio signals).

Known limitations and issues:
- Still crashes from time to time.  Recovers quickly. Working to find out why.

Would love to get your feedback.
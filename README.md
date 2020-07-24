# control-any-tv-with-google-assistant-esp8266
Control any TV with google home and assistant and esp8266
See video below that explain this project

https://youtu.be/lyLbQSHj5Yw

This video shows how to control any TV using Google Assistant and Google Home with ESP8266 or any IOT device.

A prerequisite to have this project working is a Java Application hosted in Google Cloud that has an interface with Google Smart-Home application and sends MQTT messages to your smart device like ESP8266 when you act on Google Home or Google Assistant. Please check the video below to see how to set up this application. https://youtu.be/-tnYAI3mE24 

Another step that is needed in this project is to define a new device TV for you in Google Home using a front-end application. You can also check the video below that explains how to install and configure this front-end application. https://youtu.be/0czm7VIgoZs

After you define this new TV device, you need to define the channels in the firestore database. 
As the channel trait was recently created by Google, the front-end does not have an example ready for it. So you need to manually add channels in the database. For instance 3 channels was added, CNN, BBC and RAI, check the channel names and numbers, this is important as you need to adapt it to your case.

When your cloud application and front-end is set up and the TV device is defined, it is time to work on your IOT device using the Arduino IDE.

Firstly you need to assemble the electronic parts, see electronic schematic and when it is working the first step is to map your TV remote control codes (the numbers) to change the channels, on and off and volume buttons. Each remote control button has a unique infrared code for each button.

Install the library IRRemote8266 and load the example IRrecvDumpV3 and then upload it to your IOT device (ESP8266), in my case Wemos D1 R1.

Then with the code running in ESP8266 press the numbers from your remote control and also on/off button and volume buttons and save the values that you get in the serial monitor.

Now is the last step. Load the code (from github here) this code will receive MQTT messages from your cloud application connected to Google and will send the infrared signals to you TV ou cable TV receptor.
The code needs to be adapted to your device id (created in front-end application), mqtt and wifi credentials. 

After your updates, upload it to your IOT Device.

Now you can start giving voice commands to change the channel, volume and turning on/off your TV.

Hope you enjoy this video, any questions or comments I will be glad to answer!

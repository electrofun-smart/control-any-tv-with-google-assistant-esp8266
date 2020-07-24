
#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include <ArduinoJson.h> // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <IRremoteESP8266.h>
#include <IRsend.h>

String device_tv = "12js"; // Device ID defined in front-end application

char ssid[] = "<wifi_ssid>";   //  your network SSID (name)
char pass[] = "<wifi_password>"; // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;
WiFiClient net;
MQTTClient client;
unsigned long lastMillis = 0;

// MQTT info
const char *thehostname = "<thehostname of your mqtt broker>";
const char *user = "<user mqtt broker>";
const char *user_password = "<password mqtt broker>";
const char *id = "Wemos-TV-Project";

const uint16_t kIrLed = 14;   // ESP8266 GPIO pin to use. D5 Wemos D1

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

int khz = 38; //NB Change this default value as neccessary to the correct modulation frequency

uint16_t rawDataOnOff[71] = {9018, 4464, 590, 542, 590, 542, 590, 1662, 588, 542, 588, 542, 590, 542, 590, 542, 590, 542, 588, 1662, 564, 1688, 590, 546, 586, 1664, 586, 1662, 592, 1662, 588, 1662, 588, 1664, 588, 546, 586, 544, 588, 544, 590, 1660, 590, 542, 590, 542, 588, 544, 592, 540, 588, 1662, 588, 1664, 588, 1664, 590, 540, 588, 1664, 588, 1664, 586, 1664, 592, 1660, 590, 39852, 9038, 2226, 590};     // NEC 20DF10EF
uint16_t volIncrease[71] = {9042, 4444, 610, 544, 588, 542, 588, 1664, 586, 542, 588, 544, 590, 540, 588, 544, 590, 542, 590, 1660, 588, 1662, 588, 544, 588, 1664, 590, 1660, 590, 1660, 590, 1662, 588, 1662, 588, 544, 590, 1660, 590, 538, 590, 542, 614, 518, 590, 540, 590, 542, 590, 542, 590, 1660, 590, 542, 590, 1662, 590, 1660, 590, 1660, 590, 1662, 588, 1664, 586, 1662, 588, 39850, 9038, 2208, 608}; // NEC 20DF40BF
uint16_t volDecrease[71] = {9042, 4464, 590, 544, 588, 544, 588, 1660, 590, 546, 586, 542, 590, 542, 590, 542, 588, 542, 588, 1664, 590, 1660, 588, 542, 590, 1662, 590, 1660, 590, 1660, 588, 1664, 590, 1662, 588, 1662, 590, 1662, 588, 544, 590, 540, 588, 544, 590, 542, 590, 540, 588, 544, 588, 544, 590, 542, 590, 1660, 590, 1660, 590, 1662, 588, 1662, 588, 1664, 586, 1662, 588, 39848, 9040, 2206, 610}; // NEC 20DFC03F
uint16_t tvinput[71] = {9042, 4466, 588, 570, 562, 572, 562, 1662, 610, 522, 610, 522, 584, 548, 586, 568, 560, 572, 562, 1666, 584, 1666, 608, 522, 610, 1640, 586, 1690, 562, 1664, 610, 1640, 584, 1668, 586, 1664, 584, 1664, 610, 522, 610, 1638, 586, 546, 610, 522, 610, 522, 612, 520, 610, 522, 586, 546, 586, 1664, 586, 548, 584, 1664, 612, 1640, 586, 1668, 586, 1664, 614, 39888, 9018, 2232, 562};     // NEC 20DFD02F

uint16_t netDigito1[71] = {9008, 4482, 560, 1710, 538, 1712, 560, 1688, 536, 582, 538, 580, 536, 582, 538, 580, 538, 1712, 536, 582, 536, 1710, 536, 1710, 538, 1710, 560, 1692, 534, 580, 538, 1710, 536, 582, 538, 1712, 560, 556, 538, 582, 560, 558, 540, 578, 538, 584, 534, 580, 538, 582, 536, 580, 538, 1716, 534, 1708, 538, 1710, 538, 1712, 558, 1688, 536, 1710, 538, 1710, 538, 39400, 9008, 2256, 538}; // NEC E17A807F
uint16_t netDigito2[71] = {9006, 4486, 562, 1708, 542, 1708, 536, 1712, 536, 580, 538, 582, 538, 582, 536, 580, 538, 1712, 536, 582, 538, 1710, 538, 1710, 538, 1710, 538, 1712, 536, 580, 536, 1712, 538, 580, 536, 582, 586, 1662, 536, 582, 562, 558, 536, 582, 538, 582, 610, 508, 536, 582, 638, 1610, 538, 580, 538, 1708, 538, 1710, 536, 1714, 688, 1558, 688, 1558, 538, 1712, 536, 39398, 9008, 2236, 562}; // NEC E17A40BF
uint16_t netDigito5[71] = {9008, 4484, 582, 1688, 536, 1714, 558, 1688, 562, 556, 536, 582, 560, 560, 562, 556, 558, 1692, 532, 582, 560, 1688, 560, 1690, 534, 1714, 562, 1688, 558, 558, 538, 1712, 558, 560, 558, 1692, 556, 560, 538, 1710, 538, 580, 560, 560, 534, 582, 560, 558, 538, 582, 560, 560, 536, 1714, 534, 582, 536, 1712, 538, 1710, 560, 1688, 560, 1690, 536, 1712, 558, 39388, 9002, 2236, 584}; // NEC E17AA05F
uint16_t netDigito7[71] = {8984, 4506, 538, 1710, 538, 1710, 536, 1710, 538, 582, 538, 580, 538, 582, 536, 580, 538, 1710, 538, 580, 538, 1710, 560, 1688, 536, 1712, 536, 1712, 536, 582, 536, 1710, 536, 582, 536, 1712, 536, 1712, 536, 1712, 560, 558, 536, 582, 560, 558, 542, 576, 538, 584, 534, 580, 538, 582, 560, 558, 538, 1710, 538, 1712, 536, 1708, 542, 1706, 560, 1688, 536, 39400, 9012, 2232, 558}; // NEC E17AE01F
uint16_t netDigito0[71] = {9008, 4482, 562, 1712, 586, 1662, 538, 1710, 540, 578, 558, 560, 538, 582, 538, 580, 538, 1710, 538, 582, 536, 1712, 538, 1712, 584, 1664, 536, 1714, 534, 582, 560, 1688, 538, 580, 538, 582,  538, 580,  538, 580,  538, 586,  556, 558,  542, 578,  560, 562,  534, 580,  538, 1712,  614, 1634,  538, 1712,  536, 1712,  536, 1712,  536, 1710,  562, 1686,  610, 1638,  536, 39426,  8986, 2234,  562};  // NEC E17A00FF
uint16_t netDigito3[71] = {9030, 4486,  638, 1612,  634, 1610,  538, 1712,  538, 582,  536, 584,  536, 586,  534, 582,  560, 1688,  538, 580,  538, 1712,  640, 1608,  538, 1710,  538, 1712,  538, 580,  538, 1710,  536, 582,  536, 1712,  536, 1714,  610, 504,  538, 582,  538, 582,  536, 580,  538, 582,  584, 534,  536, 582,  590, 528,  538, 1712,  536, 1710,  538, 1710,  532, 1716,  612, 1636,  536, 1710,  538, 39402,  9008, 2258,  538};  // NEC E17AC03F

int currentVolume = 0;

void setup()
{

  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  //Setup Rest Client
  WiFi.begin(ssid, pass);

  irsend.begin();

  client.begin(thehostname, 16157, net);
  client.onMessage(messageReceived);
  connect();
  delay(1000);
}

void connect()
{
  Serial.print("checking wifi…");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.print("\nconnecting…");
  while (!client.connect(id, user, user_password))
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");
  client.subscribe(device_tv + "-client"); 
  delay(1000);
}

void messageReceived(String &topic, String &payload)
{
  Serial.println("incoming: " + topic + " - " + payload);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.parseObject(payload);
  String deviceOn = json["on"];
  int currentVolumeReceived = json["currentVolume"];
  String inputTv = json["currentInput"];
  String channelNumber = json["channelNumber"];

  if (topic == (device_tv + "-client"))
  { 
    if (deviceOn == "true")
    {
      Serial.println("OnOff - toggle");
      irsend.sendRaw(rawDataOnOff, 71, khz); 
    }
    if (deviceOn == "false")
    {
      Serial.println("OnOff - toggle");
      irsend.sendRaw(rawDataOnOff, 71, khz); 
    }

    if (channelNumber == "200"){    // CNN Channel number
      irsend.sendRaw(netDigito2, 71, khz); 
      delay(500);
      irsend.sendRaw(netDigito0, 71, khz); 
      delay(500);
      irsend.sendRaw(netDigito0, 71, khz); 
    }

    if (channelNumber == "202"){    // BBC Channel number
      irsend.sendRaw(netDigito2, 71, khz); 
      delay(500);
      irsend.sendRaw(netDigito0, 71, khz); 
      delay(500);
      irsend.sendRaw(netDigito2, 71, khz); 
    }

    if (channelNumber == "203"){    // RAI Channel number
      irsend.sendRaw(netDigito2, 71, khz); 
      delay(500);
      irsend.sendRaw(netDigito0, 71, khz); 
      delay(500);
      irsend.sendRaw(netDigito3, 71, khz); 
    }
    
    if (currentVolumeReceived > 0)
    {
      if (currentVolumeReceived > currentVolume)
      {
        // increase
        irsend.sendRaw(volIncrease, 71, khz); 
        currentVolume = currentVolumeReceived;
        Serial.println("Increase Volume");
      }
      else
      {
        irsend.sendRaw(volDecrease, 71, khz); 
        currentVolume = currentVolumeReceived;
        Serial.println("Decrease Volume");
      }
    }
    if (inputTv == "hdmi_1")
    {
      irsend.sendRaw(tvinput, 71, khz); 
      delay(500);
      irsend.sendRaw(tvinput, 71, khz); 
      Serial.println("Changing TV Input");
    }

  } // end topic TV 

}

void loop()
{
  client.loop();
  if (!client.connected())
  {
    connect();
  }
}

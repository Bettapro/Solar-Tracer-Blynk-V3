# SW getting started Home Assistant

*!You must have a working mqtt broker and also the mqtt integration in you home assistant integration!*

Download the latest version of this firmware from [here](https://github.com/Bettapro/Solar-Tracer-Blynk-V3/releases/latest).  
Choose the correct bin file according to your board (esp8266, esp32):
- *SolarTracerBlynk_xxxx_esp32dev_mqttHA.bin*: ESP32 only
- *SolarTracerBlynk_xxxx_esp8266_mqttHA.bin*: ESP8266 only

Flash the firmware on your EPS8266/ESP32, more information how to flash firmware are available [ESP32](esp32.md#how-to-flash) [ESP8266](esp8266.md#how-to-flash)


Boot the ESP in Configuration mode and double check all the settings, double check the mqtt section and make sure that settings are correct.
![Alt text](../images/wifi_manager_mqtt_settings.png)  

If everything is working your mqtt integration will discover and configure the solar tracer.  
![image](https://user-images.githubusercontent.com/3984840/160839821-8fd79592-4d98-4107-8b78-3d2c2c555c7f.png)

![image](https://user-images.githubusercontent.com/3984840/160840312-218e7718-c8e7-4236-bc88-470d8f4ea4ed.png)

You can now proceed and display all the data you need on your widgets (and even on the Energy monitoring page!)
![image](https://user-images.githubusercontent.com/3984840/160840967-0c4d75c4-d63d-4e99-bd12-842cfa3c866f.png)
![image](https://user-images.githubusercontent.com/3984840/160841387-f500738a-e4a1-4b52-a2cd-69c78a732b52.png)







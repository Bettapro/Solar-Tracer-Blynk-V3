# SW getting started HomeAssistant

*!You must have a working mqtt broker and also the mqtt integration in you home assistant integration!*

Flash the correct firmware on your EPS8266/32 (v3.0.5 or later). Use the latest version from [Release page](https://github.com/Bettapro/Solar-Tracer-Blynk-V3/releases) ,
download the bin file ending with "_HA".

Here few tips if you decide to compile the code on your own:
- disable Blynk (comment `#define USE_BLYNK`)
- enable MQTT (uncomment `#define USE_MQTT`)
- enable MQTT_HA (uncomment `#define USE_MQTT_HOME_ASSISTANT`)
- Parameters like mqtt server ip, mqtt server port, device name could be specified in config.h or via the wifi configuration page

Boot the ESP in Configuration mode and double check all the settings.
If everything is working your mqtt integration will discover and cofigure the solar tracer.

![image](https://user-images.githubusercontent.com/3984840/160839821-8fd79592-4d98-4107-8b78-3d2c2c555c7f.png)

![image](https://user-images.githubusercontent.com/3984840/160840312-218e7718-c8e7-4236-bc88-470d8f4ea4ed.png)

You can now proceed and display all the data you need on your widgets (and even on the Energy monitoring page!)
![image](https://user-images.githubusercontent.com/3984840/160840967-0c4d75c4-d63d-4e99-bd12-842cfa3c866f.png)
![image](https://user-images.githubusercontent.com/3984840/160841387-f500738a-e4a1-4b52-a2cd-69c78a732b52.png)







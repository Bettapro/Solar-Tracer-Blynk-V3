# SW getting started Blynk LEGACY


Blynk IOT requires *TEMPLATE_ID* to be defined at compilation time, for this reason, there's no pre-build firmware for it.

Download/Clone this project, then, using Arduino IDE or VSC(Platformio) check the file named *config.h*.

- Make sure `#define USE_BLYNK` is enabled/un-commented
- Make sure `#define USE_BLYNK_2` is enabled/un-commented
- Make sure `#define USE_MQTT` is disabled/commented
- Insert your `BLYNK_TEMPLATE_ID` [blynk docs](https://docs.blynk.io/en/getting-started/activating-devices/manual-device-activation#step-2-getting-template-id)
- Insert your `BLYNK_DEVICE_NAME`
- Insert your `BLYNK_AUTH` [blynk docs](https://docs.blynk.io/en/getting-started/activating-devices/manual-device-activation#step-3-getting-auth-token)


Compile the sketch and upload to your ESP module.

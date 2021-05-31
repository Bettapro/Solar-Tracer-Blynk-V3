# SW getting started

If you are using Arduino IDE open *SolarTracerBlynk* folder, then open *SolarTracerBlynk.ino* .

If you are using Platformio import the project using *platformio.ini* .


### Edit `config.h` library

* Firstly, enter WiFi credentials in `WIFI_SSID` and `WIFI_PASS`.

* Open the Blynk mobile app and create a new project by scanning the following QR code

![Project QR Code](../images/blynk-app-qr-code.png)

* Send yourself the generated auth code
* Paste your auth code into `BLYNK_AUTH`

* Upload the sketch to your ESP8266 / ESP32, if you are using platformio you should edit `default_envs` accordingly (eg: *esp8266* to use a esp8266, *esp8266_ota* to update ESP8266 via OTA, *esp32dev* to use and ESP32, .... )

* Load the Blynk project and hit PLAY button to start receiving data
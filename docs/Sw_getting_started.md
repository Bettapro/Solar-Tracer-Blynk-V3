# SW getting started

## 1. Prepare your IDE
Download the most recent version of this project https://github.com/Bettapro/Solar-Tracer-Blynk-V3/releases

Open you favorite IDE:
 - Arduino IDE:  open *SolarTracerBlynk* folder, then open *SolarTracerBlynk.ino* .
 - Platformio: import the project using *platformio.ini* .


## 2. Mobile App

Open the Blynk mobile app and create a new project by scanning the following QR code


### Legacy app (v3) - SUGGESTED
Current version of the app, complete overview of the solar charge controller.

![Project QR Code](../images/blynk-app-qr-code_v3.png)

### Legacy app (v2)
Original version of the mobile app, shows a small set of data.

![Project QR Code](../images/blynk-app-qr-code.png)

## 3. Edit `config.h` library
Open `SolarTracerBlynk\config.h`

* Enter your WiFi credentials in `WIFI_SSID` and `WIFI_PASS`.
* Send yourself the generated auth code from the mobile app
* Paste your auth code into `BLYNK_AUTH`

You can customize further more modifying config.h (eg. status LED, ntp time sync, blynk serve ...), each option is documented on the config.h file.
Please refer to the notes written on it.


## 4. Edit `config.h` library
Upload the sketch to your ESP8266 / ESP32, if you are using platformio you should edit `default_envs` accordingly (eg: *esp8266* to use a esp8266, *esp8266_ota* to update ESP8266 via OTA, *esp32dev* to use and ESP32, .... )

Click PLAY on you mobile app, you should start receiving updates from your solar tracer!.
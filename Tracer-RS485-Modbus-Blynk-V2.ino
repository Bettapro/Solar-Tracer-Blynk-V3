
// CONNECT THE RS485 MODULE.
// MAX485 module <-> ESP8266
//  - DI -> D10 / GPIO1 / TX
//  - RO -> D9 / GPIO3 / RX
//  - DE and RE are interconnected with a jumper and then connected do eighter pin D1 or D2
//  - VCC to +5V / VIN on ESP8266
//  - GNDs wired together
// -------------------------------------
// You do not need to disconnect the RS485 while uploading code.
// After first upload you should be able to upload over WiFi
// Tested on NodeMCU + MAX485 module
// RJ 45 cable: Green -> A, Blue -> B, Brown -> GND module + GND ESP8266
// MAX485: DE + RE interconnected with a jumper and connected to D1 or D2
//
// Developed by @jaminNZx
// With modifications by @tekk
#if defined ESP32
#define USE_WIFI_NINA false
#define USE_WIFI101 false

#include <WiFi.h>
// not needed ?
//#include <WiFiUdp.h>
//#include <DNSServer.h>
#include <WiFiWebServer.h>
// not needed ?
//#include <ESPmDNS.h>
#include <BlynkSimpleEsp32.h>
#elif defined ESP8266
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <BlynkSimpleEsp8266.h>
#else
#error This board is not supported.
#endif

#ifdef USE_ARDUINO_OTA
#include <ArduinoOTA.h>
#endif

#include "config.h"

#include "SolarTracer.h"
// should be include if tracer is epsolar/epever  
#include "EPEVERSolarTracer.hpp"




const int defaultBaudRate = 115200;
int timerTask1, timerTask2, timerTask3;


ModbusMaster node;
SolarTracer *thisController;
SimpleTimer timer;

// ****************************************************************************

void updateSolarController() {

	if (thisController->updateRun()) {
		Serial.println("Update Solar-Tracer SUCCESS!");
	} else {
		Serial.println("Update Solar-Tracer FAILED!");
	}
}

// upload values
void uploadRealtimeToBlynk() {
	Blynk.virtualWrite(vPIN_PV_POWER,
			thisController->getFloatValue(SolarTracerVariables::PV_POWER));
	Blynk.virtualWrite(vPIN_PV_CURRENT,
			thisController->getFloatValue(SolarTracerVariables::PV_CURRENT));
	Blynk.virtualWrite(vPIN_PV_VOLTAGE,
			thisController->getFloatValue(SolarTracerVariables::PV_VOLTAGE));
	Blynk.virtualWrite(vPIN_LOAD_CURRENT,
			thisController->getFloatValue(SolarTracerVariables::LOAD_CURRENT));
	Blynk.virtualWrite(vPIN_LOAD_POWER,
			thisController->getFloatValue(SolarTracerVariables::LOAD_POWER));
	Blynk.virtualWrite(vPIN_BATT_TEMP,
			thisController->getFloatValue(SolarTracerVariables::BATTERY_TEMP));
	Blynk.virtualWrite(vPIN_BATT_VOLTAGE,
			thisController->getFloatValue(
					SolarTracerVariables::BATTERY_VOLTAGE));
	Blynk.virtualWrite(vPIN_BATT_REMAIN,
			thisController->getFloatValue(SolarTracerVariables::BATTERY_SOC));
	Blynk.virtualWrite(vPIN_CONTROLLER_TEMP,
			thisController->getFloatValue(
					SolarTracerVariables::CONTROLLER_TEMP));
	Blynk.virtualWrite(vPIN_BATTERY_CHARGE_CURRENT,
			thisController->getFloatValue(
					SolarTracerVariables::BATTERY_CHARGE_CURRENT));
	Blynk.virtualWrite(vPIN_BATTERY_CHARGE_POWER,
			thisController->getFloatValue(
					SolarTracerVariables::BATTERY_CHARGE_POWER));
	Blynk.virtualWrite(vPIN_BATTERY_OVERALL_CURRENT,
			thisController->getFloatValue(
					SolarTracerVariables::BATTERY_OVERALL_CURRENT));
	Blynk.virtualWrite(vPIN_LOAD_ENABLED,
			thisController->getFloatValue(
					SolarTracerVariables::LOAD_MANUAL_ONOFF));
}

// upload values
void uploadStatsToBlynk() {
	Blynk.virtualWrite(vPIN_STAT_ENERGY_GENERATED_TODAY,
			thisController->getFloatValue(
					SolarTracerVariables::GENERATED_ENERGY_TODAY));
	Blynk.virtualWrite(vPIN_STAT_ENERGY_GENERATED_THIS_MONTH,
			thisController->getFloatValue(
					SolarTracerVariables::GENERATED_ENERGY_MONTH));
	Blynk.virtualWrite(vPIN_STAT_ENERGY_GENERATED_THIS_YEAR,
			thisController->getFloatValue(
					SolarTracerVariables::GENERATED_ENERGY_YEAR));
	Blynk.virtualWrite(vPIN_STAT_ENERGY_GENERATED_TOTAL,
			thisController->getFloatValue(
					SolarTracerVariables::GENERATED_ENERGY_TOTAL));
}

void setup() {

	Serial.begin(defaultBaudRate);

	// Modbus slave ID 1
	Serial2.begin(defaultBaudRate);
	thisController = new EPEVERSolarTracer(Serial2, MAX485_DE, MAX485_RE_NEG);

	// callbacks to toggle DE + RE on MAX485
	//node.preTransmission(preTransmission);
	//node.postTransmission(postTransmission);

	Serial.println(" ++ Setting up WIFI:");
	Serial.println("Connecting...");

	WiFi.mode(WIFI_STA);

#ifdef USE_BLYNK_LOCAL_SERVER
	Blynk.begin(BLYNK_AUTH, WIFI_SSID, WIFI_PASS, BLYNK_SERVER, BLYNK_PORT);
#else
  Blynk.begin(AUTH, WIFI_SSID, WIFI_PASS);
#endif

	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("Connection Failed! Rebooting...");
		delay(5000);
		ESP.restart();
	}

#ifdef USE_NTP_SERVER
	Serial.println(" ++ Setting up Local Time:");
	Serial.print("Waiting for NTP time...");
	configTime(0, 0, NTP_SERVER);
	setenv("TZ", TIMEZONE, 3);
	tzset();

	while (time(nullptr) < 100000ul) {
		Serial.print(".");
		delay(10);
	}
	Serial.println(" time synced");
#endif

	Serial.println("Connected.");

	Serial.println(" ++ Setting up Blynk:");
	Serial.print("Connecting...");

	while (!Blynk.connect()) {
		Serial.print(".");
		delay(100);
	}

	Serial.println();
	Serial.println("Connected to Blynk.");

#ifdef USE_ARDUINO_OTA


	Serial.println("Starting ArduinoOTA...");

	ArduinoOTA.setHostname(OTA_HOSTNAME);
	ArduinoOTA.setPassword((const char*) OTA_PASS);

	ArduinoOTA.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH) {
			type = "sketch";
		} else { // U_SPIFFS
			type = "filesystem";
		}

		// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
		Serial.println("Start updating " + type);
	});

	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd of update");
	});

	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});

	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) {
			Serial.println("Auth Failed");
		} else if (error == OTA_BEGIN_ERROR) {
			Serial.println("Begin Failed");
		} else if (error == OTA_CONNECT_ERROR) {
			Serial.println("Connect Failed");
		} else if (error == OTA_RECEIVE_ERROR) {
			Serial.println("Receive Failed");
		} else if (error == OTA_END_ERROR) {
			Serial.println("End Failed");
		}
	});

	ArduinoOTA.begin();

	Serial.print("ArduinoOTA running. ");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
#endif

	Serial.println("Starting timed actions...");

	timerTask1 = timer.setInterval(CONTROLLER_UPDATE_MS_PERIOD,
			updateSolarController);
	timerTask2 = timer.setInterval(BLINK_SYNC_STATS_MS_PERIOD,
			uploadStatsToBlynk);
	timerTask3 = timer.setInterval(BLINK_SYNC_REALTIME_MS_PERIOD,
			uploadRealtimeToBlynk);

	Serial.println(" ++ Setting up solar tracer:");
#ifdef TRACER_SYNC_TIME_WITH_CONTROLLER
	Serial.println("Set NTP time");
	getTimeFromServer();
	delay(500);
#endif
  Serial.println("Get all values");
	thisController->fetchAllValues();
	delay(500);
	uploadRealtimeToBlynk();
	uploadStatsToBlynk();

	delay(1000);
	Serial.println("Setup OK!");
	Serial.println("----------------------------");
	Serial.println();
}

// -------------------------------------------------------------------------------



void getTimeFromServer() {
	struct tm *ti;
  time_t tnow;
  char strftime_buf[64];

	tnow = time(nullptr) + 1;
	strftime(strftime_buf, sizeof(strftime_buf), "%c", localtime(&tnow));
	ti = localtime(&tnow);

	Serial.println("---------------------------------");
	Serial.println(ti->tm_year + 1900);
	Serial.println(ti->tm_mon + 1);
	Serial.println(ti->tm_mday);
	Serial.println(ti->tm_hour);
	Serial.println(ti->tm_min);
	Serial.println(ti->tm_sec);
	Serial.println("---------------------------------");

	thisController->syncRealtimeClock(ti);
}

// --------------------------------------------------------------------------------

// callback to on/off button state changes from the Blynk app
BLYNK_WRITE(vPIN_LOAD_ENABLED) {
	uint8_t newState = (uint8_t) param.asInt();

	Serial.print("Setting load state output coil to value: ");
	Serial.println(newState);

	if (thisController->writeBoolValue(SolarTracerVariables::LOAD_MANUAL_ONOFF,
			newState > 0)) {
		Serial.println("Write & Read suceeded.");
	} else {
		Serial.println("Write & Read failed.");
	}
	thisController->fetchValue(SolarTracerVariables::LOAD_MANUAL_ONOFF);

	Serial.println("Uploading results to Blynk.");

	uploadRealtimeToBlynk();
}

void loop() {
	Blynk.run();
#ifdef USE_ARDUINO_OTA
	ArduinoOTA.handle();
#endif
	timer.run();
}

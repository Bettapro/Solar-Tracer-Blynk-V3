
#pragma once 

#ifndef PROJECT_INCLUDE
#define PROJECT_INCLUDE

#include "project_core_config.h"

#if defined ESP32
#define USE_WIFI_NINA false
#define USE_WIFI101 false
#include <WiFi.h>
#elif defined ESP8266
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#else
#error This board is not supported.
#endif

#include <SimpleTimer.h>
//#define BlynkDebug_h
//#include <Blynk/BlynkTimer.h>
//#undef BlynkDebug_h

// BLYNK ?!

#ifdef USE_OTA_UPDATE
#include <ArduinoOTA.h>
#endif


#if defined USE_WIFI_AP_CONFIGURATION
#include <ArduinoJson.h>

#include <LittleFS.h>

// disable WM all logs
// #define WM_NODEBUG
#include <WiFiManager.h>
#endif

#if defined(USE_MQTT) && !defined(USE_MQTT_HOME_ASSISTANT)
#if defined(USE_MQTT_RPC_SUBSCRIBE) || defined(USE_MQTT_JSON_PUBLISH)
#include <ArduinoJson.h>
#endif
#include <PubSubClient.h>
#endif
#if defined(USE_MQTT) && defined(USE_MQTT_HOME_ASSISTANT)
#include <ArduinoHA.h>
#endif

#endif
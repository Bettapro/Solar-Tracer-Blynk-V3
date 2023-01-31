
#pragma once

#ifndef INCLUDE_ALL_LIB_H
#define INCLUDE_ALL_LIB_H

#include "include_all_core.h"

#if defined ESP32
#define USE_WIFI_NINA false
#define USE_WIFI101 false
#include <WiFi.h>
#elif defined ESP8266
#include <ESP8266WiFi.h>
#else
#error This board is not supported.
#endif

#include <SimpleTimer.h>

#ifdef USE_DOUBLE_RESET_TRIGGER
#include <LittleFS.h>
#define ESP_DRD_USE_LITTLEFS true
#include <ESP_DoubleResetDetector.h>
#endif

#ifdef USE_OTA_UPDATE
#include <ArduinoOTA.h>
#endif

#include <ArduinoJson.h>
#if defined USE_WIFI_AP_CONFIGURATION
#include <LittleFS.h>
// disable WM all logs
// #define WM_NODEBUG
#include <WiFiManager.h>
#endif

#if defined(USE_MQTT) && !defined(USE_MQTT_HOME_ASSISTANT)
#include <PubSubClient.h>
#endif
#if defined(USE_MQTT) && defined(USE_MQTT_HOME_ASSISTANT)
#include <ArduinoHA.h>
#endif

#ifdef USE_EXTERNAL_HEAVY_LOAD_CURRENT_METER
#ifdef USE_EXTERNAL_HEAVY_LOAD_CURRENT_METER_ADS1015_ADC
#include "ADS1X15.h"
#endif
#include <LinearSensHallCurrent.h>
#endif

#endif
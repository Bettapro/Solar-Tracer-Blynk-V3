#pragma once

#include <Arduino.h>

/**
 *  Main defs
 */
#include "solartracer_all.h"
#include "communication_protocol_all.h"
#include "status_all.h"

/**
 * Include user + board + solar tracer configs
 */
#include "../../config.h"
#include "../board/board_config.h"
#include "../solartracer/solartracer_config.h"


// reducing Blynk footprint
#define BLYNK_NO_BUILTIN   // Disable built-in analog & digital pin operations

/**
 * Conditional includes depending on the BOARD
 */
#if defined ESP32
    #define USE_WIFI_NINA false
    #define USE_WIFI101 false
    #include <WiFi.h>
    #include <BlynkSimpleEsp32.h>
#elif defined ESP8266
    #include <ESP8266WiFi.h>
    #include <DNSServer.h>
    #include <ESP8266WebServer.h>
    #include <ESP8266mDNS.h>
    #include <WiFiUdp.h>
    #include <BlynkSimpleEsp8266.h>
#endif




#include "../incl/variables_contains.h"

#ifdef USE_STATUS_LED
#include "../feature/status_led.h"
#endif

#include "../core/status.h"
#include "../core/debug.h"
#include "../core/timer.h"
#include "../core/controller.h"



#ifdef USE_OTA_UPDATE
#include <ArduinoOTA.h>
#include "../feature/arduino_ota.h"
#endif
#include "../feature/blynk_sync.h"



#include "../solartracer/SolarTracer.h"

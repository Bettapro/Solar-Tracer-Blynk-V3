#pragma once

#include <Arduino.h>

/**
 *  Main defs
 */
#include "incl/solartracer_all.h"
#include "incl/communication_protocol_all.h"
#include "incl/status_all.h"

/**
 * Include user + board + solar tracer configs
 */
#include "config.h"
#include "board/board_config.h"
#include "solartracer/solartracer_config.h"


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

#ifdef USE_OTA_UPDATE
#include <ArduinoOTA.h>
#endif


#include "solartracer/SolarTracer.h"

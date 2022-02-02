/**
 * Solar Tracer Blynk V3 [https://github.com/Bettapro/Solar-Tracer-Blynk-V3]
 * Copyright (c) 2021 Alberto Bettin 
 *
 * Based on the work of @jaminNZx and @tekk.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#pragma once


#define PROJECT_NAME "SolarTracer"
#define PROJECT_VERSION "v3.0.4"
#define PROJECT_SUBVERSION 1


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

#if defined ARDUINO_ARCH_ESP32
    #include "../board/esp32_config.h"
#elif defined ARDUINO_ARCH_ESP8266
    #include "../board/esp8266_config.h"
#else
    #error This board is not supported.
#endif

#if ( SOLAR_TRACER_MODEL == EPEVER_SOLAR_TRACER_A | SOLAR_TRACER_MODEL == EPEVER_SOLAR_TRACER_B | SOLAR_TRACER_MODEL == EPEVER_SOLAR_TRACER_TRITON | SOLAR_TRACER_MODEL == EPEVER_SOLAR_TRACER_XTRA )
    #include "../solartracer/epever/epever_config.h"
#else
    #error This Solar Tracer is not supported.
#endif

#include "../core/debug.h"

#if defined USE_WIFI_AP_CONFIGURATION
    #include <ArduinoJson.h>
    
    #include <LittleFS.h>
    #include "../incl/config_persistence.h"
#endif
#include "../core/environment.h"


// reducing Blynk footprint
#define BLYNK_NO_BUILTIN   // Disable built-in analog & digital pin operations

/**
 * Conditional includes depending on the BOARD
 */
#if defined ESP32
    #define USE_WIFI_NINA false
    #define USE_WIFI101 false
    #include <WiFi.h>
    #if defined USE_BLYNK
        #include <BlynkSimpleEsp32.h>
    #endif
#elif defined ESP8266
    #include <ESP8266WiFi.h>
    #include <DNSServer.h>
    #include <ESP8266WebServer.h>
    #include <ESP8266mDNS.h>
    #include <WiFiUdp.h>
    #if defined USE_BLYNK
        #include <BlynkSimpleEsp8266.h>
    #endif
#endif

#ifndef USE_BLYNK
    #include <SimpleTimer.h>
#endif

#include "../incl/variables_contains.h"

#ifdef USE_STATUS_LED
    #include "../feature/status_led.h"
#endif

#include "../core/status.h"
#include "../core/timer.h"
#include "../core/controller.h"
#include "../core/datetime.h"

#ifdef USE_OTA_UPDATE
    #include <ArduinoOTA.h>
    #include "../feature/arduino_ota.h"
#endif

#if defined USE_WIFI_AP_CONFIGURATION
    // disable WM all logs
    // #define WM_NODEBUG
    #include <WiFiManager.h> 

    #include "../feature/wifi_manager.h"
#endif

#if defined USE_BLYNK
    #include "../feature/blynk_sync.h"
#endif
#if defined(USE_MQTT) && !defined(USE_MQTT_HOME_ASSISTANT)
    #if defined(USE_MQTT_RPC_SUBSCRIBE) || defined(USE_MQTT_JSON_PUBLISH)
        #include <ArduinoJson.h>
    #endif
    #include <PubSubClient.h>
    #include "../feature/mqtt_sync.h"
#endif
#if defined(USE_MQTT) && defined(USE_MQTT_HOME_ASSISTANT)
    
    

    #include <ArduinoHA.h>
    #include "../feature/mqtt_homeassistant_sync.h"
#endif

#include "../solartracer/SolarTracer.h"


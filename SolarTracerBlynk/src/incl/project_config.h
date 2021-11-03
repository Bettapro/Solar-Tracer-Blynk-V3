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

 #include <SimpleTimer.h>

#include "../incl/variables_contains.h"

#ifdef USE_STATUS_LED
    #include "../feature/status_led.h"
#endif

#include "../core/status.h"
#include "../core/debug.h"
#include "../core/timer.h"
#include "../core/controller.h"

#if defined USE_WIFI_AP_CONFIGURATION
    #include <ArduinoJson.h>
    
    #include <LittleFS.h>
    #include "../incl/config_persistence.h"
#endif

#include "../core/environment.h"

#ifdef USE_OTA_UPDATE
    #include <ArduinoOTA.h>
    #include "../feature/arduino_ota.h"
#endif

#if defined USE_WIFI_AP_CONFIGURATION
    #include <WiFiManager.h> 

    #include "../feature/wifi_manager.h"
#endif

#if defined USE_BLYNK
    #include "../feature/blynk_sync.h"
#endif
#if defined USE_MQTT
    #include <PubSubClient.h>
    #include "../feature/mqtt_sync.h"
#endif

#include "../solartracer/SolarTracer.h"


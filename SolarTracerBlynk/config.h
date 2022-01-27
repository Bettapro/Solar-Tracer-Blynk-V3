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

/**
 *  
 * Use this file to set parameters and settings for your 
 * setup.
 *
 */

#pragma once

/**
 * DEBUG MESSAGES
 * 
 * Support for debug messages via serial
 */
// specify which Serial to use for debug messages
//#define BOARD_DEBUG_SERIAL_STREAM Serial
// set accordingly: monitor_speed in platformio.ini
//#define BOARD_DEBUG_SERIAL_STREAM_BAUDRATE 115200

/**
 * STATUS
 * 
 * Define how/if the board should notify an error
 */
// blink a led when errors occur
//#define USE_STATUS_LED
#ifdef USE_STATUS_LED
  //specify pin number connected to the status led
  #define STATUS_LED_PIN 2
#endif

/**
 * SOLAR CHARGE CONTROLLER 
 * 
 * Define the communication parameters for the
 * solar tracer
 */
// solar tracer model
#define SOLAR_TRACER_MODEL EPEVER_SOLAR_TRACER_A
// use serial as communication interfce
#define USE_SERIAL_STREAM
#ifdef USE_SERIAL_STREAM
  // specify the serial port to use 
  //#define BOARD_ST_SERIAL_STREAM Serial2
  // specify the baudrate to use (leave commented to use the correct based on you solar tracer)
  //#define BOARD_ST_SERIAL_STREAM_BAUDRATE 115200 

  // ms to wait for a response from controller, then a timeout error will be triggered
  //#define SERIAL_COMMUNICATION_TIMEOUT 2000

  // specify your rx pin
  //#define BOARD_ST_SERIAL_PIN_MAPPING_RX 16
  // specify your tx pin
  //#define BOARD_ST_SERIAL_PIN_MAPPING_TX 17

  #define USE_SERIAL_MAX485
  #ifdef USE_SERIAL_MAX485
    // specify the pin num. connected to DE
    //#define MAX485_DE 13
    // specify the pin num. connected to RE_NEG
    //#define MAX485_RE_NEG 13
  #endif
#endif

#define SOLAR_TRACER_COMMUNICATION_PROTOCOL COMMUNICATION_PROTOCOL_MODBUS_RTU
#if SOLAR_TRACER_COMMUNICATION_PROTOCOL == COMMUNICATION_PROTOCOL_MODBUS_RTU
  //#define MODBUS_SLAVE_ID 1
#endif

// How many ms between each refresh request 
//#define CONTROLLER_UPDATE_MS_PERIOD 2000L

 /*
  * TIME SYNC
  */
// uncomment if the ESP should get the time from a NTP server at boot 
#define USE_NTP_SERVER 
#ifdef USE_NTP_SERVER
  // uncomment if the controller has to sync its time with the tracer at boot
  #define SYNC_ST_TIME
  // specify the preferred NTP server
  #define NTP_SERVER_CONNECT_TO "europe.pool.ntp.org"
  // specify your timezone (refer to: https://sites.google.com/a/usapiens.com/opnode/time-zones )
  #define CURRENT_TIMEZONE  "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"
#endif


/*
 * WIFI

 * Fill with you wifi auth. data
 */
// your wifi SSID
#define WIFI_SSID             "-your-wifi-"
// your wifi PASSWORD
#define WIFI_PASS             "-your-password-"


/*
 * OTA UPDATE
 */
// uncomment to enable OTA updates
#define USE_OTA_UPDATE
#ifdef USE_OTA_UPDATE
	//Over The Air Hostname, set accordingly: upload_port in platformio.ini
	#define OTA_HOSTNAME "SOLAR-MODBUS"
  // OTA password, set accordingly: upload_flags --auth in platformio.ini
	#define OTA_PASS "admin"
#endif

/**
 *  SETTING SETUP OVER WEB PAGE
 * 
 *  The board will enter in this mode if:
 *    - it cannot connect to wifi network
 *    - USE_HALL_AP_CONFIGURATION_TRIGGER is enabled and magnet is near to ESP module
 *    - USE_PIN_AP_CONFIGURATION_TRIGGER is enabled and the pin is HIGH (or LOW)
 */
#define USE_WIFI_AP_CONFIGURATION
#if defined USE_WIFI_AP_CONFIGURATION
  // ap name
  #define WIFI_AP_CONFIGURATION_HOSTNAME "SolarTracerAP"
  // ap password
  #define WIFI_AP_CONFIGURATION_PASSWORD "admin1234"

  // use hall sensor to trigger ap mode
  //#define USE_HALL_AP_CONFIGURATION_TRIGGER
  #if defined USE_HALL_AP_CONFIGURATION_TRIGGER
    // standard value (no interaction)
    //#define HALL_AP_CONFIGURATION_BASE_VALUE 75
    // threshoold to detect interraction
    //#define HALL_AP_CONFIGURATION_THR_VALUE 30
  #endif

  // use pin state to trigger ap mode
  #define USE_PIN_AP_CONFIGURATION_TRIGGER
  #if defined USE_PIN_AP_CONFIGURATION_TRIGGER
    // pin number to use
    #define PIN_AP_TRIGGER_PIN 19
    // value to consider the pin "triggered"
    #define PIN_AP_TRIGGER_VALUE HIGH
  #endif

  // use pin to reset settings
  #define USE_PIN_RESET_CONFIGURATION_TRIGGER
  #if defined USE_PIN_RESET_CONFIGURATION_TRIGGER
    // pin number to use
    #define PIN_RESET_TRIGGER_PIN 18
    // value to consider the pin "triggered"
    #define PIN_RESET_TRIGGER_VALUE HIGH
  #endif
#endif

// How many ms between each realtime sync to blynk server
#define SYNC_REALTIME_MS_PERIOD 2000L
// How many ms between each stat sync to blynk server
#define SYNC_STATS_MS_PERIOD 720000L




/*
 * BLYNK
 * Specify how to connect to blynk server and when to sync the data from the tracer
 */

#define USE_BLYNK
#if defined (USE_BLYNK)

  // uncomment to use blynk 2.0
  //#define USE_BLYNK_2
  #ifdef USE_BLYNK_2
    // your project template id
    #define BLYNK_TEMPLATE_ID "-template-id-"
    // device name
    #define BLYNK_DEVICE_NAME "SolarTracer"
  #endif

  // Blynk API key
  #define BLYNK_AUTH "-blynk-aut-"

  // uncomment to use an user-defined blynk server
  //#define USE_BLYNK_LOCAL_SERVER
  #ifdef USE_BLYNK_LOCAL_SERVER
    //address of blynk server (specify the hostname or the ip address eg: IPAddress(192, 168, 1, 78) )
    #define BLYNK_SERVER                  ""
    // port number of the blynk server (eg: 8080)
    #define BLYNK_PORT                  8080
  #endif

  // ESP will wait indefinitely for blynk connection
  #define BLYNK_CONNECTION_REQUIRED

  // virtual pins definition and mapping

  #define vPIN_PV_POWER                                   V1
  #define vPIN_PV_CURRENT                                 V2
  #define vPIN_PV_VOLTAGE                                 V3
  #define vPIN_LOAD_CURRENT                               V4
  #define vPIN_LOAD_POWER                                 V5
  #define vPIN_BATT_TEMP                                  V6
  #define vPIN_BATT_VOLTAGE                               V7
  #define vPIN_BATT_REMAIN                                V8
  #define vPIN_CONTROLLER_TEMP                            V9
  #define vPIN_BATTERY_CHARGE_CURRENT                     V10
  #define vPIN_BATTERY_CHARGE_POWER                       V11
  #define vPIN_BATTERY_OVERALL_CURRENT                    V12
  #define vPIN_LOAD_ENABLED                               V14
  #define vPIN_STAT_ENERGY_GENERATED_TODAY                V15
  #define vPIN_STAT_ENERGY_GENERATED_THIS_MONTH           V16
  #define vPIN_STAT_ENERGY_GENERATED_THIS_YEAR            V17
  #define vPIN_STAT_ENERGY_GENERATED_TOTAL                V18
  #define vPIN_MIN_BATTERY_VOLTAGE_TODAY                  V19
  #define vPIN_MAX_BATTERY_VOLTAGE_TODAY                  V20
  #define vPIN_BATTERY_STATUS_TEXT                        V23
  #define vPIN_CHARGING_EQUIPMENT_STATUS_TEXT             V24
  #define vPIN_DISCHARGING_EQUIPMENT_STATUS_TEXT          V25
  #define vPIN_CHARGE_DEVICE_ENABLED                      V26
  #define vPIN_CONTROLLER_HEATSINK_TEMP                   V29
  #define vPIN_BATTERY_BOOST_VOLTAGE                      V30
  #define vPIN_BATTERY_EQUALIZATION_VOLTAGE               V31
  #define vPIN_BATTERY_FLOAT_VOLTAGE                      V32
  #define vPIN_BATTERY_FLOAT_MIN_VOLTAGE                  V33
  #define vPIN_BATTERY_CHARGING_LIMIT_VOLTAGE             V34
  #define vPIN_BATTERY_DISCHARGING_LIMIT_VOLTAGE          V35
  #define vPIN_BATTERY_LOW_VOLTAGE_DISCONNECT             V36
  #define vPIN_BATTERY_LOW_VOLTAGE_RECONNECT              V37
  #define vPIN_BATTERY_OVER_VOLTAGE_DISCONNECT            V38
  #define vPIN_BATTERY_OVER_VOLTAGE_RECONNECT             V39
  #define vPIN_BATTERY_UNDER_VOLTAGE_RESET                V40
  #define vPIN_BATTERY_UNDER_VOLTAGE_SET                  V41
  #define vPIN_MIN_PV_VOLTAGE_TODAY                       V42
  #define vPIN_MAX_PV_VOLTAGE_TODAY                       V43
  // internal
  #define vPIN_INTERNAL_STATUS                            V27
  //#define vPIN_INTERNAL_DEBUG_TERMINAL                    V44
  //action
  #define vPIN_UPDATE_ALL_CONTROLLER_DATA                 V28
  #define vPIN_UPDATE_CONTROLLER_DATETIME                 V45
#endif


/*
 * MQTT
 * Specify how to connect to mqtt broker and when to sync the data from the tracer
 * 
 * NOTE: mqtt support is till a WIP, it supports a small set of the features and variable compared
 *        to blynk. Developed and tested using a thingspeak server and mosquitto broker
 */
//#define USE_MQTT
#if defined (USE_MQTT)
  //address of mqtt server (specify the hostname or the ip address eg: IPAddress(192, 168, 1, 78) )
  #define MQTT_SERVER "127.0.0.1"
  // port number of the mqtt server (eg: 1883)
  #define MQTT_PORT 1883
  // username to use 
  //#define MQTT_USERNAME "solartracer"
  // password to use in combination with MQTT_USERNAME
  //#define MQTT_PASSWORD "solartracer"
  // client id
  #define MQTT_CLIENT_ID "solarTracer1"

  // all the "variable topics" will be published in JSON on a single topic defined in MQTT_JSON_PUBLISH_TOPIC
  //#define USE_MQTT_JSON_PUBLISH
   #ifdef USE_MQTT_JSON_PUBLISH
    #define MQTT_JSON_PUBLISH_TOPIC "v1/devices/me/telemetry"
  #endif
  
  // use rpc to send control messages to this board (early stage support)
  //#define USE_MQTT_RPC_SUBSCRIBE
  #ifdef USE_MQTT_RPC_SUBSCRIBE
    #define MQTT_RPC_SUBSCRIBE_TOPIC "v1/devices/me/rpc/request/+"
  #endif

  // topics definition and mapping
  #define MQTT_TOPIC_PV_POWER                                  "pv/power"
  #define MQTT_TOPIC_PV_CURRENT                                "pv/current"
  #define MQTT_TOPIC_PV_VOLTAGE                                "pv/voltage"
  #define MQTT_TOPIC_LOAD_CURRENT                              "load/current"
  #define MQTT_TOPIC_LOAD_POWER                                "load/power"
  #define MQTT_TOPIC_BATT_TEMP                                 "battery/power"
  #define MQTT_TOPIC_BATT_VOLTAGE                              "battery/voltage"
  #define MQTT_TOPIC_BATT_REMAIN                               "battery/SOC"
  #define MQTT_TOPIC_CONTROLLER_TEMP                           "controller/temperature"
  #define MQTT_TOPIC_BATTERY_CHARGE_CURRENT                    "battery/chargeCurrent"
  #define MQTT_TOPIC_BATTERY_CHARGE_POWER                      "battery/chargePower"
  #define MQTT_TOPIC_BATTERY_OVERALL_CURRENT                   "battery/overallCurrent"
  #define MQTT_TOPIC_LOAD_ENABLED                              "controller/loadEnabled"
  #define MQTT_TOPIC_STAT_ENERGY_GENERATED_TODAY               "stats/production/day"
  #define MQTT_TOPIC_STAT_ENERGY_GENERATED_THIS_MONTH          "stats/production/month"
  #define MQTT_TOPIC_STAT_ENERGY_GENERATED_THIS_YEAR           "stats/production/year"
  #define MQTT_TOPIC_STAT_ENERGY_GENERATED_TOTAL               "stats/production/total"
  #define MQTT_TOPIC_MIN_BATTERY_VOLTAGE_TODAY                 "stats/battery/maxVoltage"
  #define MQTT_TOPIC_MAX_BATTERY_VOLTAGE_TODAY                 "stats/battery/minVoltage"
  #define MQTT_TOPIC_BATTERY_STATUS_TEXT                       "battery/statusText"
  #define MQTT_TOPIC_CHARGING_EQUIPMENT_STATUS_TEXT            "pv/statusText"
  #define MQTT_TOPIC_DISCHARGING_EQUIPMENT_STATUS_TEXT         "load/statusText"
  #define MQTT_TOPIC_CHARGE_DEVICE_ENABLED                     "controller/chargeEnabled"
  #define MQTT_TOPIC_CONTROLLER_HEATSINK_TEMP                  "controller/heatsinkTemperature"
  #define MQTT_TOPIC_BATTERY_BOOST_VOLTAGE                     "battery/settings/boostVoltage"
  #define MQTT_TOPIC_BATTERY_EQUALIZATION_VOLTAGE              "battery/settings/equalizationVoltage"
  #define MQTT_TOPIC_BATTERY_FLOAT_VOLTAGE                     "battery/settings/floatVoltage"
  #define MQTT_TOPIC_BATTERY_FLOAT_MIN_VOLTAGE                 "battery/settings/floatMinVoltage"
  #define MQTT_TOPIC_BATTERY_CHARGING_LIMIT_VOLTAGE            "battery/settings/charginLimitVoltage"
  #define MQTT_TOPIC_BATTERY_DISCHARGING_LIMIT_VOLTAGE         "battery/settings/discharginLimitVoltage"
  #define MQTT_TOPIC_BATTERY_LOW_VOLTAGE_DISCONNECT            "battery/settings/lowVoltageDisconnect"
  #define MQTT_TOPIC_BATTERY_LOW_VOLTAGE_RECONNECT             "battery/settings/loVoltageReconnect"
  #define MQTT_TOPIC_BATTERY_OVER_VOLTAGE_DISCONNECT           "battery/settings/overVoltageDisconnect"
  #define MQTT_TOPIC_BATTERY_OVER_VOLTAGE_RECONNECT            "battery/settings/overVoltageReconnect"
  #define MQTT_TOPIC_BATTERY_UNDER_VOLTAGE_RESET               "battery/settings/underVoltageReset"
  #define MQTT_TOPIC_BATTERY_UNDER_VOLTAGE_SET                 "battery/settings/underVoltageSet"
  #define MQTT_TOPIC_MIN_PV_VOLTAGE_TODAY                      "stats/pv/maxVoltage"
  #define MQTT_TOPIC_MAX_PV_VOLTAGE_TODAY                      "stats/pv/minVoltage"
  // internal
  #define MQTT_TOPIC_INTERNAL_STATUS                           "internal/status"
  //action
  #define MQTT_TOPIC_UPDATE_ALL_CONTROLLER_DATA                "internal/updateAll"
#endif
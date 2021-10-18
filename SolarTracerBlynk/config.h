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
// bink a led when errors occur
#define USE_STATUS_LED
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

  // specify your rx pin
  //#define BOARD_ST_SERIAL_PIN_MAPPING_RX 16
  // specify your tx pin
  //#define BOARD_ST_SERIAL_PIN_MAPPING_TX 17

  #define USE_SERIAL_MAX485
  #ifdef USE_SERIAL_MAX485
    // specify the pin num. connected to DE
    //#define MAX485_DE 18
    // specify the pin num. connected to RE_NEG
    //#define MAX485_RE_NEG 18
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

// Blynk API key
#define BLYNK_AUTH "-blynk-aut-"

// uncomment to us an user-defined blynk server
//#define USE_BLYNK_LOCAL_SERVER
#ifdef USE_BLYNK_LOCAL_SERVER
  //address of blynk server (specify the hostname or the ip address eg: IPAddress(192, 168, 1, 78) )
  #define BLYNK_SERVER                  ""
  // port number of the blynk server (eg: 8080)
  #define BLYNK_PORT                  8080
#endif



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
// internal
#define vPIN_INTERNAL_STATUS                            V27
//action
#define vPIN_UPDATE_ALL_CONTROLLER_DATA                 V28


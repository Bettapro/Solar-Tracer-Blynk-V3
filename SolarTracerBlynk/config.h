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
// debug will print a warning line for each un-synced variable
//#define USE_DEBUG_SERIAL_VERBOSE_SYNC_ERROR_VARIABLE 

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

  // ms to wait before sending any request
  //#define BOARD_ST_SERIAL_PRETRANSMIT_WAIT 0
  
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

//#define USE_EXTERNAL_HEAVY_LOAD_CURRENT_METER
#ifdef USE_EXTERNAL_HEAVY_LOAD_CURRENT_METER
  // voltage reading are taken using ESP adcs
  //#define USE_EXTERNAL_HEAVY_LOAD_CURRENT_ADC
  #ifdef USE_EXTERNAL_HEAVY_LOAD_CURRENT_ADC
    #define ADC_LOAD_CURRENT_PIN 36
  #endif

  // voltage reading by I2c ADS1015 
  #define USE_EXTERNAL_HEAVY_LOAD_CURRENT_METER_ADS1015_ADC
  #ifdef USE_EXTERNAL_HEAVY_LOAD_CURRENT_METER_ADS1015_ADC
    #define ADS1015_LOAD_CURRENT_CHANNEL 0
  #endif

  // number of samples to get each run
  #define EXTERNAL_HEAVY_LOAD_CURRENT_METER_SAMPLES_NUMBER 4
  // milliseconds between 2 reading
  #define EXTERNAL_HEAVY_LOAD_CURRENT_METER_SAMPLE_INTERVAL 150
  // V measured at 0 ampere flowing to the load
  #define EXTERNAL_HEAVY_LOAD_CURRENT_METER_VOLTAGE_ZERO_AMP_VOLT 0.009
  // Volt/Ampere ratio
  #define EXTERNAL_HEAVY_LOAD_CURRENT_METER_VOLTAGE_AMP_VOLT 4/100.0
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
// your wifi IP ADDRESS
#define WIFI_IP_ADDRESS       ""
// your wifi GATEWAY
#define WIFI_GATEWAY          ""
// your wifi SUBNET
#define WIFI_SUBNET           ""
// your wifi DNS1
#define WIFI_DNS1             ""
// your wifi DNS2
#define WIFI_DNS2             ""


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

  // use a double reset to trigger ap mode
  #define USE_DOUBLE_RESET_TRIGGER

  // use pin state to trigger ap mode
  //#define USE_PIN_AP_CONFIGURATION_TRIGGER
  #if defined USE_PIN_AP_CONFIGURATION_TRIGGER
    // pin number to use
    //#define PIN_AP_TRIGGER_PIN 19
    // value to consider the pin "triggered"
    #define PIN_AP_TRIGGER_VALUE HIGH
  #endif

  // use pin to reset settings
  //#define USE_PIN_RESET_CONFIGURATION_TRIGGER
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
#ifdef USE_NOT_BLYNK
  #undef USE_BLYNK
#endif
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

  #define vPIN_PV_POWER                                   1
  #define vPIN_PV_CURRENT                                 2
  #define vPIN_PV_VOLTAGE                                 3
  #define vPIN_LOAD_CURRENT                               4
  #define vPIN_LOAD_POWER                                 5
  #define vPIN_BATT_TEMP                                  6
  #define vPIN_BATT_VOLTAGE                               7
  #define vPIN_BATT_REMAIN                                8
  #define vPIN_CONTROLLER_TEMP                            9
  #define vPIN_BATTERY_CHARGE_CURRENT                     10
  #define vPIN_BATTERY_CHARGE_POWER                       11
  #define vPIN_BATTERY_OVERALL_CURRENT                    12
  #define vPIN_LOAD_ENABLED                               14
  #define vPIN_STAT_ENERGY_GENERATED_TODAY                15
  #define vPIN_STAT_ENERGY_GENERATED_THIS_MONTH           16
  #define vPIN_STAT_ENERGY_GENERATED_THIS_YEAR            17
  #define vPIN_STAT_ENERGY_GENERATED_TOTAL                18
  #define vPIN_MIN_BATTERY_VOLTAGE_TODAY                  19
  #define vPIN_MAX_BATTERY_VOLTAGE_TODAY                  20
  #define vPIN_BATTERY_STATUS_TEXT                        23
  #define vPIN_CHARGING_EQUIPMENT_STATUS_TEXT             24
  #define vPIN_DISCHARGING_EQUIPMENT_STATUS_TEXT          25
  #define vPIN_CHARGE_DEVICE_ENABLED                      26
  #define vPIN_CONTROLLER_HEATSINK_TEMP                   29
  #define vPIN_BATTERY_BOOST_VOLTAGE                      30
  #define vPIN_BATTERY_EQUALIZATION_VOLTAGE               31
  #define vPIN_BATTERY_FLOAT_VOLTAGE                      32
  #define vPIN_BATTERY_FLOAT_MIN_VOLTAGE                  33
  #define vPIN_BATTERY_CHARGING_LIMIT_VOLTAGE             34
  #define vPIN_BATTERY_DISCHARGING_LIMIT_VOLTAGE          35
  #define vPIN_BATTERY_LOW_VOLTAGE_DISCONNECT             36
  #define vPIN_BATTERY_LOW_VOLTAGE_RECONNECT              37
  #define vPIN_BATTERY_OVER_VOLTAGE_DISCONNECT            38
  #define vPIN_BATTERY_OVER_VOLTAGE_RECONNECT             39
  #define vPIN_BATTERY_UNDER_VOLTAGE_RESET                40
  #define vPIN_BATTERY_UNDER_VOLTAGE_SET                  41
  #define vPIN_MIN_PV_VOLTAGE_TODAY                       42
  #define vPIN_MAX_PV_VOLTAGE_TODAY                       43
  #define vPIN_BATTERY_RATED_VOLTAGE                      46
  #define vPIN_BATTERY_TYPE                               47
  #define vPIN_BATTERY_CAPACITY                           48
  #define vPIN_BATTERY_EQUALIZATION_DURATION              49
  #define vPIN_BATTERY_BOOST_DURATION                     50
  #define vPIN_BATTERY_TEMPERATURE_COMPENSATION_COEFF     51
  #define vPIN_BATTERY_MANAGEMENT_MODE                    52
  #define vPIN_STAT_ENERGY_CONSUMED_TOTAL                 53
  // internal
  #define vPIN_INTERNAL_STATUS                            27
  #define vPIN_INTERNAL_DEBUG_TERMINAL                    44
  //action
  #define vPIN_UPDATE_ALL_CONTROLLER_DATA                 28
  #define vPIN_UPDATE_CONTROLLER_DATETIME                 45
#endif


/*
 * MQTT
 * Specify how to connect to mqtt broker and when to sync the data from the tracer
 * 
 * NOTE: mqtt support is till a WIP, it supports a small set of the features and variable compared
 *        to blynk. Developed and tested using a thingspeak server and mosquitto broker
 */
//#define USE_MQTT
#ifdef USE_NOT_MQTT
  #undef USE_MQTT
#endif
#if defined (USE_MQTT)
  //address of mqtt server (specify the hostname or the ip address eg: IPAddress(192, 168, 1, 78) )
  #define MQTT_SERVER "127.0.0.1"
  // port number of the mqtt server (eg: 1883)
  #define MQTT_PORT 1883
  // username to use 
  #define MQTT_USERNAME "solartracer"
  // password to use in combination with MQTT_USERNAME
  #define MQTT_PASSWORD "solar123"
  // client id
  #define MQTT_CLIENT_ID "solarTracer1"

  #define USE_MQTT_HOME_ASSISTANT
  #ifdef USE_MQTT_HOME_ASSISTANT
    #define MQTT_HOME_ASSISTANT_DEVICE_NAME "SolarTracer1"
  #endif

  // all the "variable topics" will be published in JSON on a single topic defined in MQTT_JSON_PUBLISH_TOPIC
  //#define USE_MQTT_JSON_PUBLISH
  #if defined(USE_MQTT_JSON_PUBLISH) && ! defined(USE_MQTT_HOME_ASSISTANT)
    #define MQTT_JSON_PUBLISH_TOPIC "v1/devices/me/telemetry"
  #endif
  
  // use rpc to send control messages to this board (early stage support)
  //#define USE_MQTT_RPC_SUBSCRIBE
  #if defined(USE_MQTT_RPC_SUBSCRIBE) && ! defined(USE_MQTT_HOME_ASSISTANT)
    #define MQTT_RPC_SUBSCRIBE_TOPIC "v1/devices/me/rpc/request/+"
  #endif

  #if !defined(USE_MQTT_HOME_ASSISTANT)  && ! defined(USE_MQTT_HOME_ASSISTANT)
    #define MQTT_TOPIC_ROOT "solarTracer/values/"
  #else
    // no need to have a topic root
    #define MQTT_TOPIC_ROOT ""
  #endif

  // topics definition and mapping
  #define MQTT_TOPIC_PV_POWER                                 MQTT_TOPIC_ROOT "pv_power"
  #define MQTT_TOPIC_PV_CURRENT                               MQTT_TOPIC_ROOT "pv_current"
  #define MQTT_TOPIC_PV_VOLTAGE                               MQTT_TOPIC_ROOT "pv_voltage"
  #define MQTT_TOPIC_LOAD_CURRENT                             MQTT_TOPIC_ROOT "load_current"
  #define MQTT_TOPIC_LOAD_POWER                               MQTT_TOPIC_ROOT "load_power"
  #define MQTT_TOPIC_BATT_TEMP                                MQTT_TOPIC_ROOT "battery_temperature"
  #define MQTT_TOPIC_BATT_VOLTAGE                             MQTT_TOPIC_ROOT "battery_voltage"
  #define MQTT_TOPIC_BATT_REMAIN                              MQTT_TOPIC_ROOT "battery_soc"
  #define MQTT_TOPIC_CONTROLLER_TEMP                          MQTT_TOPIC_ROOT "controller_temperature"
  #define MQTT_TOPIC_BATTERY_CHARGE_CURRENT                   MQTT_TOPIC_ROOT "battery_charging_current"
  #define MQTT_TOPIC_BATTERY_CHARGE_POWER                     MQTT_TOPIC_ROOT "battery_charging_power"
  #define MQTT_TOPIC_BATTERY_OVERALL_CURRENT                  MQTT_TOPIC_ROOT "battery_overall_current"
  #define MQTT_TOPIC_LOAD_ENABLED                             MQTT_TOPIC_ROOT "controller_load_enabled"
  #define MQTT_TOPIC_STAT_ENERGY_GENERATED_TODAY              MQTT_TOPIC_ROOT "stats_production_day"
  #define MQTT_TOPIC_STAT_ENERGY_GENERATED_THIS_MONTH         MQTT_TOPIC_ROOT "stats_production_month"
  #define MQTT_TOPIC_STAT_ENERGY_GENERATED_THIS_YEAR          MQTT_TOPIC_ROOT "stats_production_year"
  #define MQTT_TOPIC_STAT_ENERGY_GENERATED_TOTAL              MQTT_TOPIC_ROOT "stats_production_total"
  #define MQTT_TOPIC_MIN_BATTERY_VOLTAGE_TODAY                MQTT_TOPIC_ROOT "stats_battery_max_voltage"
  #define MQTT_TOPIC_MAX_BATTERY_VOLTAGE_TODAY                MQTT_TOPIC_ROOT "stats_battery_min_voltage"
  #define MQTT_TOPIC_BATTERY_STATUS_TEXT                      MQTT_TOPIC_ROOT "battery_status_text"
  #define MQTT_TOPIC_CHARGING_EQUIPMENT_STATUS_TEXT           MQTT_TOPIC_ROOT "pv_status_text"
  #define MQTT_TOPIC_DISCHARGING_EQUIPMENT_STATUS_TEXT        MQTT_TOPIC_ROOT "load_status_text"
  #define MQTT_TOPIC_CHARGE_DEVICE_ENABLED                    MQTT_TOPIC_ROOT "controller_charge_enabled"
  #define MQTT_TOPIC_CONTROLLER_HEATSINK_TEMP                 MQTT_TOPIC_ROOT "controller_heatsink_temperature"
  #define MQTT_TOPIC_BATTERY_BOOST_VOLTAGE                    MQTT_TOPIC_ROOT "battery_settings_boost_voltage"
  #define MQTT_TOPIC_BATTERY_EQUALIZATION_VOLTAGE             MQTT_TOPIC_ROOT "battery_settings_equalization_voltage"
  #define MQTT_TOPIC_BATTERY_FLOAT_VOLTAGE                    MQTT_TOPIC_ROOT "battery_settings_float_voltage"
  #define MQTT_TOPIC_BATTERY_FLOAT_MIN_VOLTAGE                MQTT_TOPIC_ROOT "battery_settings_float_min_voltage"
  #define MQTT_TOPIC_BATTERY_CHARGING_LIMIT_VOLTAGE           MQTT_TOPIC_ROOT "battery_settings_charging_limit_voltage"
  #define MQTT_TOPIC_BATTERY_DISCHARGING_LIMIT_VOLTAGE        MQTT_TOPIC_ROOT "battery_settings_discharging_limit_voltage"
  #define MQTT_TOPIC_BATTERY_LOW_VOLTAGE_DISCONNECT           MQTT_TOPIC_ROOT "battery_settings_low_voltage_disconnect"
  #define MQTT_TOPIC_BATTERY_LOW_VOLTAGE_RECONNECT            MQTT_TOPIC_ROOT "battery_settings_low_voltage_reconnect"
  #define MQTT_TOPIC_BATTERY_OVER_VOLTAGE_DISCONNECT          MQTT_TOPIC_ROOT "battery_settings_over_voltage_disconnect"
  #define MQTT_TOPIC_BATTERY_OVER_VOLTAGE_RECONNECT           MQTT_TOPIC_ROOT "battery_settings_over_voltage_reconnect"
  #define MQTT_TOPIC_BATTERY_UNDER_VOLTAGE_RESET              MQTT_TOPIC_ROOT "battery_settings_under_voltage_reset"
  #define MQTT_TOPIC_BATTERY_UNDER_VOLTAGE_SET                MQTT_TOPIC_ROOT "battery_settings_under_voltage_set"
  #define MQTT_TOPIC_MIN_PV_VOLTAGE_TODAY                     MQTT_TOPIC_ROOT "stats_pv_max_voltage"
  #define MQTT_TOPIC_MAX_PV_VOLTAGE_TODAY                     MQTT_TOPIC_ROOT "stats_pv_min_voltage"
  #define MQTT_TOPIC_BATTERY_RATED_VOLTAGE                    MQTT_TOPIC_ROOT "battery_settings_rated_voltage"
  #define MQTT_TOPIC_BATTERY_TYPE                             MQTT_TOPIC_ROOT "battery_settings_type"
  #define MQTT_TOPIC_BATTERY_CAPACITY                         MQTT_TOPIC_ROOT "battery_settings_capacity"
  #define MQTT_TOPIC_BATTERY_EQUALIZATION_DURATION            MQTT_TOPIC_ROOT "battery_settings_equalization_duration"
  #define MQTT_TOPIC_BATTERY_BOOST_DURATION                   MQTT_TOPIC_ROOT "battery_settings_boost_duration"
  #define MQTT_TOPIC_BATTERY_TEMPERATURE_COMPENSATION_COEFF   MQTT_TOPIC_ROOT "battery_settings_temperature_compensation_coeff"
  #define MQTT_TOPIC_BATTERY_MANAGEMENT_MODE                  MQTT_TOPIC_ROOT "battery_settings_management_mode"
  #define MQTT_TOPIC_STAT_ENERGY_CONSUMED_TOTAL               MQTT_TOPIC_ROOT "stats_consumption_total"
  // internal
  #define MQTT_TOPIC_INTERNAL_STATUS                          MQTT_TOPIC_ROOT "internal_status"
  //action
  #define MQTT_TOPIC_UPDATE_ALL_CONTROLLER_DATA               MQTT_TOPIC_ROOT "internal_update_all"
#endif
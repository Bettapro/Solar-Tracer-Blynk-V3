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

#define CONFIG_PERSISTENCE "/config.json"

//settings
#define CONFIG_WIFI_SSID "ssid"
#define CONFIG_WIFI_SSID_LEN 20

#define CONFIG_WIFI_PASSWORD "passw"
#define CONFIG_WIFI_PASSWORD_LEN 30

#define CONFIG_WM_AP_SSID "wmSsid"
#define CONFIG_WM_AP_SSID_LEN 15

#define CONFIG_WM_AP_PASSWORD "wmPassw"
#define CONFIG_WM_AP_PASSWORD_LEN 30

#define CONFIG_BLYNK_AUTH "blynkToken"
#define CONFIG_BLYNK_AUTH_LEN 32

#define CONFIG_BLYNK_HOSTNAME "blynkHostname"
#define CONFIG_BLYNK_HOSTNAME_LEN 20

#define CONFIG_BLYNK_PORT "blynkPort"

#define CONFIG_MQTT_HOSTNAME "mqttSrv"
#define CONFIG_MQTT_HOSTNAME_LEN 20

#define CONFIG_MQTT_PORT "mqttPort"

#define CONFIG_MQTT_CLIENT_ID "mqttClnt"
#define CONFIG_MQTT_CLIENT_ID_LEN 15

#define CONFIG_MQTT_USERNAME "mqttUsr"
#define CONFIG_MQTT_USERNAME_LEN 15

#define CONFIG_MQTT_PASSWORD "mqttPassw"
#define CONFIG_MQTT_PASSWORD_LEN 15

#define CONFIG_OTA_HOSTNAME "otaHostname"
#define CONFIG_OTA_HOSTNAME_LEN 15

#define CONFIG_MQTT_HA_DEVICE_NAME "haDeviceName"
#define CONFIG_MQTT_HA_DEVICE_NAME_LEN 15

#define CONFIG_OTA_PASSWORD "otaPassword"
#define CONFIG_OTA_PASSWORD_LEN 15


#define CONFIG_NTP_SERVER "ntpServer"
#define CONFIG_NTP_SERVER_LEN 22

#define CONFIG_NTP_TIMEZONE "ntpTimezone"
#define CONFIG_NTP_TIMEZONE_LEN 47

#define CONFIG_WIFI_IP_ADDRESS "ipAddr"
#define CONFIG_WIFI_IP_ADDRESS_LEN 15

#define CONFIG_WIFI_SUBNET "subnet"
#define CONFIG_WIFI_SUBNET_LEN 15

#define CONFIG_WIFI_GATEWAY "gateway"
#define CONFIG_WIFI_GATEWAY_LEN 15

#define CONFIG_WIFI_DNS1 "dns1"
#define CONFIG_WIFI_DNS1_LEN 15

#define CONFIG_WIFI_DNS2 "dns2"
#define CONFIG_WIFI_DNS2_LEN 15

#define CONFIG_EXTERNAL_HEAVY_LOAD_CURRENT_METER_VOLTAGE_ZERO_AMP_VOLT "hlZeroVOff"

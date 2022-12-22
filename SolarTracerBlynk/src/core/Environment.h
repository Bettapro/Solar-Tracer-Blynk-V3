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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../incl/include_all_core.h"
#include "../incl/include_all_lib.h"
#include "../core/debug.h"

struct environrmentData
{
    
    // wifi
    char wifiSSID[CONFIG_PERSISTENCE_WIFI_SSID_LEN + 1];
    char wifiPassword[CONFIG_PERSISTENCE_WIFI_PASSWORD_LEN + 1];
    char wifiIp[CONFIG_PERSISTENCE_WIFI_IP_ADDRESS_LEN + 1];
    char wifiGateway[CONFIG_PERSISTENCE_WIFI_GATEWAY_LEN + 1];
    char wifiSubnet[CONFIG_PERSISTENCE_WIFI_SUBNET_LEN + 1];
    char wifiDns1[CONFIG_PERSISTENCE_WIFI_DNS1_LEN + 1];
    char wifiDns2[CONFIG_PERSISTENCE_WIFI_DNS2_LEN + 1];
    // wifi manager
#ifdef USE_WIFI_AP_CONFIGURATION
    char wmApSSID[CONFIG_PERSISTENCE_WM_AP_SSID_LEN + 1];
    char wmApPassword[CONFIG_PERSISTENCE_WM_AP_PASSWORD_LEN + 1];
#endif
// blynk
#ifdef USE_BLYNK
#ifndef USE_BLYNK_2
    bool blynkLocalServer;
    char blynkServerHostname[CONFIG_PERSISTENCE_BLYNK_HOSTNAME_LEN + 1];
    uint16_t blynkServerPort;
#endif
    char blynkAuth[CONFIG_PERSISTENCE_BLYNK_AUTH_LEN + 1];
#endif
// mqtt
#ifdef USE_MQTT
    char mqttServerHostname[CONFIG_PERSISTENCE_MQTT_HOSTNAME_LEN + 1];
    uint16_t mqttServerPort;
    char mqttUsername[CONFIG_PERSISTENCE_MQTT_USERNAME_LEN + 1];
    char mqttPassword[CONFIG_PERSISTENCE_MQTT_PASSWORD_LEN + 1];
    char mqttClientId[CONFIG_PERSISTENCE_MQTT_CLIENT_ID_LEN + 1];
#endif
  #ifdef USE_MQTT_HOME_ASSISTANT
    char mqttHADeviceName[CONFIG_PERSISTENCE_MQTT_HA_DEVICE_NAME_LEN + 1];
  #endif
#ifdef USE_OTA_UPDATE
    char otaHostname[CONFIG_PERSISTENCE_OTA_HOSTNAME_LEN + 1];
    char otaPassword[CONFIG_PERSISTENCE_OTA_PASSWORD_LEN + 1];
#endif
#ifdef USE_NTP_SERVER
    char ntpServer[CONFIG_PERSISTENCE_NTP_SERVER_LEN + 1];
    char ntpTimezone[CONFIG_PERSISTENCE_NTP_TIMEZONE_LEN + 1];
#endif
#ifdef USE_EXTERNAL_HEAVY_LOAD_CURRENT_METER
    float hlZeroVOff;
#endif
};

class Environment
{
public:
    static void loadEnvData();

    static void resetEnvData();

    static environrmentData *getData();

private:
    static struct environrmentData envData;
#ifdef USE_WIFI_AP_CONFIGURATION
    static void loadStringToEnvIfExist(DynamicJsonDocument doc, const char *envKey, char *envValue);
#endif
};


#endif
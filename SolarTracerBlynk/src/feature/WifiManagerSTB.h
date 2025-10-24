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

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "../incl/include_all_core.h"

#ifdef USE_WIFI_AP_CONFIGURATION

#define ADD_PARAM_CHECKBOX(id, label, checked)                \
    this->wifiManager->addParameter(new WiFiManagerParameter( \
        id, label, "on", 15, checked ? "type=\"checkbox\" checked" : "type=\"checkbox\""))

#define ADD_PARAM(id, label, value, len) \
    this->wifiManager->addParameter(new WiFiManagerParameter(id, label, value, len))

#define ADD_PARAM_HTML(html) \
    this->wifiManager->addParameter(new WiFiManagerParameter(html))

#define ADD_PARAM_TYPED(id, label, value, len, type) \
    this->wifiManager->addParameter(new WiFiManagerParameter(id, label, value, len, type))

#include "../core/Environment.h"
#include "../incl/include_all_lib.h"

class WifiManagerSTB {
    public:
        void exit() {
            if (this->wifiManager == nullptr) {
                return;
            }

            this->wifiManager->stopConfigPortal();
            delete this->wifiManager;
            this->wifiManager = nullptr;
        }

        void saveAllParameters() {
            debugPrint("Saving parameters... ");
            if (!LittleFS.begin()) {
                debugPrintf(true, Text::errorWithCode, STATUS_ERR_LITTLEFS_BEGIN_FAILED);
                if (!LittleFS.format() || !LittleFS.begin()) {
                    debugPrintf(true, Text::errorWithCode, STATUS_ERR_LITTLEFS_FORMAT_FAILED);
                }
                return;
            }

            JsonDocument doc;
            doc[CONFIG_WIFI_SSID] = WiFi.SSID();
            doc[CONFIG_WIFI_PASSWORD] = WiFi.psk();

            WiFiManagerParameter** ppointer = this->wifiManager->getParameters();
            for (uint16_t paramCount = 0; paramCount < this->wifiManager->getParametersCount(); paramCount++) {
                WiFiManagerParameter* parameter = ppointer[paramCount];

                const char* paramId = parameter->getID();
                // skip parameters with no id (titles..)
                if (paramId == nullptr) {
                    continue;
                }

                if (
                    strcmp(paramId, CONFIG_SERIAL_DEBUG) == 0) {
                    // boolean
                    doc[paramId] = strcmp(parameter->getValue(), "on") == 0;
                } else if (
                    strcmp(paramId, CONFIG_BLYNK_PORT) == 0 ||
                    strcmp(paramId, CONFIG_MQTT_PORT) == 0) {
                    // integer
                    doc[paramId] = strlen(parameter->getValue()) > 0 ? atoi(parameter->getValue()) : 0;
                } else {
                    // default = string
                    doc[paramId] = parameter->getValue();
                }
            }

            File configFile = LittleFS.open(CONFIG_PERSISTENCE, "w");
            if (!configFile) {
                LittleFS.end();
            } else {
                serializeJson(doc, configFile);
                configFile.flush();
                configFile.close();
                LittleFS.end();
                debugPrintln(Text::ok);
            }
        }

        void start(bool tryConnection, bool blocking) {
            if (this->wifiManager != nullptr) {
                return;
            }

            // making sure to be connected to wifi from settings to get WiFi.SSID and WiFi.psk
            if (tryConnection) {
                WiFi.waitForConnectResult();
            }

            this->wifiManager = new WiFiManager();

            // "wifi", "wifinoscan", "info", "param", "custom", "close", "sep", "erase", "update", "restart", "exit"
            std::vector<const char*> menu = {
                "wifi",                        // COnfigure WiFi
                "param",                       // Setup
                "sep",                         // ---------
                "info",                        // Info
                "sep",                         // ---------
                "restart"                      // restart
            };

            wifiManager->setMenu(menu);

            this->wifiManager->setTitle("Solar-tracer-Blynk-V3");
            
            this->wifiManager->setSaveParamsCallback([this]() { this->saveAllParameters(); });
            this->wifiManager->setSaveConfigCallback([this]() { this->saveAllParameters(); });
            this->wifiManager->setConfigPortalBlocking(blocking);
            this->wifiManager->setBreakAfterConfig(true);
            this->wifiManager->setDisableConfigPortal(false);
            this->wifiManager->setShowInfoErase(false);
            this->wifiManager->setDebugOutput(false);

            ADD_PARAM_CHECKBOX(CONFIG_SERIAL_DEBUG, "Serial debug", Environment::getData()->serialDebug);

            ADD_PARAM_HTML("<p><b>WIFI:</b></p>");
            ADD_PARAM(CONFIG_WIFI_IP_ADDRESS, "IP", Environment::getData()->wifiIp, CONFIG_WIFI_IP_ADDRESS_LEN);
            ADD_PARAM(CONFIG_WIFI_GATEWAY, "Gateway", Environment::getData()->wifiGateway, CONFIG_WIFI_GATEWAY_LEN);
            ADD_PARAM(CONFIG_WIFI_SUBNET, "Subnet", Environment::getData()->wifiSubnet, CONFIG_WIFI_SUBNET_LEN);
            ADD_PARAM(CONFIG_WIFI_DNS1, "Dns1", Environment::getData()->wifiDns1, CONFIG_WIFI_DNS1_LEN);
            ADD_PARAM(CONFIG_WIFI_DNS2, "Dns2", Environment::getData()->wifiDns2, CONFIG_WIFI_DNS2_LEN);

#ifdef USE_BLYNK
            ADD_PARAM_HTML("<p><b>BLYNK:</b></p>");
            ADD_PARAM(CONFIG_BLYNK_AUTH, "API key", Environment::getData()->blynkAuth, CONFIG_BLYNK_AUTH_LEN);
#ifndef USE_BLYNK_2
            ADD_PARAM(CONFIG_BLYNK_HOSTNAME, Text::server, Environment::getData()->blynkServerHostname, CONFIG_BLYNK_HOSTNAME_LEN);
            ADD_PARAM_TYPED(CONFIG_BLYNK_PORT, Text::port, Util::intToChar(Environment::getData()->blynkServerPort), 5, "type=\"number\" min=\"0\"");
#endif
#endif

#ifdef USE_MQTT
            ADD_PARAM_HTML("<p><b>MQTT:</b></p>");
            ADD_PARAM(CONFIG_MQTT_HOSTNAME, Text::server, Environment::getData()->mqttServerHostname, CONFIG_MQTT_HOSTNAME_LEN);
            ADD_PARAM_TYPED(CONFIG_MQTT_PORT, Text::port, Util::intToChar(Environment::getData()->mqttServerPort), 5, "type=\"number\" min=\"0\"");
            ADD_PARAM(CONFIG_MQTT_CLIENT_ID, "Client ID", Environment::getData()->mqttClientId, CONFIG_MQTT_CLIENT_ID_LEN);
            ADD_PARAM(CONFIG_MQTT_USERNAME, "Username", Environment::getData()->mqttUsername, CONFIG_MQTT_USERNAME_LEN);
            ADD_PARAM(CONFIG_MQTT_PASSWORD, Text::password, Environment::getData()->mqttPassword, CONFIG_MQTT_PASSWORD_LEN);
#endif

#ifdef USE_MQTT_HOME_ASSISTANT
            ADD_PARAM_HTML("<p><b>MQTT-HA:</b></p>");
            ADD_PARAM(CONFIG_MQTT_HA_DEVICE_ID, "Device ID", Environment::getData()->mqttHADeviceId, CONFIG_MQTT_HA_DEVICE_ID_LEN);
            ADD_PARAM(CONFIG_MQTT_HA_DEVICE_NAME, "Device Name", Environment::getData()->mqttHADeviceName, CONFIG_MQTT_HA_DEVICE_NAME_LEN);
#endif

#ifdef USE_OTA_UPDATE
            ADD_PARAM_HTML("<p><b>OTA:</b></p>");
            ADD_PARAM(CONFIG_OTA_HOSTNAME, "Hostname", Environment::getData()->otaHostname, CONFIG_OTA_HOSTNAME_LEN);
            ADD_PARAM(CONFIG_OTA_PASSWORD, Text::password, Environment::getData()->otaPassword, CONFIG_OTA_PASSWORD_LEN);
#endif

#ifdef USE_NTP_SERVER
            ADD_PARAM_HTML("<p><b>NTP:</b></p>");
            ADD_PARAM(CONFIG_NTP_SERVER, Text::server, Environment::getData()->ntpServer, CONFIG_NTP_SERVER_LEN);
            ADD_PARAM(CONFIG_NTP_TIMEZONE, "Timezone", Environment::getData()->ntpTimezone, CONFIG_NTP_TIMEZONE_LEN);
#endif

#ifdef USE_EXTERNAL_HEAVY_LOAD_CURRENT_METER
            ADD_PARAM_HTML("<p><b>EXT. LOAD METER:</b></p>");
            ADD_PARAM_TYPED(CONFIG_EXTERNAL_HEAVY_LOAD_CURRENT_METER_VOLTAGE_ZERO_AMP_VOLT, "Volt off.", Util::floatToChar(Environment::getData()->heavyLoadCurrentZeroV), 8, "type=\"number\" step=\"0.001\"");
#endif

            ADD_PARAM_HTML("<p><b>ACCESS POINT:</b></p>");
            ADD_PARAM(CONFIG_WM_AP_SSID, "SSID", Environment::getData()->wmApSSID, CONFIG_WM_AP_SSID_LEN);
            ADD_PARAM(CONFIG_WM_AP_PASSWORD, Text::password, Environment::getData()->wmApPassword, CONFIG_WM_AP_PASSWORD_LEN);

            if (blocking) {
                this->wifiManager->setConfigPortalTimeout(WIFI_AP_TIMEOUT);
            }
            this->wifiManager->startConfigPortal(Environment::getData()->wmApSSID, Environment::getData()->wmApPassword);

            if (blocking) {
                this->exit();
                ESP.restart();
            }
        }

        void loop() {
            if (this->wifiManager != nullptr) {
                this->wifiManager->process();
            }
        }

        static WifiManagerSTB& getInstance() {
            static WifiManagerSTB instance;
            return instance;
        }

    private:
        WiFiManager* wifiManager;
};

#endif
#endif
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

#include "../incl/include_all_lib.h"
#include "../core/Environment.h"

class WifiManagerSTB
{
public:
      static bool formatLittleFS()
      {
            LittleFS.end();
            LittleFS.begin();
            LittleFS.format();
            return LittleFS.begin();
      }

      static void startWifiConfigurationAP(bool tryConnection)
      {
            bool shouldSaveConfig = false;

            // making sure to be connected to wifi from settings to get WiFi.SSID and WiFi.psk
            if (tryConnection)
            {
                  WiFi.waitForConnectResult();
            }

            WiFiManager wifiManager;
            wifiManager.setTitle("Solar-tracer-Blynk-V3");
            wifiManager.setBreakAfterConfig(true);
            wifiManager.setSaveParamsCallback([&shouldSaveConfig, &wifiManager]()
                                              {
                                      shouldSaveConfig = true;
                                      wifiManager.stopConfigPortal(); });
            wifiManager.setSaveConfigCallback([&shouldSaveConfig]()
                                              { shouldSaveConfig = true; });
            wifiManager.setParamsPage(true);
            wifiManager.setConfigPortalBlocking(true);
            wifiManager.setDebugOutput(false);

#ifdef USE_BLYNK
            WiFiManagerParameter custom_blynkLocalServerText("<p><b>BLYNK:</b></p>");
            wifiManager.addParameter(&custom_blynkLocalServerText);

            WiFiManagerParameter custom_BlynkAuth(CONFIG_PERSISTENCE_BLYNK_AUTH, "API key", Environment::getData()->blynkAuth, CONFIG_PERSISTENCE_BLYNK_AUTH_LEN);
            wifiManager.addParameter(&custom_BlynkAuth);

#ifndef USE_BLYNK_2

            WiFiManagerParameter customBlynkServerHostname(CONFIG_PERSISTENCE_BLYNK_HOSTNAME, "Hostname", Environment::getData()->blynkServerHostname, CONFIG_PERSISTENCE_BLYNK_HOSTNAME_LEN);
            WiFiManagerParameter custom_BlynkServerPort(CONFIG_PERSISTENCE_BLYNK_PORT, "Port", String(Environment::getData()->blynkServerPort).c_str(), 5, "type=\"number\" min=\"0\"");

            wifiManager.addParameter(&customBlynkServerHostname);
            wifiManager.addParameter(&custom_BlynkServerPort);
#endif
#endif

#ifdef USE_MQTT
            WiFiManagerParameter custom_mqttText("<p><b>MQTT:</b></p>");
            wifiManager.addParameter(&custom_mqttText);

            WiFiManagerParameter custom_mqttServer(CONFIG_PERSISTENCE_MQTT_HOSTNAME, "Server", Environment::getData()->mqttServerHostname, CONFIG_PERSISTENCE_MQTT_HOSTNAME_LEN);
            wifiManager.addParameter(&custom_mqttServer);
            char portString[5];
            WiFiManagerParameter custom_mqttServerPort(CONFIG_PERSISTENCE_MQTT_PORT, "Port", Util::intToChar(Environment::getData()->mqttServerPort, portString), 5, "type=\"number\" min=\"0\"");
            wifiManager.addParameter(&custom_mqttServerPort);
            WiFiManagerParameter custom_mqttUsername(CONFIG_PERSISTENCE_MQTT_USERNAME, "Username", Environment::getData()->mqttUsername, CONFIG_PERSISTENCE_MQTT_USERNAME_LEN);
            wifiManager.addParameter(&custom_mqttUsername);
            WiFiManagerParameter custom_mqttPassword(CONFIG_PERSISTENCE_MQTT_PASSWORD, Text::password, Environment::getData()->mqttPassword, CONFIG_PERSISTENCE_MQTT_PASSWORD_LEN);
            wifiManager.addParameter(&custom_mqttPassword);
#endif
#ifdef USE_MQTT_HOME_ASSISTANT
            WiFiManagerParameter custom_mqttHAText("<p><b>MQTT-HA:</b></p>");
            wifiManager.addParameter(&custom_mqttHAText);

            WiFiManagerParameter custom_mqttHADeviceName(CONFIG_PERSISTENCE_MQTT_HA_DEVICE_NAME, "Device Name", Environment::getData()->mqttHADeviceName, CONFIG_PERSISTENCE_MQTT_HA_DEVICE_NAME_LEN);
            wifiManager.addParameter(&custom_mqttHADeviceName);
            WiFiManagerParameter custom_mqttHADeviceId(CONFIG_PERSISTENCE_MQTT_HA_DEVICE_ID, "Id", Environment::getData()->mqttHADeviceId, CONFIG_PERSISTENCE_MQTT_HA_DEVICE_ID_LEN);
            wifiManager.addParameter(&custom_mqttHADeviceId);
#endif

#ifdef USE_OTA_UPDATE
            WiFiManagerParameter customOtaUpdateText("<p><b>OTA:</b></p>");

            WiFiManagerParameter customOtaHostname(CONFIG_PERSISTENCE_OTA_HOSTNAME, "Hostname", Environment::getData()->otaHostname, CONFIG_PERSISTENCE_OTA_HOSTNAME_LEN);
            WiFiManagerParameter customOtaPassword(CONFIG_PERSISTENCE_OTA_PASSWORD, Text::password, Environment::getData()->otaPassword, CONFIG_PERSISTENCE_OTA_PASSWORD_LEN);

            wifiManager.addParameter(&customOtaUpdateText);
            wifiManager.addParameter(&customOtaHostname);
            wifiManager.addParameter(&customOtaPassword);
#endif

#ifdef USE_NTP_SERVER
            WiFiManagerParameter customNtpText("<p><b>NTP:</b></p>");

            WiFiManagerParameter customNtpServer(CONFIG_PERSISTENCE_NTP_SERVER, "Server", Environment::getData()->ntpServer, CONFIG_PERSISTENCE_NTP_SERVER_LEN);
            WiFiManagerParameter customNtpTimezone(CONFIG_PERSISTENCE_NTP_TIMEZONE, "Timezone", Environment::getData()->ntpTimezone, CONFIG_PERSISTENCE_NTP_TIMEZONE_LEN);

            wifiManager.addParameter(&customNtpText);
            wifiManager.addParameter(&customNtpServer);
            wifiManager.addParameter(&customNtpTimezone);
#endif

            WiFiManagerParameter custom_wmText("<p><b>ACCESS POINT:</b></p>");
            WiFiManagerParameter custom_wmSSID(CONFIG_PERSISTENCE_WM_AP_SSID, "SSID", Environment::getData()->wmApSSID, CONFIG_PERSISTENCE_WM_AP_SSID_LEN);
            WiFiManagerParameter custom_wmPassword(CONFIG_PERSISTENCE_WM_AP_PASSWORD, Text::password, Environment::getData()->wmApPassword, CONFIG_PERSISTENCE_WM_AP_PASSWORD_LEN);

            wifiManager.addParameter(&custom_wmText);
            wifiManager.addParameter(&custom_wmSSID);
            wifiManager.addParameter(&custom_wmPassword);

            wifiManager.startConfigPortal(Environment::getData()->wmApSSID, Environment::getData()->wmApPassword);

            if (shouldSaveConfig)
            {
                  debugPrintln("Saving wifimanager parameters... ");

                  strcpy(Environment::getData()->wmApSSID, custom_wmSSID.getValue());
                  strcpy(Environment::getData()->wmApPassword, custom_wmPassword.getValue());

#ifdef USE_BLYNK
#ifndef USE_BLYNK_2
                  Environment::getData()->blynkLocalServer = strlen(customBlynkServerHostname.getValue()) > 0;
                  strcpy(Environment::getData()->blynkServerHostname, !Environment::getData()->blynkLocalServer ? "" : customBlynkServerHostname.getValue());
                  Environment::getData()->blynkServerPort = !Environment::getData()->blynkLocalServer ? 0 : String(custom_BlynkServerPort.getValue()).toInt();
#endif
                  strcpy(Environment::getData()->blynkAuth, custom_BlynkAuth.getValue());
#endif

#ifdef USE_MQTT
                  strcpy(Environment::getData()->mqttServerHostname, custom_mqttServer.getValue());
                  Environment::getData()->mqttServerPort = atoi(custom_mqttServerPort.getValue());
                  strcpy(Environment::getData()->mqttUsername, custom_mqttUsername.getValue());
                  strcpy(Environment::getData()->mqttPassword, custom_mqttPassword.getValue());
#endif
#ifdef USE_MQTT_HOME_ASSISTANT
                  strcpy(Environment::getData()->mqttHADeviceName, custom_mqttHADeviceId.getValue());
                  strcpy(Environment::getData()->mqttHADeviceId, custom_mqttHADeviceName.getValue());
#endif

#ifdef USE_OTA_UPDATE
                  strcpy(Environment::getData()->otaHostname, customOtaHostname.getValue());
                  strcpy(Environment::getData()->otaPassword, customOtaPassword.getValue());
#endif

#ifdef USE_NTP_SERVER
                  strcpy(Environment::getData()->ntpServer, customNtpServer.getValue());
                  strcpy(Environment::getData()->ntpTimezone, customNtpTimezone.getValue());
#endif

                  // if LittleFS is not usable
                  if (!LittleFS.begin())
                  {
                        debugPrintf(true, Text::errorWithCode, 1);
                        if (!formatLittleFS())
                        {
                              debugPrintf(true, Text::errorWithCode, 2);
                        }
                        return;
                  }

                  DynamicJsonDocument doc(2048);

                  doc[CONFIG_PERSISTENCE_WIFI_SSID] = WiFi.SSID();
                  doc[CONFIG_PERSISTENCE_WIFI_PASSWORD] = WiFi.psk();

                  doc[CONFIG_PERSISTENCE_WM_AP_SSID] = Environment::getData()->wmApSSID;
                  doc[CONFIG_PERSISTENCE_WM_AP_PASSWORD] = Environment::getData()->wmApPassword;
#ifdef USE_BLYNK
                  doc[CONFIG_PERSISTENCE_BLYNK_AUTH] = Environment::getData()->blynkAuth;
#ifndef USE_BLYNK_2
                  doc[CONFIG_PERSISTENCE_BLYNK_HOSTNAME] = Environment::getData()->blynkServerHostname;
                  doc[CONFIG_PERSISTENCE_BLYNK_PORT] = Environment::getData()->blynkServerPort;
#endif
#endif

#ifdef USE_OTA_UPDATE
                  doc[CONFIG_PERSISTENCE_OTA_HOSTNAME] = Environment::getData()->otaHostname;
                  doc[CONFIG_PERSISTENCE_OTA_PASSWORD] = Environment::getData()->otaPassword;
#endif

#ifdef USE_NTP_SERVER
                  doc[CONFIG_PERSISTENCE_NTP_SERVER] = Environment::getData()->ntpServer;
                  doc[CONFIG_PERSISTENCE_NTP_TIMEZONE] = Environment::getData()->ntpTimezone;
#endif

                  File configFile = LittleFS.open(CONFIG_PERSISTENCE, "w");
                  if (!configFile)
                  {
                        LittleFS.end();
                  }
                  else
                  {
                        serializeJson(doc, configFile);
                        configFile.flush();
                        configFile.close();
                        LittleFS.end();
                        debugPrintln(Text::ok);
                  }
            }
      }
};

#endif
#endif
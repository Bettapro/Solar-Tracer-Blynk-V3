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


            WiFiManagerParameter customWIFIText("<p><b>WIFI:</b></p>");
            wifiManager.addParameter(&customWIFIText);

            WiFiManagerParameter customWIFIIpAddress(CONFIG_PERSISTENCE_WIFI_IP_ADDRESS, "IP", Environment::getData()->wifiIp, CONFIG_PERSISTENCE_WIFI_IP_ADDRESS_LEN);
            wifiManager.addParameter(&customWIFIIpAddress);
            WiFiManagerParameter customWIFIGateway(CONFIG_PERSISTENCE_WIFI_GATEWAY, "Gateway", Environment::getData()->wifiGateway, CONFIG_PERSISTENCE_WIFI_GATEWAY_LEN);
            wifiManager.addParameter(&customWIFIGateway);
            WiFiManagerParameter customWIFISubnet(CONFIG_PERSISTENCE_WIFI_SUBNET, "Subnet", Environment::getData()->wifiSubnet, CONFIG_PERSISTENCE_WIFI_SUBNET_LEN);
            wifiManager.addParameter(&customWIFISubnet);
            WiFiManagerParameter customWIFIDns1(CONFIG_PERSISTENCE_WIFI_DNS1, "Dns1", Environment::getData()->wifiDns1, CONFIG_PERSISTENCE_WIFI_DNS1_LEN);
            wifiManager.addParameter(&customWIFIDns1);
            WiFiManagerParameter customWIFIDns2(CONFIG_PERSISTENCE_WIFI_DNS2, "Dns2", Environment::getData()->wifiDns2, CONFIG_PERSISTENCE_WIFI_DNS2_LEN);
            wifiManager.addParameter(&customWIFIDns2);

#ifdef USE_BLYNK
            WiFiManagerParameter customBlynkLocalServerText("<p><b>BLYNK:</b></p>");
            wifiManager.addParameter(&customBlynkLocalServerText);

            WiFiManagerParameter customBlynkAuth(CONFIG_PERSISTENCE_BLYNK_AUTH, "API key", Environment::getData()->blynkAuth, CONFIG_PERSISTENCE_BLYNK_AUTH_LEN);
            wifiManager.addParameter(&customBlynkAuth);

#ifndef USE_BLYNK_2

            WiFiManagerParameter customBlynkServerHostname(CONFIG_PERSISTENCE_BLYNK_HOSTNAME, Text::server, Environment::getData()->blynkServerHostname, CONFIG_PERSISTENCE_BLYNK_HOSTNAME_LEN);
            WiFiManagerParameter customBlynkServerPort(CONFIG_PERSISTENCE_BLYNK_PORT, Text::port, String(Environment::getData()->blynkServerPort).c_str(), 5, "type=\"number\" min=\"0\"");

            wifiManager.addParameter(&customBlynkServerHostname);
            wifiManager.addParameter(&customBlynkServerPort);
#endif
#endif



#ifdef USE_MQTT
            WiFiManagerParameter customMqttText("<p><b>MQTT:</b></p>");
            wifiManager.addParameter(&customMqttText);

            WiFiManagerParameter customMqttServer(CONFIG_PERSISTENCE_MQTT_HOSTNAME, Text::server, Environment::getData()->mqttServerHostname, CONFIG_PERSISTENCE_MQTT_HOSTNAME_LEN);
            wifiManager.addParameter(&customMqttServer);
            char portString[5];
            WiFiManagerParameter customMqttServerPort(CONFIG_PERSISTENCE_MQTT_PORT, Text::port, Util::intToChar(Environment::getData()->mqttServerPort, portString), 5, "type=\"number\" min=\"0\"");
            wifiManager.addParameter(&customMqttServerPort);
            WiFiManagerParameter customMqttClientId(CONFIG_PERSISTENCE_MQTT_CLIENT_ID, "Client ID", Environment::getData()->mqttClientId, CONFIG_PERSISTENCE_MQTT_CLIENT_ID_LEN);
            wifiManager.addParameter(&customMqttClientId);
            WiFiManagerParameter customMqttUsername(CONFIG_PERSISTENCE_MQTT_USERNAME, "Username", Environment::getData()->mqttUsername, CONFIG_PERSISTENCE_MQTT_USERNAME_LEN);
            wifiManager.addParameter(&customMqttUsername);
            WiFiManagerParameter customMqttPassword(CONFIG_PERSISTENCE_MQTT_PASSWORD, Text::password, Environment::getData()->mqttPassword, CONFIG_PERSISTENCE_MQTT_PASSWORD_LEN);
            wifiManager.addParameter(&customMqttPassword);
#endif
#ifdef USE_MQTT_HOME_ASSISTANT
            WiFiManagerParameter customMqttHAText("<p><b>MQTT-HA:</b></p>");
            wifiManager.addParameter(&customMqttHAText);

            WiFiManagerParameter customMqttHADeviceName(CONFIG_PERSISTENCE_MQTT_HA_DEVICE_NAME, "Device Name", Environment::getData()->mqttHADeviceName, CONFIG_PERSISTENCE_MQTT_HA_DEVICE_NAME_LEN);
            wifiManager.addParameter(&customMqttHADeviceName);
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

            WiFiManagerParameter customNtpServer(CONFIG_PERSISTENCE_NTP_SERVER, Text::server, Environment::getData()->ntpServer, CONFIG_PERSISTENCE_NTP_SERVER_LEN);
            WiFiManagerParameter customNtpTimezone(CONFIG_PERSISTENCE_NTP_TIMEZONE, "Timezone", Environment::getData()->ntpTimezone, CONFIG_PERSISTENCE_NTP_TIMEZONE_LEN);

            wifiManager.addParameter(&customNtpText);
            wifiManager.addParameter(&customNtpServer);
            wifiManager.addParameter(&customNtpTimezone);
#endif

            WiFiManagerParameter customWmText("<p><b>ACCESS POINT:</b></p>");
            WiFiManagerParameter customWmSSID(CONFIG_PERSISTENCE_WM_AP_SSID, "SSID", Environment::getData()->wmApSSID, CONFIG_PERSISTENCE_WM_AP_SSID_LEN);
            WiFiManagerParameter customWmPassword(CONFIG_PERSISTENCE_WM_AP_PASSWORD, Text::password, Environment::getData()->wmApPassword, CONFIG_PERSISTENCE_WM_AP_PASSWORD_LEN);

            wifiManager.addParameter(&customWmText);
            wifiManager.addParameter(&customWmSSID);
            wifiManager.addParameter(&customWmPassword);

            wifiManager.startConfigPortal(Environment::getData()->wmApSSID, Environment::getData()->wmApPassword);

            if (shouldSaveConfig)
            {
                  debugPrintln("Saving wifimanager parameters... ");

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

                  doc[CONFIG_PERSISTENCE_WIFI_IP_ADDRESS] = customWIFIIpAddress.getValue();
                  doc[CONFIG_PERSISTENCE_WIFI_GATEWAY] = customWIFIGateway.getValue();
                  doc[CONFIG_PERSISTENCE_WIFI_SUBNET] = customWIFISubnet.getValue();
                  doc[CONFIG_PERSISTENCE_WIFI_DNS1] = customWIFIDns1.getValue();
                  doc[CONFIG_PERSISTENCE_WIFI_DNS2] = customWIFIDns2.getValue();

                  doc[CONFIG_PERSISTENCE_WM_AP_SSID] = customWmSSID.getValue();
                  doc[CONFIG_PERSISTENCE_WM_AP_PASSWORD] = customWmPassword.getValue();
#ifdef USE_BLYNK
                  doc[CONFIG_PERSISTENCE_BLYNK_AUTH] = customBlynkAuth.getValue();
#ifndef USE_BLYNK_2
                  doc[CONFIG_PERSISTENCE_BLYNK_HOSTNAME] = customBlynkServerHostname.getValue();
                  doc[CONFIG_PERSISTENCE_BLYNK_PORT] = strlen(customBlynkServerPort.getValue()) > 0 ? atoi(customBlynkServerPort.getValue()) : 0;
#endif
#endif
#ifdef USE_MQTT
                  doc[CONFIG_PERSISTENCE_MQTT_HOSTNAME] =  customMqttServer.getValue();
                  doc[CONFIG_PERSISTENCE_MQTT_PORT] = strlen(customMqttServerPort.getValue()) > 0 ? atoi(customMqttServerPort.getValue()) : 0;
                  doc[CONFIG_PERSISTENCE_MQTT_USERNAME] = customMqttUsername.getValue();
                  doc[CONFIG_PERSISTENCE_MQTT_PASSWORD] =  customMqttPassword.getValue();
                  doc[CONFIG_PERSISTENCE_MQTT_CLIENT_ID] = customMqttClientId.getValue();
#endif
#ifdef USE_MQTT_HOME_ASSISTANT
                  doc[CONFIG_PERSISTENCE_MQTT_HA_DEVICE_NAME] = customMqttHADeviceName.getValue();
#endif

#ifdef USE_OTA_UPDATE
                  doc[CONFIG_PERSISTENCE_OTA_HOSTNAME] = customOtaHostname.getValue();
                  doc[CONFIG_PERSISTENCE_OTA_PASSWORD] = customOtaPassword.getValue();
#endif

#ifdef USE_NTP_SERVER
                  doc[CONFIG_PERSISTENCE_NTP_SERVER] = customNtpServer.getValue();
                  doc[CONFIG_PERSISTENCE_NTP_TIMEZONE] = customNtpTimezone.getValue();
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
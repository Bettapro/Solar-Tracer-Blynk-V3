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
#include "../incl/project_config.h"

bool formatLittleFS()
{
  LittleFS.end();
  LittleFS.begin();
  LittleFS.format();
  return LittleFS.begin();
}

void startWifiConfigurationAP(bool tryConnection)
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
                                      wifiManager.stopConfigPortal();
                                    });
  wifiManager.setSaveConfigCallback([&shouldSaveConfig]()
                                    { shouldSaveConfig = true; });
  wifiManager.setParamsPage(true);
  wifiManager.setConfigPortalBlocking(true);
  wifiManager.setDebugOutput(false);

  WiFiManagerParameter custom_wmSSID("wmApSSID", "Access Point SSID", envData.wmApSSID, CONFIG_PERSISTENCE_WM_AP_SSID_LEN);
  WiFiManagerParameter custom_wmPassword("wmApPassword", "Access Point Password", envData.wmApPassword, CONFIG_PERSISTENCE_WM_AP_PASSWORD_LEN);

#ifdef USE_BLYNK
  WiFiManagerParameter custom_BlynkAuth("blynkAuth", "Blynk API key", envData.blynkAuth, CONFIG_PERSISTENCE_BLYNK_AUTH_LEN);

  wifiManager.addParameter(&custom_BlynkAuth);

#ifndef USE_BLYNK_2
  WiFiManagerParameter custom_blynkLocalServerText("<p><b>BLYNK LOCAL SERVER ONLY:</b></p>");

  WiFiManagerParameter customBlynkServerHostname("blynkServerHostname", "Blynk Server Hostname", envData.blynkServerHostname, CONFIG_PERSISTENCE_BLYNK_HOSTNAME_LEN);
  WiFiManagerParameter custom_BlynkServerPort("blynkServerPort", "Blynk Server Port", String(envData.blynkServerPort).c_str(), 5, "type=\"number\" min=\"0\"");

  wifiManager.addParameter(&custom_blynkLocalServerText);
  wifiManager.addParameter(&customBlynkServerHostname);
  wifiManager.addParameter(&custom_BlynkServerPort);
#endif
#endif

#ifdef USE_OTA_UPDATE
  WiFiManagerParameter customOtaUpdateText("<p><b>OTA UPDATE:</b></p>");

  WiFiManagerParameter customOtaHostname("otaHostname", "OTA Hostname", envData.otaHostname, CONFIG_PERSISTENCE_OTA_HOSTNAME_LEN);
  WiFiManagerParameter customOtaPassword("otaPassword", "OTA Password", envData.otaPassword, CONFIG_PERSISTENCE_OTA_PASSWORD_LEN);

  wifiManager.addParameter(&customOtaUpdateText);
  wifiManager.addParameter(&customOtaHostname);
  wifiManager.addParameter(&customOtaPassword);
#endif

#ifdef USE_NTP_SERVER
  WiFiManagerParameter customNtpText("<p><b>NTP settings:</b></p>");

  WiFiManagerParameter customNtpServer("ntpServer", "NTP Server", envData.ntpServer, CONFIG_PERSISTENCE_NTP_SERVER_LEN);
  WiFiManagerParameter customNtpTimezone("ntpTimezone", "Timezone", envData.ntpTimezone, CONFIG_PERSISTENCE_NTP_TIMEZONE_LEN);

  wifiManager.addParameter(&customNtpText);
  wifiManager.addParameter(&customNtpServer);
  wifiManager.addParameter(&customNtpTimezone);
#endif

  WiFiManagerParameter custom_wmText("<p><b>CONFIGURATION OVER WIFI:</b></p>");

  wifiManager.addParameter(&custom_wmText);
  wifiManager.addParameter(&custom_wmSSID);
  wifiManager.addParameter(&custom_wmPassword);

  wifiManager.startConfigPortal(envData.wmApSSID, envData.wmApPassword);

  if (shouldSaveConfig)
  {
    debugPrintln("Saving wifimanager parameters...");

    strcpy(envData.wmApSSID, custom_wmSSID.getValue());
    strcpy(envData.wmApPassword, custom_wmPassword.getValue());

#ifdef USE_BLYNK
#ifndef USE_BLYNK_2
    envData.blynkLocalServer = strlen(customBlynkServerHostname.getValue()) > 0;
    strcpy(envData.blynkServerHostname, !envData.blynkLocalServer ? "" : customBlynkServerHostname.getValue());
    envData.blynkServerPort = !envData.blynkLocalServer ? 0 : String(custom_BlynkServerPort.getValue()).toInt();
#endif
    strcpy(envData.blynkAuth, custom_BlynkAuth.getValue());
#endif

#ifdef USE_OTA_UPDATE
    strcpy(envData.otaHostname, customOtaHostname.getValue());
    strcpy(envData.otaPassword, customOtaPassword.getValue());
#endif

#ifdef USE_NTP_SERVER
    strcpy(envData.ntpServer, customNtpServer.getValue());
    strcpy(envData.ntpTimezone, customNtpTimezone.getValue());
#endif

    // if LittleFS is not usable
    if (!LittleFS.begin())
    {
      debugPrintln("--> Failed to mount file system");
      if (!formatLittleFS())
      {
        debugPrintln("--> Failed to format file system - hardware issues!");
      }
    }

    DynamicJsonDocument doc(2048);

    doc[CONFIG_PERSISTENCE_WIFI_SSID] = WiFi.SSID();
    doc[CONFIG_PERSISTENCE_WIFI_PASSWORD] = WiFi.psk();

    doc[CONFIG_PERSISTENCE_WM_AP_SSID] = envData.wmApSSID;
    doc[CONFIG_PERSISTENCE_WM_AP_PASSWORD] = envData.wmApPassword;
#ifdef USE_BLYNK
    doc[CONFIG_PERSISTENCE_BLYNK_AUTH] = envData.blynkAuth;
#ifndef USE_BLYNK_2
    doc[CONFIG_PERSISTENCE_BLYNK_IS_LOCAL] = envData.blynkLocalServer;
    doc[CONFIG_PERSISTENCE_BLYNK_HOSTNAME] = envData.blynkServerHostname;
    doc[CONFIG_PERSISTENCE_BLYNK_PORT] = envData.blynkServerPort;
#endif
#endif

#ifdef USE_OTA_UPDATE
    doc[CONFIG_PERSISTENCE_OTA_HOSTNAME] = envData.otaHostname;
    doc[CONFIG_PERSISTENCE_OTA_PASSWORD] = envData.otaPassword;
#endif

#ifdef USE_NTP_SERVER
    doc[CONFIG_PERSISTENCE_NTP_SERVER] = envData.ntpServer;
    doc[CONFIG_PERSISTENCE_NTP_TIMEZONE] = envData.ntpTimezone;
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
      debugPrintln("-> SAVED");
    }
  }
}

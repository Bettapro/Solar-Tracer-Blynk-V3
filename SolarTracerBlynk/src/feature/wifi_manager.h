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

bool shouldSaveConfig = false;
void saveConfigCallback()
{
  shouldSaveConfig = true;
}

bool formatLittleFS()
{
  LittleFS.end();
  LittleFS.begin();
  LittleFS.format();
  return LittleFS.begin();
}

void startWifiConfigurationAP(bool tryConnection)
{
  // making sure to be connected to wifi from settings to get WiFi.SSID and WiFi.psk
  if (tryConnection)
  {
    WiFi.waitForConnectResult();
  }

  WiFiManager wifiManager;
  wifiManager.setTitle("Solar-tracer-Blynk-V3");
  wifiManager.setBreakAfterConfig(true);
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  WiFiManagerParameter customBlynkServerHostname("blynkServerHostname", "Blynk Server Hostname", envData.blynkServerHostname, CONFIG_PERSISTENCE_WIFI_BLYNK_HOSTNAME_LEN);
  WiFiManagerParameter custom_BlynkServerPort("blynkServerPort", "Blynk Server Port", String(envData.blynkServerPort).c_str(), 5, "type=\"number\" min=\"0\"");
  WiFiManagerParameter custom_BlynkAuth("blynkAuth", "Blynk API key", envData.blynkAuth, CONFIG_PERSISTENCE_WIFI_BLYNK_AUTH_LEN);
  WiFiManagerParameter custom_wmSSID("wmApSSID", "Access Point SSID", envData.wmApSSID, CONFIG_PERSISTENCE_WM_AP_SSID_LEN);
  WiFiManagerParameter custom_wmPassword("wmApPassword", "Access Point Password", envData.wmApPassword, CONFIG_PERSISTENCE_WM_AP_PASSWORD_LEN);

  WiFiManagerParameter custom_blynkLocalServerText("<p><b>BLYNK LOCAL SERVER ONLY:</b></p>");
  WiFiManagerParameter custom_wmText("<p><b>CONFIGURATION OVER WIFI:</b></p>");

  wifiManager.addParameter(&custom_BlynkAuth);
  wifiManager.addParameter(&custom_blynkLocalServerText);
  wifiManager.addParameter(&customBlynkServerHostname);
  wifiManager.addParameter(&custom_BlynkServerPort);
  wifiManager.addParameter(&custom_wmText);
  wifiManager.addParameter(&custom_wmSSID);
  wifiManager.addParameter(&custom_wmPassword);

  wifiManager.startConfigPortal(envData.wmApSSID, envData.wmApPassword);
  wifiManager.setConfigPortalBlocking(true);

  if (shouldSaveConfig)
  {
    debugPrintln("Saving wifimanager parameters...");

    envData.blynkLocalServer = strlen(customBlynkServerHostname.getValue()) > 0;
    strcpy(envData.blynkServerHostname, !envData.blynkLocalServer ? "" : customBlynkServerHostname.getValue());
    envData.blynkServerPort = !envData.blynkLocalServer ? 0 : String(custom_BlynkServerPort.getValue()).toInt();

    strcpy(envData.blynkAuth, custom_BlynkAuth.getValue());

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
    doc[CONFIG_PERSISTENCE_WIFI_BLYNK_AUTH] = envData.blynkAuth;
    doc[CONFIG_PERSISTENCE_WIFI_BLYNK_IS_LOCAL] = envData.blynkLocalServer;
    doc[CONFIG_PERSISTENCE_WIFI_BLYNK_HOSTNAME] = envData.blynkServerHostname;
    doc[CONFIG_PERSISTENCE_WIFI_BLYNK_PORT] = envData.blynkServerPort;
    doc[CONFIG_PERSISTENCE_WM_AP_SSID] = envData.wmApSSID;
    doc[CONFIG_PERSISTENCE_WM_AP_PASSWORD] = envData.wmApPassword;

    File configFile = LittleFS.open(CONFIG_PERSISTENCE, "w");
    if (!configFile)
    {
      LittleFS.end();
    }
    else
    {
      serializeJson(doc, configFile);
      //serializeJson(doc, Serial);
      configFile.flush();
      configFile.close();
      LittleFS.end();
      debugPrintln("-> SAVED");
    }
  }
}

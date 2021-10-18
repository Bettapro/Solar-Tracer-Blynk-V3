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
  debugPrint("aaaaaaaaaaaaaaaaaa");
}

bool formatLittleFS()
{
  LittleFS.end();
  LittleFS.begin();
  LittleFS.format();
  return LittleFS.begin();
}


void startWifiConfigurationAP()
{
  WiFiManager wifiManager;
  wifiManager.setBreakAfterConfig(true);
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //char customHtml[24] = "type=\"checkbox\"";
  //if (envData.blynkLocalServer)
  //{
  //  strcat(customHtml, " checked");
  //}

  //WiFiManagerParameter customBlynkIsLocalServer("blynkIsLocalServer", "Blynk Local Server", "T", 2, customHtml);
  WiFiManagerParameter customBlynkServerHostname("blynkServerHostname", "Blynk Server Hostname", envData.blynkServerHostname, CONFIG_PERSISTENCE_WIFI_BLYNK_HOSTNAME_LEN);
  WiFiManagerParameter custom_BlynkServerPort("blynkServerPort", "Blynk Server Port", String(envData.blynkServerPort).c_str(), 5);
  WiFiManagerParameter custom_BlynkAuth("blynkAuth", "Blynk API key", envData.blynkAuth, CONFIG_PERSISTENCE_WIFI_BLYNK_AUTH_LEN);

  //wifiManager.addParameter(&customBlynkIsLocalServer);
  wifiManager.addParameter(&customBlynkServerHostname);
  wifiManager.addParameter(&custom_BlynkServerPort);
  wifiManager.addParameter(&custom_BlynkAuth);

  wifiManager.startConfigPortal(WIFI_AP_CONFIGURATION_HOSTNAME, WIFI_AP_CONFIGURATION_PASSWORD);
  wifiManager.setConfigPortalBlocking(true);

  if (shouldSaveConfig)
  {
    debugPrintln("Saving wifimanager parameters...");

    envData.blynkLocalServer = strlen( customBlynkServerHostname.getValue()) > 0;
    strcpy(envData.blynkServerHostname, !envData.blynkLocalServer ? "" : customBlynkServerHostname.getValue());
    envData.blynkServerPort = !envData.blynkLocalServer ? 0 :  String(custom_BlynkServerPort.getValue()).toInt();
    
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


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

void startWifiConfigurationAP()
{
    WiFiManager wifiManager;
    wifiManager.setBreakAfterConfig(true);
    wifiManager.setSaveConfigCallback(saveConfigCallback);

    char customHtml[24] = "type=\"checkbox\"";
    if (envData.blynkLocalServer)
    {
        strcat(customHtml, " checked");
    }

    WiFiManagerParameter customBlynkIsLocalServer("blynkIsLocalServer", "Blynk Local Server", "T", 2, customHtml);
    WiFiManagerParameter customBlynkServerHostname("blynkServerHostname", "Blynk Server Hostname", envData.blynkServerHostname, 20);
    WiFiManagerParameter custom_BlynkServerPort("blynkServerPort", "Blynk Server Port", String(envData.blynkServerPort).c_str(), 20);
    WiFiManagerParameter custom_BlynkAuth("blynkAuth", "Blynk API key", String(envData.blynkAuth).c_str(), 32);


    wifiManager.addParameter(&customBlynkIsLocalServer);
    wifiManager.addParameter(&customBlynkServerHostname);
    wifiManager.addParameter(&custom_BlynkServerPort);
    wifiManager.addParameter(&custom_BlynkAuth);

    wifiManager.setParamsPage(true);

    wifiManager.startConfigPortal(WIFI_AP_CONFIGURATION_HOSTNAME, WIFI_AP_CONFIGURATION_PASSWORD);
    wifiManager.setConfigPortalBlocking(true);

}


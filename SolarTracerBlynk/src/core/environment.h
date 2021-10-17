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

struct environrmentData
{
    // wifi
    char wifiSSID[10];
    char wifiPassword[20];
    // blynk
    bool blynkLocalServer;
    char blynkServerHostname[20];
    uint16_t blynkServerPort;
    char blynkAuth[33 * 2];
};

struct environrmentData envData;


void loadEnvData()
{
    // default from config.h
    strcpy(envData.wifiSSID, WIFI_SSID);
    strcpy(envData.wifiPassword, WIFI_PASS);

#ifdef USE_BLYNK_LOCAL_SERVER
    envData.blynkLocalServer = true;
    strcpy(envData.blynkServerHostname, BLYNK_SERVER);
    envData.blynkServerPort = BLYNK_PORT;
#else
    envData.blynkLocalServer = false;
    envData.blynkServerPort = 0;
#endif
    strcpy(envData.blynkAuth, BLYNK_AUTH);

#if defined USE_WIFI_AP_CONFIGURATION
    LittleFS.begin();
    // load from file
    if (!LittleFS.exists(CONFIG_PERSISTENCE))
    {
        // no file found
        debugPrintln("No configuration file found");
    }
    else
    {
        debugPrintln("Restoring configuration from file");
        // file exists, reading and loading
        File configFile = LittleFS.open(CONFIG_PERSISTENCE, "r");
        if (!configFile)
        {
            debugPrintln("ERROR: cannot open config file");
        }
        else
        {
            size_t size = configFile.size();
            DynamicJsonDocument doc(size * 3);
            DeserializationError error = deserializeJson(doc, configFile);
            if (error)
            {
                debugPrintln("ERROR: Cannot deserialize settings from file");
                debugPrintln(error.c_str());
            }
            else
            {
                if (doc.containsKey(CONFIG_PERSISTENCE_WIFI_SSID))
                    strcpy(envData.wifiSSID, doc[CONFIG_PERSISTENCE_WIFI_SSID]);
                if (doc.containsKey(CONFIG_PERSISTENCE_WIFI_PASSWORD))
                    strcpy(envData.wifiPassword, doc[CONFIG_PERSISTENCE_WIFI_PASSWORD]);
                if (doc.containsKey(CONFIG_PERSISTENCE_WIFI_BLYNK_AUTH))
                    strcpy(envData.blynkAuth, doc[CONFIG_PERSISTENCE_WIFI_BLYNK_AUTH]);
                if (doc.containsKey(CONFIG_PERSISTENCE_WIFI_BLYNK_IS_LOCAL))
                    envData.blynkLocalServer = doc[CONFIG_PERSISTENCE_WIFI_BLYNK_IS_LOCAL];
                if (doc.containsKey(CONFIG_PERSISTENCE_WIFI_BLYNK_HOSTNAME))
                    strcpy(envData.blynkServerHostname, doc[CONFIG_PERSISTENCE_WIFI_BLYNK_HOSTNAME]);
                if (doc.containsKey(CONFIG_PERSISTENCE_WIFI_BLYNK_PORT))
                    envData.blynkServerPort = doc[CONFIG_PERSISTENCE_WIFI_BLYNK_PORT];
            }
        }
    }
    LittleFS.end();
#endif
}
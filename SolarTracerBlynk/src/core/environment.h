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

struct environrmentData
{
    // wifi
    char *wifiSSID;
    char *wifiPassword;
    // blynk
    bool blynkLocalServer;
    const char *blynkServerHostname;
    uint16_t blynkServerPort;
    const char *blynkAuth;
};

struct environrmentData envData;

void loadEnvData()
{
    // default from config.h
    envData.wifiSSID = strdup(WIFI_SSID);
    envData.wifiPassword = strdup(WIFI_PASS);

#ifdef USE_BLYNK_LOCAL_SERVER
    envData.blynkLocalServer = true;
    envData.blynkServerHostname = strdup(BLYNK_SERVER);
    envData.blynkServerPort = BLYNK_PORT;
#else
    envData.blynkLocalServer = false;
    envData.blynkServerHostname = nullptr;
    envData.blynkServerPort = 0;
#endif
    envData.blynkAuth = strdup(BLYNK_AUTH);
}


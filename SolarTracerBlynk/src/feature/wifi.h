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

#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include "src/incl/include_all_lib.h"
#include "src/core/Environment.h"

bool configWiFi() {
    const EnvironrmentData *envData = Environment::getData();

    bool wifiDataPresent = strlen(Environment::getData()->wifiSSID);

    if (wifiDataPresent) {
        IPAddress ip;
        IPAddress gateway;
        IPAddress subnet;
        IPAddress dns1;
        IPAddress dns2;

        if (strlen(envData->wifiIp)) {
            ip.fromString(envData->wifiIp);
        }

        if (strlen(envData->wifiGateway)) {
            gateway.fromString(envData->wifiGateway);
        }
        if (strlen(envData->wifiSubnet)) {
            subnet.fromString(envData->wifiSubnet);
        }
        if (strlen(envData->wifiDns1)) {
            dns1.fromString(envData->wifiDns1);
        }
        if (strlen(envData->wifiDns2)) {
            dns2.fromString(envData->wifiDns2);
        }

        WiFi.config(ip, gateway, subnet, dns1, dns2);
        WiFi.begin(envData->wifiSSID, envData->wifiPassword);
    }
    return wifiDataPresent;
}

#endif
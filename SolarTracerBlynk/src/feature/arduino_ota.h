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

void arduinoOtaSetup()
{
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPassword((const char *)OTA_PASS);

    ArduinoOTA.onStart([]()
                       {
                           String type;
                           if (ArduinoOTA.getCommand() == U_FLASH)
                           {
                               type = "sketch";
                           }
                           else
                           { // U_SPIFFS
                               type = "filesystem";
                           }

                           // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                           debugPrintln("Start updating " + type);
                       });

    ArduinoOTA.onEnd([]()
                     { debugPrintln("\nEnd of update"); });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                          { debugPrintf("Progress: %u%%\r", (progress / (total / 100))); });

    ArduinoOTA.onError([](ota_error_t error)
                       {
                           debugPrintf("Error[%u]: ", error);
                           if (error == OTA_AUTH_ERROR)
                           {
                               debugPrintln("Auth Failed");
                           }
                           else if (error == OTA_BEGIN_ERROR)
                           {
                               debugPrintln("Begin Failed");
                           }
                           else if (error == OTA_CONNECT_ERROR)
                           {
                               debugPrintln("Connect Failed");
                           }
                           else if (error == OTA_RECEIVE_ERROR)
                           {
                               debugPrintln("Receive Failed");
                           }
                           else if (error == OTA_END_ERROR)
                           {
                               debugPrintln("End Failed");
                           }
                       });

    ArduinoOTA.begin();
}
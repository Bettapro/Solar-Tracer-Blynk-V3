#pragma once
#include "../incl/project_config.h"

void arduinoOtaSetup()
{
    debugPrintln("Starting ArduinoOTA...");

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
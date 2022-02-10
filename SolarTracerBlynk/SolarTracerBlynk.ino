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
//
#include "src/incl/include_all_core.h"
#include "src/incl/include_all_lib.h"
#include "src/core/datetime.h"
#include "src/core/Controller.h"
#include "src/incl/include_all_feature.h"

// -------------------------------------------------------------------------------
// MISC


void uploadRealtimeAll()
{
#if defined USE_BLYNK
  BlynkSync::getInstance().uploadRealtimeToBlynk();
#endif
#if defined USE_MQTT
  uploadRealtimeToMqtt();
#endif
}
void uploadStatsAll()
{
#if defined USE_BLYNK
  BlynkSync::getInstance().uploadStatsToBlynk();
#endif
#if defined USE_MQTT
  uploadStatsToMqtt();
#endif
}

// ****************************************************************************
// SETUP and LOOP

void loop()
{
  if (!WiFi.isConnected())
  {
    WiFi.reconnect();
    // cannot trust return value of reconnect()
    if (!WiFi.isConnected())
    {
      Controller::getInstance().setErrorFlag(STATUS_ERR_NO_WIFI_CONNECTION, true);
    }
    else
    {
#if defined USE_BLYNK
      BlynkSync::getInstance().connect();
#endif
#if defined USE_MQTT
      mqttConnect();
#endif
    }
  }
  else
  {
    Controller::getInstance().setErrorFlag(STATUS_ERR_NO_WIFI_CONNECTION, false);
  }
#if defined USE_BLYNK
  BlynkSync::getInstance().loop();
#endif
#if defined USE_MQTT
  mqttLoop();
#endif

#ifdef USE_OTA_UPDATE
  ArduinoOTA.handle();
#endif
  Controller::getInstance().getMainTimer()->run();
}

void setup()
{
  BOARD_DEBUG_SERIAL_STREAM.begin(BOARD_DEBUG_SERIAL_STREAM_BAUDRATE);
  debugPrintf(" ++ STARTING TRACER-RS485-MODBUS-BLYNK %s [%d]", PROJECT_VERSION, PROJECT_SUBVERSION);
  debugPrintln();
  Environment::loadEnvData();

#if defined(USE_PIN_RESET_CONFIGURATION_TRIGGER)
  pinMode(PIN_RESET_TRIGGER_PIN, INPUT);
  for (uint8_t readCount = 3; readCount >= 0; readCount--)
  {
    delay(500);
    if (digitalRead(PIN_RESET_TRIGGER_PIN) != PIN_RESET_TRIGGER_VALUE)
    {
      break;
    }
    if (readCount == 0)
    {
      debugPrintln(" ++ Reset configuration");
      Environment::resetEnvData();
      ESP.restart();
    }
  }
#endif

  Controller::getInstance().setErrorFlag(STATUS_RUN_BOOTING, true);
#ifdef USE_STATUS_LED
  ledSetupStart();
#endif

  debugPrintln(" ++ Setting up WIFI:");

  WiFi.mode(WIFI_STA);
  WiFi.begin(Environment::getData()->wifiSSID, Environment::getData()->wifiPassword);

#if defined(USE_HALL_AP_CONFIGURATION_TRIGGER)
  for (uint8_t readCount = 3; readCount >= 0; readCount--)
  {
    delay(500);
    int hallDiff = hallRead() - HALL_AP_CONFIGURATION_BASE_VALUE;
    if (hallDiff < HALL_AP_CONFIGURATION_THR_VALUE && hallDiff > -HALL_AP_CONFIGURATION_THR_VALUE)
    {
      break;
    }
    if (readCount == 0)
    {
      debugPrintln(" ++ Start AP configuration");
      WifiManagerSTB::startWifiConfigurationAP(true);
    }
  }
#endif

#if defined(USE_PIN_AP_CONFIGURATION_TRIGGER)
  pinMode(PIN_AP_TRIGGER_PIN, INPUT);
  for (uint8_t readCount = 3; readCount >= 0; readCount--)
  {
    delay(500);
    if (digitalRead(PIN_AP_TRIGGER_PIN) != PIN_AP_TRIGGER_VALUE)
    {
      break;
    }
    if (readCount == 0)
    {
      debugPrintln(" ++ Start AP configuration");
      WifiManagerSTB::startWifiConfigurationAP(true);
    }
  }
#endif

  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
#if defined USE_WIFI_AP_CONFIGURATION
    WifiManagerSTB::startWifiConfigurationAP(false);
    if (!WiFi.isConnected())
    {
      ESP.restart();
    }
#else
    debugPrintln("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
#endif
  }
  debugPrintln("Connected.");
  debugPrint("IP address: ");
  debugPrintln(WiFi.localIP().toString());

#ifdef USE_OTA_UPDATE
  arduinoOtaSetup();
  debugPrintln("ArduinoOTA is running. ");
#endif

#ifdef USE_SERIAL_STREAM
#if defined(BOARD_ST_SERIAL_PIN_MAPPING_RX) & defined(BOARD_ST_SERIAL_PIN_MAPPING_TX)
  BOARD_ST_SERIAL_STREAM.begin(BOARD_ST_SERIAL_STREAM_BAUDRATE, SERIAL_8N1, BOARD_ST_SERIAL_PIN_MAPPING_RX, BOARD_ST_SERIAL_PIN_MAPPING_TX);
#else
  BOARD_ST_SERIAL_STREAM.begin(BOARD_ST_SERIAL_STREAM_BAUDRATE);
#endif
#endif
  debugPrintln(" ++ Setting Solar Charge Controller:");
  Controller::getInstance().setup();
  debugPrint("Connection Test: ");
  uint8_t attemptControllerConnectionCount;
  for (attemptControllerConnectionCount = 1; attemptControllerConnectionCount < 4; attemptControllerConnectionCount++)
  {
    if (Controller::getInstance().getSolarController()->testConnection())
    {
      break;
    }
    debugPrint(Text::dot);
    delay(1000);
  }

  switch (attemptControllerConnectionCount)
  {
  case 1:
    debugPrintln(Text::ok);
    break;
  case 4:
    debugPrintf(true, Text::errorWithCode, Controller::getInstance().getSolarController()->getLastControllerCommunicationStatus());
    break;
  default:
    debugPrintf(true,"OK [attempt=%i]", attemptControllerConnectionCount);
  }

#ifdef USE_NTP_SERVER
  debugPrintln(" ++ Setting up Local Time:");
  Datetime::setupDatetimeFromNTP();

  struct tm *ti =  Datetime::getMyNowTm();
  debugPrintf(true, "My NOW is: %i-%02i-%02i %02i:%02i:%02i", ti->tm_year + 1900, ti->tm_mon + 1, ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec);
#endif
#if defined USE_BLYNK
  BlynkSync::getInstance().setup();
  debugPrintln("Connected to Blynk.");
#endif
#if defined USE_MQTT
  mqttSetup();
  debugPrintln("Connected to MQTT.");
#endif

  debugPrintln(" ++ Setting up solar tracer:");
#ifdef SYNC_ST_TIME
  debugPrintln("Synchronize NTP time with controller");
  Controller::getInstance().getSolarController()->syncRealtimeClock(Datetime::getMyNowTm());
  delay(500);
#endif
  debugPrintln("Get all values");
  Controller::getInstance().getSolarController()->fetchAllValues();
  delay(1000);
  debugPrintln("Send updates to Blynk");
  uploadRealtimeAll();
  uploadStatsAll();
  delay(1000);

  debugPrintln("Starting timed actions...");

  // periodically refresh tracer values
  Controller::getInstance().getMainTimer()->setInterval(CONTROLLER_UPDATE_MS_PERIOD, []()
                         {
                           debugPrint("Update Solar-Tracer ");
                           if (Controller::getInstance().getSolarController()->updateRun())
                           {
                             debugPrintln(Text::ok);
                             Controller::getInstance().setErrorFlag(STATUS_ERR_SOLAR_TRACER_NO_COMMUNICATION, false);
                           }
                           else
                           {
                             debugPrintf(true, Text::errorWithCode, Controller::getInstance().getSolarController()->getLastControllerCommunicationStatus());
                             Controller::getInstance().setErrorFlag(STATUS_ERR_SOLAR_TRACER_NO_COMMUNICATION, true);
                           }
                         });
  // periodically send STATS all value to blynk
  Controller::getInstance().getMainTimer()->setInterval(SYNC_STATS_MS_PERIOD, uploadStatsAll);
  // periodically send REALTIME  value to blynk
  Controller::getInstance().getMainTimer()->setInterval(SYNC_REALTIME_MS_PERIOD, uploadRealtimeAll);

  debugPrintln("SETUP OK!");
  debugPrintln("----------------------------");
  debugPrintln();

  Controller::getInstance().setErrorFlag(STATUS_RUN_BOOTING, false);
#ifdef USE_STATUS_LED
  ledSetupStop();
#endif
}

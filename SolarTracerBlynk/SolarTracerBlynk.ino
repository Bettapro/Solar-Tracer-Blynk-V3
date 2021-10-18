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

#include "src/incl/project_config.h"

extern SimpleTimer *mainTimer;

// -------------------------------------------------------------------------------
// MISC

#ifdef SYNC_ST_TIME
void getTimeFromServer()
{
  struct tm *ti;
  time_t tnow;
  char strftime_buf[64];

  tnow = time(nullptr) + 1;
  strftime(strftime_buf, sizeof(strftime_buf), "%c", localtime(&tnow));
  ti = localtime(&tnow);
  thisController->syncRealtimeClock(ti);
}
#endif

void uploadRealtimeAll()
{
  uploadRealtimeToBlynk();
}
void uploadStatsAll()
{
  uploadStatsToBlynk();
}

#if defined USE_WIFI_AP_CONFIGURATION

#endif

// ****************************************************************************
// SETUP and LOOP

void loop()
{
  if (WiFi.isConnected())
  {
    clearStatusError(STATUS_ERR_NO_WIFI_CONNECTION);
  }
  else
  {
    setStatusError(STATUS_ERR_NO_WIFI_CONNECTION);
  }
  blynkLoop();
#ifdef USE_OTA_UPDATE
  ArduinoOTA.handle();
#endif
  mainTimer->run();
}

void setup()
{
  BOARD_DEBUG_SERIAL_STREAM.begin(BOARD_DEBUG_SERIAL_STREAM_BAUDRATE);
  debugPrintln(" ++ STARTING TRACER-RS485-MODBUS-BLYNK");

   loadEnvData();

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
      startWifiConfigurationAP();
    }
  }
#endif

 

  setStatusError(STATUS_RUN_BOOTING);
#ifdef USE_STATUS_LED
  ledSetupStart();
#endif

#ifdef USE_SERIAL_STREAM
#if defined(BOARD_ST_SERIAL_PIN_MAPPING_RX) & defined(BOARD_ST_SERIAL_PIN_MAPPING_TX)
  BOARD_ST_SERIAL_STREAM.begin(BOARD_ST_SERIAL_STREAM_BAUDRATE, SERIAL_8N1, BOARD_ST_SERIAL_PIN_MAPPING_RX, BOARD_ST_SERIAL_PIN_MAPPING_TX);
#else
  BOARD_ST_SERIAL_STREAM.begin(BOARD_ST_SERIAL_STREAM_BAUDRATE);
#endif
#endif

  controllerSetup();

  debugPrintln(" ++ Setting up WIFI:");
  debugPrintln("Connecting...");

  WiFi.mode(WIFI_STA);

  WiFi.begin(envData.wifiSSID, envData.wifiPassword);

  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
#if defined USE_WIFI_AP_CONFIGURATION
    startWifiConfigurationAP();
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
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);

  debugPrintln("Connected.");

  blynkSetup();

#ifdef USE_NTP_SERVER
  debugPrintln(" ++ Setting up Local Time:");
  debugPrint("Waiting for NTP time...");
  configTime(0, 0, NTP_SERVER_CONNECT_TO);
  setenv("TZ", CURRENT_TIMEZONE, 3);
  tzset();

  while (time(nullptr) < 100000ul)
  {
    debugPrint(".");
    delay(10);
  }
  debugPrintln(" time synced");

  char strftime_buf[64];

  time_t tnow = time(nullptr) + 1;
  strftime(strftime_buf, sizeof(strftime_buf), "%c", localtime(&tnow));
  struct tm *ti = localtime(&tnow);

  debugPrint("My NOW is: ");
  debugPrint(ti->tm_year + 1900);
  debugPrint("-");
  debugPrint(ti->tm_mon + 1);
  debugPrint("-");
  debugPrint(ti->tm_mday);
  debugPrint(" ");
  debugPrint(ti->tm_hour);
  debugPrint(":");
  debugPrint(ti->tm_min);
  debugPrint(":");
  debugPrintln(ti->tm_sec);

#endif

  debugPrintln();
  debugPrintln("Connected to Blynk.");

#ifdef USE_OTA_UPDATE
  arduinoOtaSetup();

  debugPrint("ArduinoOTA running. ");
  debugPrint("IP address: ");
  debugPrintln(WiFi.localIP().toString());
#endif

  debugPrintln(" ++ Setting up solar tracer:");
#ifdef SYNC_ST_TIME
  debugPrintln("Set NTP time");
  getTimeFromServer();
  delay(500);
#endif
  debugPrintln("Get all values");
  thisController->fetchAllValues();
  delay(1000);
  debugPrintln("Send updates to Blynk");
  uploadRealtimeAll();
  uploadStatsAll();
  delay(1000);

  debugPrintln("Starting timed actions...");

  // periodically refresh tracer values
  mainTimer->setInterval(CONTROLLER_UPDATE_MS_PERIOD, updateSolarController);
  // periodically send STATS all value to blynk
  mainTimer->setInterval(SYNC_STATS_MS_PERIOD, uploadStatsAll);
  // periodically send REALTIME  value to blynk
  mainTimer->setInterval(SYNC_REALTIME_MS_PERIOD, uploadRealtimeAll);

  debugPrintln("SETUP OK!");
  debugPrintln("----------------------------");
  debugPrintln();

  clearStatusError(STATUS_RUN_BOOTING);
#ifdef USE_STATUS_LED
  ledSetupStop();
#endif
}

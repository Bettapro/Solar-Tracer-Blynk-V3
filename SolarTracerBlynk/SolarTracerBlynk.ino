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
#if defined USE_BLYNK
  uploadRealtimeToBlynk();
#endif
#if defined USE_MQTT
  uploadRealtimeToMqtt();
#endif
}
void uploadStatsAll()
{
#if defined USE_BLYNK
  uploadStatsToBlynk();
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
      setStatusError(STATUS_ERR_NO_WIFI_CONNECTION);
    }
  }
  else
  {
    clearStatusError(STATUS_ERR_NO_WIFI_CONNECTION);
  }
#if defined USE_BLYNK
  blynkLoop();
#endif
#if defined USE_MQTT
  mqttLoop();
#endif

#ifdef USE_OTA_UPDATE
  ArduinoOTA.handle();
#endif
  mainTimer->run();
}

void setup()
{
  BOARD_DEBUG_SERIAL_STREAM.begin(BOARD_DEBUG_SERIAL_STREAM_BAUDRATE);
  debugPrintf(" ++ STARTING TRACER-RS485-MODBUS-BLYNK %s [%d]", PROJECT_VERSION, PROJECT_SUBVERSION);
  debugPrintln();
  loadEnvData();

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
      resetEnvData();
      ESP.restart();
    }
  }
#endif

  setStatusError(STATUS_RUN_BOOTING);
#ifdef USE_STATUS_LED
  ledSetupStart();
#endif

  debugPrintln(" ++ Setting up WIFI:");

  WiFi.mode(WIFI_STA);
  WiFi.begin(envData.wifiSSID, envData.wifiPassword);

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
      startWifiConfigurationAP(true);
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
      startWifiConfigurationAP(true);
    }
  }
#endif

  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
#if defined USE_WIFI_AP_CONFIGURATION
    startWifiConfigurationAP(false);
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
  controllerSetup();
  debugPrint("Connection Test: ");
  uint8_t attemptControllerConnectionCount;
  for (attemptControllerConnectionCount = 1; attemptControllerConnectionCount < 4; attemptControllerConnectionCount++)
  {
    if (thisController->testConnection())
    {
      break;
    }
    debugPrint(".");
    delay(1000);
  }

  switch (attemptControllerConnectionCount)
  {
  case 1:
    debugPrint("OK");
    break;
  case 4:
    debugPrintf("KO [err=%i]", thisController->getLastControllerCommunicationStatus());
    break;
  default:
    debugPrintf("OK [attempt=%i]", attemptControllerConnectionCount);
  }
  debugPrintln();

#ifdef USE_NTP_SERVER
  debugPrintln(" ++ Setting up Local Time:");
  debugPrint("Waiting for NTP time...");
  configTzTime(CURRENT_TIMEZONE, NTP_SERVER_CONNECT_TO);

  while (time(nullptr) < 100000ul)
  {
    debugPrint(".");
    delay(500);
  }
  debugPrintln("OK");

  char strftime_buf[64];

  time_t tnow = time(nullptr) + 1;
  strftime(strftime_buf, sizeof(strftime_buf), "%c", localtime(&tnow));
  struct tm *ti = localtime(&tnow);

  debugPrintf("My NOW is: %i-%02i-%02i %02i:%02i:%02i", ti->tm_year + 1900, ti->tm_mon + 1, ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec);
  debugPrintln();

#endif
#if defined USE_BLYNK
  blynkSetup();
  debugPrintln("Connected to Blynk.");
#endif
#if defined USE_MQTT
  mqttSetup();
  debugPrintln("Connected to MQTT.");
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

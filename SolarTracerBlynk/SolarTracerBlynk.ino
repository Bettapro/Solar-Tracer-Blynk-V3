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
#if defined USE_MQTT && !defined USE_MQTT_HOME_ASSISTANT
  MqttSync::getInstance().uploadRealtimeToMqtt();
#endif
#if defined USE_MQTT_HOME_ASSISTANT
  MqttHASync::getInstance().uploadRealtimeToMqtt();
#endif
}
void uploadStatsAll()
{
#if defined USE_BLYNK
  BlynkSync::getInstance().uploadStatsToBlynk();
#endif
#if defined USE_MQTT && !defined USE_MQTT_HOME_ASSISTANT
  MqttSync::getInstance().uploadStatsToMqtt();
#endif
#if defined USE_MQTT_HOME_ASSISTANT
  MqttHASync::getInstance().uploadStatsToMqtt();
#endif
}

void onWifiDisconnected(WiFiEvent_t event)
{
  debugPrintln("WiFi disconnected");
  Controller::getInstance().setErrorFlag(STATUS_ERR_NO_WIFI_CONNECTION, true);
}

// ****************************************************************************
// SETUP and LOOP

void loop()
{
#ifdef USE_OTA_UPDATE
  ArduinoOTA.handle();
#endif
  Controller::getInstance().getMainTimer()->run();

  if (Controller::getInstance().getErrorFlag(STATUS_ERR_NO_WIFI_CONNECTION))
  {
    WiFi.reconnect();
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      delay(1000);
      return;
    }
    else
    {
      Controller::getInstance().setErrorFlag(STATUS_ERR_NO_WIFI_CONNECTION, false);
#if defined USE_BLYNK
      BlynkSync::getInstance().connect();
#endif
#if defined USE_MQTT && !defined USE_MQTT_HOME_ASSISTANT
      MqttSync::getInstance().connect();
#endif
#if defined USE_MQTT_HOME_ASSISTANT
      MqttHASync::getInstance().connect();
#endif
    }
  }

#if defined USE_BLYNK
  BlynkSync::getInstance().loop();
#endif
#if defined USE_MQTT && !defined USE_MQTT_HOME_ASSISTANT
  MqttSync::getInstance().loop();
#endif
#if defined USE_MQTT_HOME_ASSISTANT
  MqttHASync::getInstance().loop();
#endif
}

void setup()
{
  BOARD_DEBUG_SERIAL_STREAM.begin(BOARD_DEBUG_SERIAL_STREAM_BAUDRATE);
  debugPrintf(true, " ++ STARTING %s %s [%d]", PROJECT_NAME, PROJECT_VERSION, PROJECT_SUBVERSION);
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

  debugPrintf(true, Text::setupWithName, "WIFI");

  WiFi.mode(WIFI_STA);

  const environrmentData *envData = Environment::getData();

  IPAddress ip;
  IPAddress gateway;
  IPAddress subnet;
  IPAddress dns1;
  IPAddress dns2;

  if (strlen(envData->wifiIp))
  {
    ip.fromString(envData->wifiIp);
  }

  if (strlen(envData->wifiGateway))
  {
    gateway.fromString(envData->wifiGateway);
  }
  if (strlen(envData->wifiSubnet))
  {
    subnet.fromString(envData->wifiSubnet);
  }
  if (strlen(envData->wifiDns1))
  {
    dns1.fromString(envData->wifiDns1);
  }
  if (strlen(envData->wifiDns2))
  {
    dns2.fromString(envData->wifiDns2);
  }

  WiFi.config(ip, gateway, subnet, dns1, dns2);
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
      ESP.restart();
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
      ESP.restart();
    }
  }
#endif

  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
#if defined USE_WIFI_AP_CONFIGURATION
    WifiManagerSTB::startWifiConfigurationAP(false);
#else
    debugPrintln("Connection Failed! Rebooting...");
    delay(5000);
#endif
    ESP.restart();
  }
  

  WiFi.onEvent(onWifiDisconnected, WIFI_STATION_MODE_DISCONNECTED);

  debugPrintln("Connected.");
  debugPrint("IP address: ");
  debugPrintln(WiFi.localIP().toString());

#ifdef USE_OTA_UPDATE
  debugPrintf(true, Text::setupWithName, "ArduinoOTA");
  arduinoOtaSetup();
  debugPrintln(Text::ok);
#endif

#ifdef USE_SERIAL_STREAM
#if defined(BOARD_ST_SERIAL_PIN_MAPPING_RX) & defined(BOARD_ST_SERIAL_PIN_MAPPING_TX)
  BOARD_ST_SERIAL_STREAM.begin(BOARD_ST_SERIAL_STREAM_BAUDRATE, SERIAL_8N1, BOARD_ST_SERIAL_PIN_MAPPING_RX, BOARD_ST_SERIAL_PIN_MAPPING_TX);
#else
  BOARD_ST_SERIAL_STREAM.begin(BOARD_ST_SERIAL_STREAM_BAUDRATE);
#endif
#endif
  debugPrintf(true, Text::setupWithName, "Solar Charge Controller");
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
    debugPrintf(true, "OK [attempt=%i]", attemptControllerConnectionCount);
  }

#ifdef USE_NTP_SERVER
  debugPrintf(true, Text::setupWithName, "Local Time");
  if (Datetime::setupDatetimeFromNTP())
  {

    struct tm *ti = Datetime::getMyNowTm();
    debugPrintf(true, "My NOW is: %i-%02i-%02i %02i:%02i:%02i", ti->tm_year + 1900, ti->tm_mon + 1, ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec);
  }
#endif
#if defined USE_BLYNK
  BlynkSync::getInstance().setup();
#endif
#if defined USE_MQTT && !defined USE_MQTT_HOME_ASSISTANT
  MqttSync::getInstance().setup();
#endif
#if defined USE_MQTT_HOME_ASSISTANT
  MqttHASync::getInstance().setup();
#endif

  debugPrintf(true, Text::setupWithName, "Solar controller");
#ifdef SYNC_ST_TIME
  debugPrintln("Synchronize NTP time with controller");
  if (Datetime::getMyNowTm() != nullptr)
  {
    Controller::getInstance().getSolarController()->syncRealtimeClock(Datetime::getMyNowTm());
  }
  delay(500);
#endif
#ifdef USE_EXTERNAL_HEAVY_LOAD_CURRENT_METER
  LoadCurrentOverwrite::setup(Controller::getInstance().getSolarController());
  Controller::getInstance().getSolarController()->setOnUpdateRunCompleted([]()
                                                                          { LoadCurrentOverwrite::overWrite(Controller::getInstance().getSolarController()); });
#endif
  debugPrintln("Get all values");
  Controller::getInstance().getSolarController()->fetchAllValues();

  delay(1000);
  debugPrintln("Sync all values");
  uploadRealtimeAll();
  uploadStatsAll();
  delay(1000);

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
                                                          } });
  // periodically send STATS all value to blynk
  Controller::getInstance().getMainTimer()->setInterval(SYNC_STATS_MS_PERIOD, uploadStatsAll);
  // periodically send REALTIME  value to blynk
  Controller::getInstance().getMainTimer()->setInterval(SYNC_REALTIME_MS_PERIOD, uploadRealtimeAll);

  debugPrintln("\r\n\r\n");

  Controller::getInstance().setErrorFlag(STATUS_RUN_BOOTING, false);
#ifdef USE_STATUS_LED
  ledSetupStop();
#endif
}

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

WiFiClient espClient;
PubSubClient mqttClient(espClient);

#define RETAIN_ALL_MSG false

char mqttPublishBuffer[20];


void  uploadRealtimeToMqtt();
void  uploadStatsToMqtt();


void mqttCallback( char* topic, uint8_t* bytes, unsigned int length)
{

  String messageTemp;
  for (int i = 0; i < length; i++)
  {
    messageTemp += (char)bytes[i];
  }

#ifdef MQTT_TOPIC_LOAD_ENABLED
  if (strcmp(MQTT_TOPIC_LOAD_ENABLED, topic) == 0)
  {
    uint8_t newState = (uint8_t)messageTemp.toInt();

    debugPrint("Setting load state output coil to value: ");
    debugPrintln(newState);

    if (thisController->writeBoolValue(SolarTracerVariables::LOAD_MANUAL_ONOFF, newState > 0))
    {
      debugPrintln("Write & Read failed.");
    }
    thisController->fetchValue(SolarTracerVariables::LOAD_MANUAL_ONOFF);
    uploadRealtimeToMqtt();
    return;
  }
#endif
#ifdef MQTT_TOPIC_UPDATE_ALL_CONTROLLER_DATA
  if (strcmp(MQTT_TOPIC_UPDATE_ALL_CONTROLLER_DATA, topic) == 0)
  {
    uint8_t newState = (uint8_t)messageTemp.toInt();
    debugPrintln("Update all");

    if (newState > 0)
    {
      thisController->fetchAllValues();
      uploadRealtimeToMqtt();
      uploadStatsToMqtt();

      dtostrf(0, 5, 4, mqttPublishBuffer);
      mqttClient.publish(MQTT_TOPIC_UPDATE_ALL_CONTROLLER_DATA, mqttPublishBuffer, RETAIN_ALL_MSG);
    }
  }
#endif
#ifdef MQTT_TOPIC_CHARGE_DEVICE_ENABLED
  if (strcmp(MQTT_TOPIC_CHARGE_DEVICE_ENABLED, topic) == 0)
  {
    uint8_t newState = (uint8_t)messageTemp.toInt();

    debugPrint("Setting load state output coil to value: ");
    debugPrintln(newState);

    if (thisController->writeBoolValue(SolarTracerVariables::CHARGING_DEVICE_ONOFF, newState > 0))
    {
      debugPrintln("Write & Read failed.");
    }
    thisController->fetchValue(SolarTracerVariables::CHARGING_DEVICE_ONOFF);
    uploadRealtimeToMqtt();
    return;
  }
#endif
}

boolean mqttAttemptConnection()
{
#ifdef MQTT_PASSWORD
  return mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);
#elif defined MQTT_USERNAME
  return mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME);
#else
  return mqttClient.connect(MQTT_CLIENT_ID);
#endif
}

void mqttSetup()
{
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);

  debugPrintln(" ++ Setting up MQTT:");
  debugPrint("Connecting...");

  while (!mqttAttemptConnection())
  {
    debugPrint(".");
    delay(1000);
  }

#ifdef MQTT_TOPIC_LOAD_ENABLED
  mqttClient.subscribe(MQTT_TOPIC_LOAD_ENABLED);
#endif
#ifdef MQTT_TOPIC_UPDATE_ALL_CONTROLLER_DATA
  mqttClient.subscribe(MQTT_TOPIC_UPDATE_ALL_CONTROLLER_DATA);
#endif
#ifdef MQTT_TOPIC_CHARGE_DEVICE_ENABLED
  mqttClient.subscribe(MQTT_TOPIC_CHARGE_DEVICE_ENABLED);
#endif
}

void mqttLoop()
{
  // must call mqttClient.connected() in order to get subscription updates
  if (!mqttClient.connected()){
   // mqttAttemptConnection();
  }
  mqttClient.loop();
}

bool uploadVariableToMqtt(const mqttSolarVariableMap *varDef)
{
  if (thisController->isVariableReadReady(varDef->solarVariable))
  {
    switch (SolarTracer::getVariableDatatype(varDef->solarVariable))
    {
    case SolarTracerVariablesDataType::FLOAT:
      dtostrf(thisController->getFloatValue(varDef->solarVariable), 0, 4, mqttPublishBuffer);
      mqttClient.publish(varDef->topic, mqttPublishBuffer, RETAIN_ALL_MSG);
      break;
    case SolarTracerVariablesDataType::STRING:
      mqttClient.publish(varDef->topic, thisController->getStringValue(varDef->solarVariable), RETAIN_ALL_MSG);
      break;
    default:
      break;
    }
    return true;
  }
  return false;
}

// upload values stats
void uploadStatsToMqtt()
{
  if (!mqttClient.connected())
  {
    setStatusError(STATUS_ERR_NO_MQTT_CONNECTION);
    return;
  }
  clearStatusError(STATUS_ERR_NO_MQTT_CONNECTION);
  bool varNotReady = false;
  for (uint8_t index = 0; index < statVirtualMqttSolarVariablesCount; index++)
  {
    varNotReady |= !uploadVariableToMqtt(&statVirtualMqttSolarVariables[index]);
  }
  if (varNotReady)
  {
    debugPrintln("Some ST variables are not ready and not synced!");
    setStatusError(STATUS_ERR_SOLAR_TRACER_NO_SYNC_ST);
  }
  else
  {
    clearStatusError(STATUS_ERR_SOLAR_TRACER_NO_SYNC_ST);
  }
}

// upload values realtime
void uploadRealtimeToMqtt()
{

  if (!mqttClient.connected())
  {
    setStatusError(STATUS_ERR_NO_MQTT_CONNECTION);
    return;
  }
  clearStatusError(STATUS_ERR_NO_MQTT_CONNECTION);

#ifdef MQTT_TOPIC_INTERNAL_STATUS
  dtostrf(internalStatus, 0, 4, mqttPublishBuffer);
  mqttClient.publish(MQTT_TOPIC_INTERNAL_STATUS, mqttPublishBuffer, RETAIN_ALL_MSG);
#endif

  bool varNotReady = false;
  for (uint8_t index = 0; index < realTimeVirtualMqttkSolarVariablesCount; index++)
  {
    varNotReady |= !uploadVariableToMqtt(&realTimeVirtualMqttSolarVariables[index]);
  }
  if (varNotReady)
  {
    debugPrintln("Some RT variables are not ready and not synced!");
    setStatusError(STATUS_ERR_SOLAR_TRACER_NO_SYNC_RT);
  }
  else
  {
    clearStatusError(STATUS_ERR_SOLAR_TRACER_NO_SYNC_RT);
  }
}

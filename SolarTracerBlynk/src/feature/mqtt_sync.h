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

void uploadRealtimeToMqtt();
void uploadStatsToMqtt();

void executeFromMqttFloatWrite(SolarTracerVariables variable, float *value)
{
  if (!thisController->writeValue(variable, value))
  {
    debugPrintf(" - FAILED! [err=%i]", thisController->getLastControllerCommunicationStatus());
  }
  thisController->fetchValue(variable);
  uploadRealtimeToMqtt();
}

void executeFromMqttBoolWrite(SolarTracerVariables variable, bool *value)
{
  if (!thisController->writeValue(variable, value))
  {
    debugPrintf(" - FAILED! [err=%i]", thisController->getLastControllerCommunicationStatus());
  }
  thisController->fetchValue(variable);
  uploadRealtimeToMqtt();
}

#if defined(USE_MQTT_RPC_SUBSCRIBE) || defined(USE_MQTT_JSON_PUBLISH)
DynamicJsonDocument json(1024);
#endif;

void mqttCallback(char *topic, uint8_t *bytes, unsigned int length)
{
  String payload;
  for (int i = 0; i < length; i++)
  {
    payload += (char)bytes[i];
  }

#ifdef USE_MQTT_RPC_SUBSCRIBE
  deserializeJson(json, payload);
  const char *constTopic = json["method"].as<const char *>();
  payload = json["params"].as<String>();
#else
  const char *constTopic = topic;
#endif

#ifdef MQTT_TOPIC_LOAD_ENABLED
  if (strcmp(MQTT_TOPIC_LOAD_ENABLED, constTopic) == 0)
  {
    debugPrint("SET NEW VALUE FOR MQTT_TOPIC_LOAD_ENABLED");
    bool newState = payload.toInt() > 0;
    executeFromMqttBoolWrite(SolarTracerVariables::LOAD_MANUAL_ONOFF, &newState);
    debugPrintln();
    return;
  }
#endif
#ifdef MQTT_TOPIC_UPDATE_ALL_CONTROLLER_DATA
  if (strcmp(MQTT_TOPIC_UPDATE_ALL_CONTROLLER_DATA, constTopic) == 0)
  {
    uint8_t newState = (uint8_t)payload.toInt();
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
  if (strcmp(MQTT_TOPIC_CHARGE_DEVICE_ENABLED, constTopic) == 0)
  {
    debugPrint("SET NEW VALUE FOR MQTT_TOPIC_CHARGE_DEVICE_ENABLED");
    bool newState = payload.toInt() > 0;
    executeFromMqttBoolWrite(SolarTracerVariables::CHARGING_DEVICE_ONOFF, &newState);
    debugPrintln();
    return;
  }
#endif

  /*
  * TODO implementation for:
  * MQTT_TOPIC_BATTERY_BOOST_VOLTAGE                
  * MQTT_TOPIC_BATTERY_EQUALIZATION_VOLTAGE             
  * MQTT_TOPIC_BATTERY_FLOAT_VOLTAGE                   
  * MQTT_TOPIC_BATTERY_FLOAT_MIN_VOLTAGE                 
  * MQTT_TOPIC_BATTERY_CHARGING_LIMIT_VOLTAGE            
  * MQTT_TOPIC_BATTERY_DISCHARGING_LIMIT_VOLTAGE         
  * MQTT_TOPIC_BATTERY_LOW_VOLTAGE_DISCONNECT           
  * MQTT_TOPIC_BATTERY_LOW_VOLTAGE_RECONNECT           
  * MQTT_TOPIC_BATTERY_OVER_VOLTAGE_DISCONNECT          
  * MQTT_TOPIC_BATTERY_OVER_VOLTAGE_RECONNECT         
  * MQTT_TOPIC_BATTERY_UNDER_VOLTAGE_RESET              
  * MQTT_TOPIC_BATTERY_UNDER_VOLTAGE_SET                                     
 */
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
#ifdef USE_MQTT_RPC_SUBSCRIBE
  mqttClient.subscribe(MQTT_RPC_SUBSCRIBE_TOPIC);
#endif
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
  if (!mqttClient.connected())
  {
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
#ifdef USE_MQTT_JSON_PUBLISH
      json[varDef->topic] = thisController->getFloatValue(varDef->solarVariable);
#else
      dtostrf(thisController->getFloatValue(varDef->solarVariable), 0, 4, mqttPublishBuffer);
      mqttClient.publish(varDef->topic, mqttPublishBuffer, RETAIN_ALL_MSG);
#endif
      break;
    case SolarTracerVariablesDataType::STRING:
#ifdef USE_MQTT_JSON_PUBLISH
      json[varDef->topic] = thisController->getStringValue(varDef->solarVariable);
#else
      mqttClient.publish(varDef->topic, thisController->getStringValue(varDef->solarVariable), RETAIN_ALL_MSG);
#endif
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
  json.clear();
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

#ifdef USE_MQTT_JSON_PUBLISH
  String output;
  serializeJson(json, output);
  if (json.size() > 0)
  {
    mqttClient.publish(MQTT_JSON_PUBLISH_TOPIC, output.c_str());
  }
#endif
}

// upload values realtime
void uploadRealtimeToMqtt()
{
  json.clear();
  if (!mqttClient.connected())
  {
    setStatusError(STATUS_ERR_NO_MQTT_CONNECTION);
    return;
  }
  clearStatusError(STATUS_ERR_NO_MQTT_CONNECTION);

#ifdef MQTT_TOPIC_INTERNAL_STATUS
#ifdef USE_MQTT_JSON_PUBLISH
  json[MQTT_TOPIC_INTERNAL_STATUS] = internalStatus;
#else
  dtostrf(internalStatus, 0, 4, mqttPublishBuffer);
  mqttClient.publish(MQTT_TOPIC_INTERNAL_STATUS, mqttPublishBuffer, RETAIN_ALL_MSG);
#endif
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

#ifdef USE_MQTT_JSON_PUBLISH
  String output;
  serializeJson(json, output);
  if (json.size() > 0)
  {
    mqttClient.publish(MQTT_JSON_PUBLISH_TOPIC, output.c_str());
  }
#endif
}

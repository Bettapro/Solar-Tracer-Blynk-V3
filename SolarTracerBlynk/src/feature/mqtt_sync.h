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

char mqttPublishBuffer[20]; 

void mqttCallback(char *topic, byte *message, unsigned int length)
{
  
}

void mqttSetup()
{
  // Set the MQTT server to the server stated above ^
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

  debugPrintln(" ++ Setting up MQTT:");
  debugPrint("Connecting...");

  while (!mqttClient.connect(MQTT_CLIENT_ID))
  {
    debugPrint(".");
    delay(100);
  }
}

void mqttLoop()
{
  mqttClient.loop();
}

bool uploadVariableToMqtt(const mqttSolarVariableMap* varDef){
  if (thisController->isVariableReadReady(varDef->solarVariable))
    {
      switch (SolarTracer::getVariableDatatype(varDef->solarVariable))
      {
      case SolarTracerVariablesDataType::FLOAT:
        dtostrf(thisController->getFloatValue(varDef->solarVariable), 0, 4, mqttPublishBuffer);
        mqttClient.publish(varDef->topic, mqttPublishBuffer);
        break;
      case SolarTracerVariablesDataType::STRING:
        mqttClient.publish(varDef->topic, thisController->getStringValue(varDef->solarVariable));
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
  if(!mqttClient.connected()){
    setStatusError(STATUS_ERR_NO_MQTT_CONNECTION);
    return;
  }
  clearStatusError(STATUS_ERR_NO_MQTT_CONNECTION);
  bool varNotReady = false;
  for (uint8_t index = 0; index < statVirtualMqttSolarVariablesCount; index++)
  {
    varNotReady |= ! uploadVariableToMqtt(&statVirtualMqttSolarVariables[index]);
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
  if(!mqttClient.connected()){
    setStatusError(STATUS_ERR_NO_MQTT_CONNECTION);
    return;
  }
  clearStatusError(STATUS_ERR_NO_MQTT_CONNECTION);
  bool varNotReady = false;
  for (uint8_t index = 0; index < realTimeVirtualMqttkSolarVariablesCount; index++)
  {
    varNotReady |= ! uploadVariableToMqtt(&realTimeVirtualMqttSolarVariables[index]);
  }
  if (varNotReady)
  {
    debugPrintln("Some ST variables are not ready and not synced!");
    setStatusError(STATUS_ERR_SOLAR_TRACER_NO_SYNC_RT);
  }
  else
  {
    clearStatusError(STATUS_ERR_SOLAR_TRACER_NO_SYNC_RT);
  }
}

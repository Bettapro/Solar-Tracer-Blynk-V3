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

extern SolarTracer *thisController;

void blynkSetup()
{
  debugPrintln(" ++ Setting up Blynk:");
  debugPrint("Connecting...");

  if (envData.blynkLocalServer)
  {
    Blynk.config(envData.blynkAuth, envData.blynkServerHostname, envData.blynkServerPort);
  }
  else
  {
    Blynk.config(envData.blynkAuth);
  }

  while (!Blynk.connect())
  {
    debugPrint(".");
    delay(500);
  }
  debugPrintln("OK");
  clearStatusError(STATUS_ERR_NO_BLYNK_CONNECTION);
}

void blynkLoop()
{
  if (!Blynk.connected())
  {
    setStatusError(STATUS_ERR_NO_BLYNK_CONNECTION);
  }
  else
  {
    clearStatusError(STATUS_ERR_NO_BLYNK_CONNECTION);
  }

  Blynk.run();
}

// upload values stats
void uploadStatsToBlynk()
{
  if (!Blynk.connected())
  {
    return;
  }

  bool varNotReady = false;
  for (uint8_t index = 0; index < statVirtualBlynkSolarVariablesCount; index++)
  {
    if (thisController->isVariableReadReady(statVirtualBlynkSolarVariables[index].solarVariable))
    {
      switch (SolarTracer::getVariableDatatype(statVirtualBlynkSolarVariables[index].solarVariable))
      {
      case SolarTracerVariablesDataType::FLOAT:
        Blynk.virtualWrite(statVirtualBlynkSolarVariables[index].virtualPin, thisController->getFloatValue(statVirtualBlynkSolarVariables[index].solarVariable));
        break;
      case SolarTracerVariablesDataType::STRING:
        Blynk.virtualWrite(statVirtualBlynkSolarVariables[index].virtualPin, thisController->getStringValue(statVirtualBlynkSolarVariables[index].solarVariable));
        break;
      default:
        break;
      }
    }
    else
    {
      varNotReady = true;
    }
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
void uploadRealtimeToBlynk()
{
  if (!Blynk.connected())
  {
    return;
  }

#ifdef vPIN_INTERNAL_STATUS
  Blynk.virtualWrite(vPIN_INTERNAL_STATUS, internalStatus);
#endif

  bool varNotReady = false;
  for (uint8_t index = 0; index < realTimeVirtualBlynkSolarVariablesCount; index++)
  {
    if (thisController->isVariableReadReady(realTimeVirtualBlynkSolarVariables[index].solarVariable))
    {
      switch (SolarTracer::getVariableDatatype(realTimeVirtualBlynkSolarVariables[index].solarVariable))
      {
      case SolarTracerVariablesDataType::FLOAT:
        Blynk.virtualWrite(realTimeVirtualBlynkSolarVariables[index].virtualPin, thisController->getFloatValue(realTimeVirtualBlynkSolarVariables[index].solarVariable));
        break;
      case SolarTracerVariablesDataType::STRING:
        Blynk.virtualWrite(realTimeVirtualBlynkSolarVariables[index].virtualPin, thisController->getStringValue(realTimeVirtualBlynkSolarVariables[index].solarVariable));
        break;
      default:
        break;
      }
    }
    else
    {
      varNotReady = true;
    }
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

void executeFromBlynkFloatWrite(SolarTracerVariables variable, float* value){
  if (!thisController->writeValue(variable, value))
  {
    debugPrint (" - FAILED!");
  }
  thisController->fetchValue(variable);
  uploadRealtimeToBlynk();
}

void executeFromBlynkBoolWrite(SolarTracerVariables variable, bool* value){
  if (!thisController->writeValue(variable, value))
  {
    debugPrint (" - FAILED!");
  }
  thisController->fetchValue(variable);
  uploadRealtimeToBlynk();
}

#ifdef vPIN_LOAD_ENABLED
BLYNK_WRITE(vPIN_LOAD_ENABLED)
{
  debugPrint("SET NEW VALUE FOR vPIN_LOAD_ENABLED");
  bool newState = param.asInt() > 0;
  executeFromBlynkBoolWrite(SolarTracerVariables::LOAD_MANUAL_ONOFF, &newState);
  debugPrintln();
}
#endif

#ifdef vPIN_CHARGE_DEVICE_ENABLED
BLYNK_WRITE(vPIN_CHARGE_DEVICE_ENABLED)
{
  debugPrint("SET NEW VALUE FOR vPIN_CHARGE_DEVICE_ENABLED");
  bool newState = param.asInt() > 0;
  executeFromBlynkBoolWrite(SolarTracerVariables::CHARGING_DEVICE_ONOFF, &newState);
  debugPrintln();
}
#endif

#ifdef vPIN_UPDATE_ALL_CONTROLLER_DATA
BLYNK_WRITE(vPIN_UPDATE_ALL_CONTROLLER_DATA)
{
  uint8_t newState = (uint8_t)param.asInt();
  Blynk.virtualWrite(vPIN_UPDATE_ALL_CONTROLLER_DATA, newState);
  if (newState > 0)
  {
    thisController->fetchAllValues();
    uploadRealtimeToBlynk();
    uploadStatsToBlynk();

    Blynk.virtualWrite(vPIN_UPDATE_ALL_CONTROLLER_DATA, 0);
  }
}
#endif

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

#define BLYNK_CONNECT_ATTEMPT 3
#define BLYNK_VALUE_CACHES_UNTIL_COUNT 5
#define BLYNK_VALUE_CACHES_STRING_LEN 20

void *blynkValuesCache[statVirtualBlynkSolarVariablesCount + realTimeVirtualBlynkSolarVariablesCount];
uint8_t blynkValuesCacheValid[statVirtualBlynkSolarVariablesCount + realTimeVirtualBlynkSolarVariablesCount];

#ifdef vPIN_INTERNAL_DEBUG_TERMINAL
void blynkDebugCallback(String message)
{
  Blynk.virtualWrite(vPIN_INTERNAL_DEBUG_TERMINAL, message);
}
#endif

void blynkSetup();
void blynkConnect();
void blynkLoop();

void blynkSetup()
{
  blynkConnect();

#ifdef vPIN_INTERNAL_DEBUG_TERMINAL
  debugAddRegisterCallback(blynkDebugCallback);
  blynkDebugCallback("Solar Tracer START\r\n");
#endif

  for (uint8_t index = 0; index < statVirtualBlynkSolarVariablesCount + realTimeVirtualBlynkSolarVariablesCount; index++)
  {
    blynkValuesCacheValid[index] = 0;

    switch (SolarTracer::getVariableDatatype(index >= realTimeVirtualBlynkSolarVariablesCount ? statVirtualBlynkSolarVariables[index - realTimeVirtualBlynkSolarVariablesCount].solarVariable : realTimeVirtualBlynkSolarVariables[index].solarVariable))
    {
    case SolarTracerVariablesDataType::FLOAT:
      blynkValuesCache[index] = malloc(sizeof(float));
      break;
    case SolarTracerVariablesDataType::STRING:
      blynkValuesCache[index] = new char[BLYNK_VALUE_CACHES_STRING_LEN];
      break;
    }
  }
}

void blynkConnect()
{
  debugPrintln(" ++ Setting up Blynk:");
  debugPrint("Connecting...");

#ifdef USE_BLYNK_2
   Blynk.config(envData.blynkAuth);
#else
 if (envData.blynkLocalServer)
  {
    Blynk.config(envData.blynkAuth, envData.blynkServerHostname, envData.blynkServerPort);
  }
  else
  {
    Blynk.config(envData.blynkAuth);
  }
#endif
 

  uint8_t counter = 0;

  while (counter < BLYNK_CONNECT_ATTEMPT && !Blynk.connect())
  {
    debugPrint(".");
    delay(500);
#ifndef BLYNK_CONNECTION_REQUIRED
    counter++;
#endif
  }

  debugPrintln(Blynk.connected() ? "OK" : "KO");
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

bool updateBlynkVariable(bool isRT, uint8_t index, SolarTracerVariables variable, int pin)
{
  if (thisController->isVariableEnabled(variable) &&
      thisController->isVariableReadReady(variable))
  {
    void *cachedValue = blynkValuesCache[index + (isRT ? 0 : realTimeVirtualBlynkSolarVariablesCount)];
    uint8_t *cachedUntil = &(blynkValuesCacheValid[index + (isRT ? 0 : realTimeVirtualBlynkSolarVariablesCount)]);

    switch (SolarTracer::getVariableDatatype(variable))
    {
    case SolarTracerVariablesDataType::FLOAT:
    {
      float currentValue = thisController->getFloatValue(variable);
      if (*cachedUntil <= 0 || (*(float *)cachedValue) != currentValue)
      {
        Blynk.virtualWrite(pin, thisController->getFloatValue(variable));
        (*(float *)cachedValue) = currentValue;
        (*cachedUntil) = BLYNK_VALUE_CACHES_UNTIL_COUNT;
      }
      (*cachedUntil)--;
    }
    break;
    case SolarTracerVariablesDataType::STRING:
    {
      const char *currentValue = thisController->getStringValue(variable);
      if (*cachedUntil <= 0 || strcmp(currentValue, (const char *)cachedValue) != 0)
      {
        Blynk.virtualWrite(pin, thisController->getStringValue(variable));
        strcpy((char *)cachedValue, currentValue);
        (*cachedUntil) = BLYNK_VALUE_CACHES_UNTIL_COUNT;
      }
      (*cachedUntil)--;
    }
    break;
    default:
      return false;
    }
    return true;
  }
  return false;
}

// upload values stats
void uploadStatsToBlynk()
{
  if (!Blynk.connected())
  {
    return;
  }

  uint8_t varNotReady = 0;
  for (uint8_t index = 0; index < statVirtualBlynkSolarVariablesCount; index++)
  {
    varNotReady += updateBlynkVariable(false, index, statVirtualBlynkSolarVariables[index].solarVariable, statVirtualBlynkSolarVariables[index].virtualPin) ? 0 : 1;
  }
  if (varNotReady > 0)
  {
    debugPrintf("WARNING %i ST var. are not ready & synced!\r\n", varNotReady);
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

  uint8_t varNotReady = 0;
  for (uint8_t index = 0; index < realTimeVirtualBlynkSolarVariablesCount; index++)
  {
    varNotReady += updateBlynkVariable(true, index, realTimeVirtualBlynkSolarVariables[index].solarVariable, realTimeVirtualBlynkSolarVariables[index].virtualPin) ? 0 : 1;
  }
  if (varNotReady > 0)
  {
    debugPrintf("WARNING %i RT var. are not ready & synced!\r\n", varNotReady);
    setStatusError(STATUS_ERR_SOLAR_TRACER_NO_SYNC_RT);
  }
  else
  {
    clearStatusError(STATUS_ERR_SOLAR_TRACER_NO_SYNC_RT);
  }
}

void executeFromBlynkFloatWrite(SolarTracerVariables variable, float *value)
{
  if (!thisController->writeValue(variable, value))
  {
    debugPrintf(" - FAILED! [err=%i]", thisController->getLastControllerCommunicationStatus());
  }
  thisController->fetchValue(variable);
  uploadRealtimeToBlynk();
}

void executeFromBlynkBoolWrite(SolarTracerVariables variable, bool *value)
{
  if (!thisController->writeValue(variable, value))
  {
    debugPrintf(" - FAILED! [err=%i]", thisController->getLastControllerCommunicationStatus());
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

#ifdef vPIN_BATTERY_BOOST_VOLTAGE
BLYNK_WRITE(vPIN_BATTERY_BOOST_VOLTAGE)
{
  debugPrint("SET NEW VALUE FOR vPIN_BATTERY_BOOST_VOLTAGE");
  float newState = param.asFloat();
  executeFromBlynkFloatWrite(SolarTracerVariables::BATTERY_BOOST_VOLTAGE, &newState);
  debugPrintln();
}
#endif

#ifdef vPIN_BATTERY_EQUALIZATION_VOLTAGE
BLYNK_WRITE(vPIN_BATTERY_EQUALIZATION_VOLTAGE)
{
  debugPrint("SET NEW VALUE FOR vPIN_BATTERY_EQUALIZATION_VOLTAGE");
  float newState = param.asFloat();
  executeFromBlynkFloatWrite(SolarTracerVariables::BATTERY_EQUALIZATION_VOLTAGE, &newState);
  debugPrintln();
}
#endif

#ifdef vPIN_BATTERY_FLOAT_VOLTAGE
BLYNK_WRITE(vPIN_BATTERY_FLOAT_VOLTAGE)
{
  debugPrint("SET NEW VALUE FOR vPIN_BATTERY_FLOAT_VOLTAGE");
  float newState = param.asFloat();
  executeFromBlynkFloatWrite(SolarTracerVariables::BATTERY_FLOAT_VOLTAGE, &newState);
  debugPrintln();
}
#endif

#ifdef vPIN_BATTERY_FLOAT_MIN_VOLTAGE
BLYNK_WRITE(vPIN_BATTERY_FLOAT_MIN_VOLTAGE)
{
  debugPrint("SET NEW VALUE FOR vPIN_BATTERY_FLOAT_MIN_VOLTAGE");
  float newState = param.asFloat();
  executeFromBlynkFloatWrite(SolarTracerVariables::BATTERY_FLOAT_MIN_VOLTAGE, &newState);
  debugPrintln();
}
#endif

#ifdef vPIN_BATTERY_CHARGING_LIMIT_VOLTAGE
BLYNK_WRITE(vPIN_BATTERY_CHARGING_LIMIT_VOLTAGE)
{
  debugPrint("SET NEW VALUE FOR vPIN_BATTERY_CHARGING_LIMIT_VOLTAGE");
  float newState = param.asFloat();
  executeFromBlynkFloatWrite(SolarTracerVariables::BATTERY_CHARGING_LIMIT_VOLTAGE, &newState);
  debugPrintln();
}
#endif

#ifdef vPIN_BATTERY_DISCHARGING_LIMIT_VOLTAGE
BLYNK_WRITE(vPIN_BATTERY_DISCHARGING_LIMIT_VOLTAGE)
{
  debugPrint("SET NEW VALUE FOR vPIN_BATTERY_DISCHARGING_LIMIT_VOLTAGE");
  float newState = param.asFloat();
  executeFromBlynkFloatWrite(SolarTracerVariables::BATTERY_DISCHARGING_LIMIT_VOLTAGE, &newState);
  debugPrintln();
}
#endif

#ifdef vPIN_BATTERY_LOW_VOLTAGE_DISCONNECT
BLYNK_WRITE(vPIN_BATTERY_LOW_VOLTAGE_DISCONNECT)
{
  debugPrint("SET NEW VALUE FOR vPIN_BATTERY_LOW_VOLTAGE_DISCONNECT");
  float newState = param.asFloat();
  executeFromBlynkFloatWrite(SolarTracerVariables::BATTERY_LOW_VOLTAGE_DISCONNECT, &newState);
  debugPrintln();
}
#endif

#ifdef vPIN_BATTERY_LOW_VOLTAGE_RECONNECT
BLYNK_WRITE(vPIN_BATTERY_LOW_VOLTAGE_RECONNECT)
{
  debugPrint("SET NEW VALUE FOR vPIN_BATTERY_LOW_VOLTAGE_RECONNECT");
  float newState = param.asFloat();
  executeFromBlynkFloatWrite(SolarTracerVariables::BATTERY_LOW_VOLTAGE_RECONNECT, &newState);
  debugPrintln();
}
#endif

#ifdef vPIN_BATTERY_OVER_VOLTAGE_DISCONNECT
BLYNK_WRITE(vPIN_BATTERY_OVER_VOLTAGE_DISCONNECT)
{
  debugPrint("SET NEW VALUE FOR vPIN_BATTERY_OVER_VOLTAGE_DISCONNECT");
  float newState = param.asFloat();
  executeFromBlynkFloatWrite(SolarTracerVariables::BATTERY_OVER_VOLTAGE_DISCONNECT, &newState);
  debugPrintln();
}
#endif

#ifdef vPIN_BATTERY_OVER_VOLTAGE_RECONNECT
BLYNK_WRITE(vPIN_BATTERY_OVER_VOLTAGE_RECONNECT)
{
  debugPrint("SET NEW VALUE FOR vPIN_BATTERY_OVER_VOLTAGE_RECONNECT");
  float newState = param.asFloat();
  executeFromBlynkFloatWrite(SolarTracerVariables::BATTERY_OVER_VOLTAGE_RECONNECT, &newState);
  debugPrintln();
}
#endif

#ifdef vPIN_BATTERY_UNDER_VOLTAGE_RESET
BLYNK_WRITE(vPIN_BATTERY_UNDER_VOLTAGE_RESET)
{
  debugPrint("SET NEW VALUE FOR vPIN_BATTERY_UNDER_VOLTAGE_RESET");
  float newState = param.asFloat();
  executeFromBlynkFloatWrite(SolarTracerVariables::BATTERY_UNDER_VOLTAGE_RESET, &newState);
  debugPrintln();
}
#endif

#ifdef vPIN_BATTERY_UNDER_VOLTAGE_SET
BLYNK_WRITE(vPIN_BATTERY_UNDER_VOLTAGE_SET)
{
  debugPrint("SET NEW VALUE FOR vPIN_BATTERY_UNDER_VOLTAGE_SET");
  float newState = param.asFloat();
  executeFromBlynkFloatWrite(SolarTracerVariables::BATTERY_UNDER_VOLTAGE_SET, &newState);
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
    debugPrintln("REQUEST ALL VALUES TO CONTROLLER");
    thisController->fetchAllValues();
    uploadRealtimeToBlynk();
    uploadStatsToBlynk();

    Blynk.virtualWrite(vPIN_UPDATE_ALL_CONTROLLER_DATA, 0);
  }
}
#endif

#ifdef vPIN_UPDATE_CONTROLLER_DATETIME
BLYNK_WRITE(vPIN_UPDATE_CONTROLLER_DATETIME)
{
  uint8_t newState = (uint8_t)param.asInt();
  Blynk.virtualWrite(vPIN_UPDATE_CONTROLLER_DATETIME, newState);
  if (newState > 0)
  {
    debugPrintln("UPDATE CONTROLLER DATETIME");
    thisController->syncRealtimeClock(getMyNowTm());
    Blynk.virtualWrite(vPIN_UPDATE_CONTROLLER_DATETIME, 0);
  }
}
#endif
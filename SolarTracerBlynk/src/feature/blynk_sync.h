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

#if !defined(blynk_sync_h) && defined(USE_BLYNK)
#define blynk_sync_h

#define BLYNK_CONNECT_ATTEMPT 3
#define BLYNK_VALUE_CACHES_UNTIL_COUNT 5
#define BLYNK_VALUE_CACHES_STRING_LEN 20

void *blynkValuesCache[Variable::VARIABLES_COUNT];
uint8_t blynkValuesCacheValid[Variable::VARIABLES_COUNT];

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

  for (uint8_t index = 0; index < Variable::VARIABLES_COUNT; index++)
  {
    blynkValuesCacheValid[index] = 0;

    switch (VariableDefiner::getInstance().getDatatype((Variable)index))
    {
    case VariableDatatype::DT_FLOAT:
      blynkValuesCache[index] = malloc(sizeof(float));
      break;
    case VariableDatatype::DT_STRING:
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
  Controller::getInstance().setStatusFlag(STATUS_ERR_NO_BLYNK_CONNECTION, !Blynk.connected());
  Blynk.run();
}

bool updateBlynkVariable(uint8_t index, Variable variable, int pin)
{
  SolarTracer *thisController = Controller::getInstance().getSolarController();
  if (thisController->isVariableEnabled(variable) &&
      thisController->isVariableReadReady(variable))
  {
    void *cachedValue = blynkValuesCache[index];
    uint8_t *cachedUntil = &(blynkValuesCacheValid[index]);

    switch (VariableDefiner::getInstance().getDatatype(variable))
    {
    case VariableDatatype::DT_FLOAT:
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
    case VariableDatatype::DT_STRING:
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

uint8_t uploadDataToBlynk(bool uploadRealtime, bool uploadStat, bool uploadInternal)
{

  uint8_t varNotReady = 0;
  const VariableDefinition *def;
  int pin;
  for (uint8_t index = 0; index < Variable::VARIABLES_COUNT; index++)
  {
    def = VariableDefiner::getInstance().getDefinition((Variable)index);
    if (def->blynkVPin != nullptr && ((uploadRealtime && def->source == VariableSource::SR_REALTIME) ||
                                      (uploadStat && def->source == VariableSource::SR_INTERNAL) ||
                                      (uploadInternal && def->source == VariableSource::SR_STATS)))
    {
      pin = *(def->blynkVPin) + 0;
      varNotReady *= updateBlynkVariable(index, def->variable, pin) ? 0 : 1;
    }
  }

  return varNotReady;
}

// upload values stats
void uploadStatsToBlynk()
{
  if (!Blynk.connected())
  {
    return;
  }

  uint8_t varNotReady = uploadDataToBlynk(false, true, true);
  if (varNotReady > 0)
  {
    debugPrintf("WARNING %i ST var. are not ready & synced!\r\n", varNotReady);
  }
  Controller::getInstance().setStatusFlag(STATUS_ERR_SOLAR_TRACER_NO_SYNC_ST, varNotReady > 0);
}

// upload values realtime
void uploadRealtimeToBlynk()
{
  if (!Blynk.connected())
  {
    return;
  }

#ifdef vPIN_INTERNAL_STATUS
  Blynk.virtualWrite(vPIN_INTERNAL_STATUS, Controller::getInstance().getStatus());
#endif

  uint8_t varNotReady = uploadDataToBlynk(false, true, true);
  if (varNotReady > 0)
  {
    debugPrintf("WARNING %i RT var. are not ready & synced!\r\n", varNotReady);
  }
  Controller::getInstance().setStatusFlag(STATUS_ERR_SOLAR_TRACER_NO_SYNC_RT, varNotReady > 0);
}

void executeFromBlynkWrite(Variable variable, void *value)
{
  if (!Controller::getInstance().getSolarController()->writeValue(variable, value))
  {
    debugPrintf(" - FAILED! [err=%i]", Controller::getInstance().getSolarController()->getLastControllerCommunicationStatus());
  }
  Controller::getInstance().getSolarController()->fetchValue(variable);
  uploadRealtimeToBlynk();
}

BLYNK_WRITE_DEFAULT()
{
  const VariableDefinition *def = VariableDefiner::getInstance().getDefinitionByBlynkVPin(request.pin);
  if (def != nullptr)
  {
    switch (def->datatype)
    {
    case VariableDatatype::DT_FLOAT:
    {
      float newState = param.asFloat();
      debugPrintf("WRITE REQUEST : \"%s\" to %.4f", def->text, newState);
      executeFromBlynkWrite(def->variable, &newState);
      debugPrintln();
    }
    break;
    }
  }
}

#ifdef vPIN_LOAD_ENABLED
BLYNK_WRITE(vPIN_LOAD_ENABLED)
{
  debugPrint("SET NEW VALUE FOR vPIN_LOAD_ENABLED");
  bool newState = param.asInt() > 0;
  executeFromBlynkWrite(Variable::LOAD_MANUAL_ONOFF, &newState);
  debugPrintln();
}
#endif

#ifdef vPIN_CHARGE_DEVICE_ENABLED
BLYNK_WRITE(vPIN_CHARGE_DEVICE_ENABLED)
{
  debugPrint("SET NEW VALUE FOR vPIN_CHARGE_DEVICE_ENABLED");
  bool newState = param.asInt() > 0;
  executeFromBlynkWrite(Variable::CHARGING_DEVICE_ONOFF, &newState);
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
    Controller::getInstance().getSolarController()->fetchAllValues();
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
    Controller::getInstance().getSolarController()->syncRealtimeClock(getMyNowTm());
    Blynk.virtualWrite(vPIN_UPDATE_CONTROLLER_DATETIME, 0);
  }
}
#endif

#endif

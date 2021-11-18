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

boolean blynkConnect(){
  if(envData.blynkLocalServer){
    Blynk.config(envData.blynkAuth, envData.blynkServerHostname, envData.blynkServerPort);
  }
  else{
    Blynk.config(envData.blynkAuth);
  }
  return Blynk.connect();
}

void blynkSetup()
{
  debugPrintln(" ++ Setting up Blynk:");
  debugPrint("Connecting...");

  while (!blynkConnect())
  {
    debugPrint(".");
    delay(500);
  }
  debugPrintln("OK");
  clearStatusError(STATUS_ERR_NO_BLYNK_CONNECTION);
}

void blynkLoop(){
  if(!Blynk.connected()){
    if(!blynkConnect()){
      setStatusError(STATUS_ERR_NO_BLYNK_CONNECTION);
    }
    else{
      clearStatusError(STATUS_ERR_NO_BLYNK_CONNECTION);
    }
  } 
  
  Blynk.run();
}


bool uploadVariableToBlynk(const mqttSolarVariableMap* varDef){
  if (thisController->isVariableReadReady(varDef->solarVariable))
    {
     if (thisController->isVariableReadReady(varDef->solarVariable))
    {
      switch (SolarTracer::getVariableDatatype(varDef->solarVariable))
      {
      case SolarTracerVariablesDataType::FLOAT:
        Blynk.virtualWrite(varDef->virtualPin, thisController->getFloatValue(varDef->solarVariable));
        break;
      case SolarTracerVariablesDataType::STRING:
        Blynk.virtualWrite(varDef->virtualPin, thisController->getStringValue(varDef->solarVariable));
        break;
      default:
        break;
      }
    }
      return true;
    }
    return false;
}

// upload values stats
void uploadStatsToBlynk()
{
  if(!Blynk.connected()){
    setStatusError(STATUS_ERR_NO_BLYNK_CONNECTION);
    return;
  }
  clearStatusError(STATUS_ERR_NO_BLYNK_CONNECTION);
  bool varNotReady = false;
  for (uint8_t index = 0; index < statVirtualBlynkSolarVariablesCount; index++)
  {
    varNotReady |= ! uploadVariableToBlynk(&statTimeVirtualBlynkSolarVariables[index]);
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
  if(!Blynk.connected()){
    return;
  }

#ifdef vPIN_INTERNAL_STATUS
  Blynk.virtualWrite(vPIN_INTERNAL_STATUS, internalStatus);
#endif

  bool varNotReady = false;
  for (uint8_t index = 0; index < realTimeVirtualBlynkSolarVariablesCount; index++)
  {
    varNotReady |= ! uploadVariableToBlynk(&realTimeVirtualBlynkSolarVariables[index]);
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

#ifdef vPIN_LOAD_ENABLED
BLYNK_WRITE(vPIN_LOAD_ENABLED)
{
  uint8_t newState = (uint8_t)param.asInt();

  debugPrint("Setting load state output coil to value: ");
  debugPrintln(newState);

  if (!thisController->writeBoolValue(SolarTracerVariables::LOAD_MANUAL_ONOFF, newState > 0))
  {
    debugPrintln("Write & Read failed.");
  }
  thisController->fetchValue(SolarTracerVariables::LOAD_MANUAL_ONOFF);
  uploadRealtimeToBlynk();
}
#endif

#ifdef vPIN_CHARGE_DEVICE_ENABLED
BLYNK_WRITE(vPIN_CHARGE_DEVICE_ENABLED)
{
  uint8_t newState = (uint8_t)param.asInt();

  debugPrint("Setting load state output coil to value: ");
  debugPrintln(newState);

  if (thisController->writeBoolValue(SolarTracerVariables::CHARGING_DEVICE_ONOFF, newState > 0))
  {
    debugPrintln("Write & Read failed.");
  }
  thisController->fetchValue(SolarTracerVariables::CHARGING_DEVICE_ONOFF);
  uploadRealtimeToBlynk();
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

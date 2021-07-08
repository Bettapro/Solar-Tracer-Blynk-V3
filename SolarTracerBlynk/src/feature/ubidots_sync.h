#pragma once
#include "../incl/project_config.h"

extern SolarTracer *thisController;

Ubidots *ubidots;

void ubidotsSetup()
{
  ubidots = new Ubidots(UBIDOTS_AUTH, UBI_TCP);

  debugPrintln(" ++ Setting up Ubidots:");
  debugPrint("Connecting...");
}

// upload values stats
void uploadStatsToUbidots()
{
  uint8_t toUpdateCount = 0;
  bool varNotReady = false;
  for (uint8_t index = 0; index < statVirtualBlynkSolarVariablesCount; index++)
  {
    if (thisController->isVariableReadReady(statVirtualBlynkSolarVariables[index].solarVariable))
    {
      switch (SolarTracer::getVariableDatatype(statVirtualBlynkSolarVariables[index].solarVariable))
      {
      case SolarTracerVariablesDataType::FLOAT:
        ubidots->add(statVirtualBlynkSolarVariables[index].virtualPin, thisController->getFloatValue(statVirtualBlynkSolarVariables[index].solarVariable));
        toUpdateCount++;
        break;
      case SolarTracerVariablesDataType::STRING:
        // not supported !!
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
  if (toUpdateCount > 0)
  {
#ifdef UBIDOTS_DEVICE_LABEL
    ubidots->send(UBIDOTS_DEVICE_LABEL);
#else
    ubidots->send();
#endif
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
void uploadRealtimeToUbidots()
{
  uint8_t toUpdateCount = 0;
#ifdef vPIN_INTERNAL_STATUS
  ubidots->add(vPIN_INTERNAL_STATUS, internalStatus);
  toUpdateCount++;
#endif

  bool varNotReady = false;
  for (uint8_t index = 0; index < realTimeVirtualBlynkSolarVariablesCount; index++)
  {
    if (thisController->isVariableReadReady(realTimeVirtualBlynkSolarVariables[index].solarVariable))
    {
      switch (SolarTracer::getVariableDatatype(realTimeVirtualBlynkSolarVariables[index].solarVariable))
      {
      case SolarTracerVariablesDataType::FLOAT:
        ubidots->add(realTimeVirtualBlynkSolarVariables[index].virtualPin, thisController->getFloatValue(realTimeVirtualBlynkSolarVariables[index].solarVariable));
        toUpdateCount++;
        break;
      case SolarTracerVariablesDataType::STRING:
        // not supported
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
  if (toUpdateCount > 0)
  {
#ifdef UBIDOTS_DEVICE_LABEL
    ubidots->send(UBIDOTS_DEVICE_LABEL);
#else
    ubidots->send();
#endif
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

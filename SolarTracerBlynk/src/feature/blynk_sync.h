 #pragma once
 #include "../incl/project_config.h"

 extern SolarTracer* thisController;

 // upload values stats
void uploadStatsToBlynk()
{
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
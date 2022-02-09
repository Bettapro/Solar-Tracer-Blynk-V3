#include "BlynkSync.h"
#include "../core/Environment.h"
#include "../incl/project_core_config.h"

#include "../incl/project_include.h"


// reducing Blynk footprint
#define BLYNK_NO_BUILTIN // Disable built-in analog & digital pin operations
#define BLYNKTIMER_H
#if defined USE_BLYNK
#if defined ESP32
#include <BlynkSimpleEsp32.h>
#elif defined ESP8266
#include <BlynkSimpleEsp8266.h>
#endif
#endif


BlynkSync::BlynkSync()
  {
    for (uint8_t index = 0; index < Variable::VARIABLES_COUNT; index++)
    {
      this->blynkValuesCacheValid[index] = 0;

      switch (VariableDefiner::getInstance().getDatatype((Variable)index))
      {
      case VariableDatatype::DT_FLOAT:
        this->blynkValuesCache[index] = malloc(sizeof(float));
        break;
      case VariableDatatype::DT_STRING:
        this->blynkValuesCache[index] = new char[BLYNK_VALUE_CACHES_STRING_LEN];
        break;
      }
    }
  }

void BlynkSync::setup()
  {
    this->connect();
#ifdef vPIN_INTERNAL_DEBUG_TERMINAL
    debugAddRegisterCallback(blynkDebugCallback);
#endif
    debugPrintln("Solar Tracer START\r\n");
  }

  void BlynkSync::connect()
  {
    debugPrintln(" ++ Setting up Blynk:");
    debugPrint("Connecting...");

#ifdef USE_BLYNK_2
    Blynk.config(Environment::getData()->blynkAuth);
#else
    if (Environment::getData()->blynkLocalServer)
    {
      Blynk.config(Environment::getData()->blynkAuth, Environment::getData()->blynkServerHostname, Environment::getData()->blynkServerPort);
    }
    else
    {
      Blynk.config(Environment::getData()->blynkAuth);
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
  
  void BlynkSync::loop()
  {
    Controller::getInstance().setStatusFlag(STATUS_ERR_NO_BLYNK_CONNECTION, !Blynk.connected());
    Blynk.run();
  }



  bool BlynkSync::sendUpdateToVariable(Variable variable, const void *value)
  {
    void *cachedValue = this->blynkValuesCache[variable];
    uint8_t *cachedUntil = &(this->blynkValuesCacheValid[variable]);

    const VariableDefinition *def = VariableDefiner::getInstance().getDefinition(variable);

    if (def->blynkVPin != nullptr)
    {

      switch (def->datatype)
      {
      case VariableDatatype::DT_FLOAT:
      {
        float currentValue = *(float *)value;
        if (*cachedUntil <= 0 || (*(float *)cachedValue) != currentValue)
        {
          Blynk.virtualWrite(*def->blynkVPin, currentValue);
          (*(float *)cachedValue) = currentValue;
          (*cachedUntil) = BLYNK_VALUE_CACHES_UNTIL_COUNT;
        }
        (*cachedUntil)--;
        return true;
      }
      break;
      case VariableDatatype::DT_STRING:
      {
        const char *currentValue = (const char *)value;
        if (*cachedUntil <= 0 || strcmp(currentValue, (const char *)cachedValue) != 0)
        {
          Blynk.virtualWrite(*def->blynkVPin, currentValue);
          strcpy((char *)cachedValue, currentValue);
          (*cachedUntil) = BLYNK_VALUE_CACHES_UNTIL_COUNT;
        }
        (*cachedUntil)--;
        return true;
      }
      }
      return false;
    }
  }

  // upload values stats
  void BlynkSync::uploadStatsToBlynk()
  {
    if (!Blynk.connected())
    {
      return;
    }

    BlynkSync::getInstance().sendUpdateAllBySource(VariableSource::SR_STATS, false);
  }

  // upload values realtime
  void BlynkSync::uploadRealtimeToBlynk()
  {
    if (!Blynk.connected())
    {
      return;
    }

#ifdef vPIN_INTERNAL_STATUS
    uint8_t status = Controller::getInstance().getStatus();
    BlynkSync::getInstance().sendUpdateToVariable(Variable::INTERNAL_STATUS, &(status));
#endif
    BlynkSync::getInstance().sendUpdateAllBySource(VariableSource::SR_REALTIME, false);
  }

  
BLYNK_WRITE_DEFAULT()
{
  const VariableDefinition *def = VariableDefiner::getInstance().getDefinitionByBlynkVPin(request.pin);
  if (def != nullptr)
  {
    if (def->source == VariableSource::SR_INTERNAL)
    {
      switch (def->variable)
      {
      case Variable::REALTIME_CLOCK:
      {
        if (param.asInt() > 0)
        {
          debugPrintln("UPDATE CONTROLLER DATETIME");
          Controller::getInstance().getSolarController()->syncRealtimeClock(getMyNowTm());
          Blynk.virtualWrite(*def->blynkVPin, 0);
        }
      }
      break;
      case Variable::UPDATE_ALL_CONTROLLER_DATA:
      {
        if (param.asInt() > 0)
        {
          debugPrintln("REQUEST ALL VALUES TO CONTROLLER");
          Controller::getInstance().getSolarController()->fetchAllValues();
          BlynkSync::getInstance().uploadRealtimeToBlynk();
          BlynkSync::getInstance().uploadStatsToBlynk();

          Blynk.virtualWrite(*def->blynkVPin, 0);
        }
      }
      break;
      }
    }
    else
    {
      switch (def->datatype)
      {
      case VariableDatatype::DT_FLOAT:
      {
        float newState = param.asFloat();
        BlynkSync::getInstance().applyUpdateToVariable(def->variable, &newState, false);
      }
      case VariableDatatype::DT_BOOL:
      {
        bool newState = param.asInt() > 0;
        BlynkSync::getInstance().applyUpdateToVariable(def->variable, &newState, false);
      }
      break;
      }
    }
  }
}
#include "BlynkSync.h"

#include "../incl/include_all_core.h"

#ifdef USE_BLYNK

#include "../incl/include_all_lib.h"
#include "../core/Environment.h"
#include "../core/datetime.h"

// reducing Blynk footprint
#define BLYNK_NO_BUILTIN // Disable built-in analog & digital pin operations
#define BLYNKTIMER_H     // no blynk timer - using simple timer
#if defined USE_BLYNK
#if defined ESP32
#include <BlynkSimpleEsp32.h>
#elif defined ESP8266
#include <BlynkSimpleEsp8266.h>
#endif
#endif

#ifdef vPIN_INTERNAL_DEBUG_TERMINAL
void blynkDebugCallback(String message)
{
  Blynk.virtualWrite(vPIN_INTERNAL_DEBUG_TERMINAL, message);
}
#endif

BlynkSync::BlynkSync()
{
  this->renewValueCount = BLYNK_VALUE_CACHES_UNTIL_COUNT;
}

void BlynkSync::setup()
{
  this->connect();
#ifdef vPIN_INTERNAL_DEBUG_TERMINAL
  debugAddRegisterCallback(blynkDebugCallback);
#endif
}

void BlynkSync::connect()
{
  debugPrintf(true, Text::setupWithName, "BLYNK");
  debugPrint(Text::connecting);

#ifdef USE_BLYNK_2
  Blynk.config(Environment::getData()->blynkAuth);
#else

  Blynk.config(
      Environment::getData()->blynkAuth,
      strlen(Environment::getData()->blynkServerHostname) ? Environment::getData()->blynkServerHostname : BLYNK_DEFAULT_DOMAIN,
      Environment::getData()->blynkServerPort ? Environment::getData()->blynkServerPort : BLYNK_DEFAULT_PORT);

#endif

  uint8_t counter = 0;

  while (counter < BLYNK_CONNECT_ATTEMPT && !Blynk.connect())
  {
    delay(500);
    debugPrint(Text::dot);

#ifndef BLYNK_CONNECTION_REQUIRED
    counter++;
#endif
  }

  debugPrintln(Blynk.connected() ? Text::ok : Text::ko);
}

void BlynkSync::loop()
{
  Controller::getInstance().setErrorFlag(STATUS_ERR_NO_BLYNK_CONNECTION, !Blynk.connected());
  Blynk.run();
}

bool BlynkSync::sendUpdateToVariable(const VariableDefinition *def, const void *value)
{
  if (def->blynkVPin != nullptr)
  {
    switch (def->datatype)
    {
    case VariableDatatype::DT_BOOL:
    {
      Blynk.virtualWrite(*def->blynkVPin, *(bool *)value);
      return true;
    }
    break;
    case VariableDatatype::DT_FLOAT:
    {
      Blynk.virtualWrite(*def->blynkVPin, *(float *)value);
      return true;
    }
    break;
    case VariableDatatype::DT_UINT16:
    {
      Blynk.virtualWrite(*def->blynkVPin, *(uint16_t *)value);
      return true;
    }
    break;
    case VariableDatatype::DT_STRING:
    {
      Blynk.virtualWrite(*def->blynkVPin, (const char *)value);
      return true;
    }
    }
  }
  return false;
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
  uint16_t status = Controller::getInstance().getStatus();
  this->sendUpdateToVariable(VariableDefiner::getInstance().getDefinition(Variable::INTERNAL_STATUS), &(status));
#endif
  this->sendUpdateAllBySource(VariableSource::SR_REALTIME, false);
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
          if (Datetime::getMyNowTm() != nullptr)
          {
            Controller::getInstance().getSolarController()->syncRealtimeClock(Datetime::getMyNowTm());
          }
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
    else if(Controller::getInstance().getSolarController()->isVariableEnabled(def->variable))
    {
      switch (def->datatype)
      {
      case VariableDatatype::DT_UINT16:
      {
        uint16_t newState = param.asInt();
        BlynkSync::getInstance().applyUpdateToVariable(def->variable, &newState, false);
      }
      break;
      case VariableDatatype::DT_FLOAT:
      {
        float newState = param.asFloat();
        BlynkSync::getInstance().applyUpdateToVariable(def->variable, &newState, false);
      }
      break;
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

#endif
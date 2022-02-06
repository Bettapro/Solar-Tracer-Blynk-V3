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


#ifndef SolarTracer_h
#define SolarTracer_h

#include "../core/VariableDefiner.h"

struct SolarTracerVariableDefinition
{
  float floatValue;
  char *textValue;
  /**
   *  byte 0 - enabled
   *  byte 1 - ready
   */  
  uint8_t status;
};

 

class SolarTracer
{
public:
  SolarTracer()
  {
    this->variableDefine = new SolarTracerVariableDefinition[Variable::VARIABLES_COUNT]();
    const VariableDefinition* def ;
    for(uint8_t varIndex = 0; varIndex < Variable::VARIABLES_COUNT; varIndex++){
      def = VariableDefiner::getInstance().getDefinition((Variable)varIndex);
      if(def->source == VariableSource::SR_STATS || def->source == VariableSource::SR_REALTIME){
        this->variableDefine[varIndex] = {0, def->datatype == VariableDatatype::DT_STRING ? new char[20] : nullptr, 0};
      }
    }
 }


  void setVariableEnabled(Variable variable)
  {
    this->setVariableEnable(variable, true);
  }
  
  void setVariableEnable(Variable variable, bool enable)
  {
    if (variable < Variable::VARIABLES_COUNT && this->isVariableEnabled(variable) != enable)
    {
      this->variableDefine[variable].status += (enable ? 1 : -1);
    }
  }

  bool isVariableEnabled(Variable variable)
  {
    if (variable < Variable::VARIABLES_COUNT)
    {
      return this->variableDefine[variable].status & 1 > 0;
    }
    return false;
  }

  void setVariableReadReady(Variable variable, bool enable)
  {
    if (variable < Variable::VARIABLES_COUNT && this->isVariableReadReady(variable) != enable)
    {
      this->variableDefine[variable].status += (enable ? 1 : -1) * 2;
    }
  }

  void setVariableReadReady(uint8_t count, bool ready, ...)
  {
    va_list args;
    va_start(args, ready);

    for (uint8_t i = 1; i <= count; i++)
    {
      this->setVariableReadReady((Variable)va_arg(args, int), ready);
    }

    va_end(args);
  }

  bool isVariableReadReady(Variable variable)
  {
    if (variable < Variable::VARIABLES_COUNT)
    {
      return this->variableDefine[variable].status & 2 > 0;
    }
    return false;
  }

  const char *getStringValue(Variable variable)
  {
    if (!this->isVariableEnabled(variable))
    {
      return "";
    }

    if (VariableDefiner::getInstance().getDatatype(variable) == VariableDatatype::DT_STRING)
    {
      return this->variableDefine[variable].textValue;
    }

    // should raise an exception?
    return "";
  }

  float getFloatValue(Variable variable)
  {
    if (!this->isVariableEnabled(variable))
    {
      return 0;
    }

    if (VariableDefiner::getInstance().getDatatype(variable) == VariableDatatype::DT_FLOAT)
    {
      return this->variableDefine[variable].floatValue;
    }

    // should raise an exception?
    return 0.0;
  }

  /**
   * Return last status code, 0 means the controller is responding to requests correctly.
   */
  const uint16_t getLastControllerCommunicationStatus()
  {
    return this->lastControllerCommunicationStatus;
  }

  virtual bool fetchValue(Variable variable) = 0;
  virtual bool syncRealtimeClock(struct tm *ti) = 0;
  virtual void fetchAllValues() = 0;
  virtual bool updateRun() = 0;
  virtual bool writeValue(Variable variable, void *value) = 0;
  virtual bool testConnection() = 0;

protected:
  bool setFloatVariable(Variable variable, float value)
  {
    return this->setVariableValue(variable, &value);
  }

  bool setVariableValue(Variable variable, const void *value)
  {
    bool valueOk = value != nullptr;
    if (valueOk)
    {
      switch (VariableDefiner::getInstance().getDatatype(variable))
      {
      case VariableDatatype::DT_FLOAT:
      {
        this->variableDefine[variable].floatValue = *(float *)value;
      }
        break;
      case VariableDatatype::DT_STRING:
        // should specialize depending on the text
        strcpy(this->variableDefine[variable].textValue, (const char *)value);
        break;
      }
    }
    this->setVariableReadReady(variable, valueOk);
 
    return valueOk;
  }

  uint16_t lastControllerCommunicationStatus;

private:
  SolarTracerVariableDefinition *variableDefine;
};

#endif

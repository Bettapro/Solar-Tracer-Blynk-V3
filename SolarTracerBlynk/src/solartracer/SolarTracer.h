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

typedef enum
{
  UNDEFINED,
  STRING,
  FLOAT
} SolarTracerVariablesDataType;

typedef enum
{
  PV_VOLTAGE,
  PV_POWER,
  PV_CURRENT,
  LOAD_CURRENT,
  LOAD_POWER,
  BATTERY_TEMP,
  BATTERY_VOLTAGE,
  BATTERY_SOC,
  CONTROLLER_TEMP,
  BATTERY_CHARGE_CURRENT,
  BATTERY_CHARGE_POWER,
  BATTERY_OVERALL_CURRENT,
  REALTIME_CLOCK,
  LOAD_FORCE_ONOFF,
  LOAD_MANUAL_ONOFF,
  REMOTE_BATTERY_TEMP,
  GENERATED_ENERGY_TODAY,
  GENERATED_ENERGY_MONTH,
  GENERATED_ENERGY_YEAR,
  GENERATED_ENERGY_TOTAL,
  MAXIMUM_PV_VOLTAGE_TODAY,
  MINIMUM_PV_VOLTAGE_TODAY,
  MAXIMUM_BATTERY_VOLTAGE_TODAY,
  MINIMUM_BATTERY_VOLTAGE_TODAY,
  BATTERY_BOOST_VOLTAGE,
  BATTERY_EQUALIZATION_VOLTAGE,
  BATTERY_FLOAT_VOLTAGE,
  BATTERY_FLOAT_MIN_VOLTAGE,
  BATTERY_CHARGING_LIMIT_VOLTAGE,
  BATTERY_DISCHARGING_LIMIT_VOLTAGE,
  BATTERY_LOW_VOLTAGE_DISCONNECT,
  BATTERY_LOW_VOLTAGE_RECONNECT,
  BATTERY_OVER_VOLTAGE_DISCONNECT,
  BATTERY_OVER_VOLTAGE_CONNECT,
  BATTERY_UNDER_VOLTAGE_SET,
  BATTERY_UNDER_VOLTAGE_RESET,
  BATTERY_STATUS_TEXT,
  CHARGING_EQUIPMENT_STATUS_TEXT,
  DISCHARGING_EQUIPMENT_STATUS_TEXT,
  CHARGING_DEVICE_ONOFF,
  HEATSINK_TEMP,

  //----------------
  VARIABLES_COUNT
} SolarTracerVariables;

class SolarTracer
{
public:
  SolarTracer()
  {
    this->variableEnables = new bool[SolarTracerVariables::VARIABLES_COUNT + 1]();
    this->variableReadReady = new bool[SolarTracerVariables::VARIABLES_COUNT + 1]();
    this->charVars = new char*[SolarTracerVariables::VARIABLES_COUNT + 1]();
    this->floatVars = new float[SolarTracerVariables::VARIABLES_COUNT + 1]();

    // make sure all variables are disabled and not read ready
    for (uint16_t i = 0; i <= SolarTracerVariables::VARIABLES_COUNT; i++)
    {
      this->variableEnables[i] = this->variableReadReady[i] = false;
      switch(this->getVariableDatatype((SolarTracerVariables)i)){
        case SolarTracerVariablesDataType::FLOAT:
          // nothing to do
          break;
        case SolarTracerVariablesDataType::STRING:
          // should specialize depending on the text
          this->charVars[i] = new char[20];
          break;
      }
    }
  }

  static SolarTracerVariablesDataType getVariableDatatype(SolarTracerVariables variable)
  {
    switch (variable)
    {
    case SolarTracerVariables::PV_VOLTAGE:
    case SolarTracerVariables::PV_POWER:
    case SolarTracerVariables::PV_CURRENT:
    case SolarTracerVariables::LOAD_CURRENT:
    case SolarTracerVariables::LOAD_POWER:
    case SolarTracerVariables::BATTERY_TEMP:
    case SolarTracerVariables::BATTERY_VOLTAGE:
    case SolarTracerVariables::BATTERY_SOC:
    case SolarTracerVariables::CONTROLLER_TEMP:
    case SolarTracerVariables::BATTERY_CHARGE_CURRENT:
    case SolarTracerVariables::BATTERY_CHARGE_POWER:
    case SolarTracerVariables::BATTERY_OVERALL_CURRENT:
    case SolarTracerVariables::LOAD_MANUAL_ONOFF:
    case SolarTracerVariables::CHARGING_DEVICE_ONOFF:
    case SolarTracerVariables::GENERATED_ENERGY_TODAY:
    case SolarTracerVariables::GENERATED_ENERGY_MONTH:
    case SolarTracerVariables::GENERATED_ENERGY_YEAR:
    case SolarTracerVariables::GENERATED_ENERGY_TOTAL:
    case SolarTracerVariables::MAXIMUM_PV_VOLTAGE_TODAY:
    case SolarTracerVariables::MINIMUM_PV_VOLTAGE_TODAY:
    case SolarTracerVariables::MAXIMUM_BATTERY_VOLTAGE_TODAY:
    case SolarTracerVariables::MINIMUM_BATTERY_VOLTAGE_TODAY:
    case SolarTracerVariables::REMOTE_BATTERY_TEMP:
    case SolarTracerVariables::HEATSINK_TEMP:
      return SolarTracerVariablesDataType::FLOAT;
    case SolarTracerVariables::BATTERY_STATUS_TEXT:
    case SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT:
    case SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT:
      return SolarTracerVariablesDataType::STRING;
    default:
      return SolarTracerVariablesDataType::UNDEFINED;
    }
  }

  void setVariableEnabled(SolarTracerVariables variable)
  {
    this->setVariableEnable(variable, true);
  }
  void setVariableEnable(SolarTracerVariables variable, bool enable)
  {
    if (variable < SolarTracerVariables::VARIABLES_COUNT)
    {
      this->variableEnables[variable] = enable;
    }
  }

  bool isVariableEnabled(SolarTracerVariables variable)
  {
    if (variable < SolarTracerVariables::VARIABLES_COUNT)
    {
      return this->variableEnables[variable];
    }
    return false;
  }

  void setVariableReadReady(SolarTracerVariables variable, bool enable)
  {
    if (variable < SolarTracerVariables::VARIABLES_COUNT)
    {
      this->variableReadReady[variable] = enable;
    }
  }

  void setVariableReadReady(uint8_t count, bool ready, ...)
  {
    va_list args;
    va_start(args, ready);

    for (uint8_t i = 1; i <= count; i++)
    {
      this->setVariableReadReady((SolarTracerVariables)va_arg(args, int), ready);
    }

    va_end(args);
  }

  bool isVariableReadReady(SolarTracerVariables variable)
  {
    if (variable < SolarTracerVariables::VARIABLES_COUNT)
    {
      return this->variableReadReady[variable];
    }
    return false;
  }

  const char *getStringValue(SolarTracerVariables variable)
  {
    if (!this->isVariableEnabled(variable))
    {
      return "";
    }

    if(this->getVariableDatatype(variable) == SolarTracerVariablesDataType::FLOAT){
      return charVars[variable];
    }

    // should raise an exception?
    return "";
  }

  float getFloatValue(SolarTracerVariables variable)
  {
    if (!this->isVariableEnabled(variable))
    {
      return 0;
    }

    if(this->getVariableDatatype(variable) == SolarTracerVariablesDataType::FLOAT){
      return floatVars[variable];
    }

    // should raise an exception?
    return 0.0;
  }
  virtual bool fetchValue(SolarTracerVariables variable) = 0;
  virtual bool syncRealtimeClock(struct tm *ti) = 0;
  virtual void fetchAllValues() = 0;
  virtual bool updateRun() = 0;
  virtual bool writeBoolValue(SolarTracerVariables variable, bool value) = 0;

protected:
  float* floatVars;
  char** charVars;
private:
  bool *variableEnables;
  bool *variableReadReady;
};

#endif

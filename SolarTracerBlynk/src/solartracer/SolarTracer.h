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
  DT_UNDEFINED,
  DT_STRING,
  DT_FLOAT
} SolarTracerVariablesDataType;

typedef enum
{
  UOM_UNDEFINED,
  UOM_STATUS,
  UOM_PERCENT,
  UOM_VOLT,
  UOM_WATT,
  UOM_KILOWATTHOUR,
  UOM_AMPERE,
  UOM_TEMPERATURE_C
} SolarTracerVariablesUOM;

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
  BATTERY_OVER_VOLTAGE_RECONNECT,
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

struct SolarTracerVariableDefinition
{
  float floatValue;
  char *textValue;
  /**
   *  byte 0 - enabled
   *  byte 1 - ready
   */  
  uint8_t status;
  SolarTracerVariablesDataType datatype;
  SolarTracerVariablesUOM uom;
};

class SolarTracer
{
public:
  SolarTracer()
  {
    this->variableDefine = new SolarTracerVariableDefinition[SolarTracerVariables::VARIABLES_COUNT]();

    this->initVariableDefine(SolarTracerVariables::PV_VOLTAGE, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::PV_POWER, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_WATT);
    this->initVariableDefine(SolarTracerVariables::PV_CURRENT, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_AMPERE);
    this->initVariableDefine(SolarTracerVariables::LOAD_CURRENT, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_AMPERE);
    this->initVariableDefine(SolarTracerVariables::LOAD_POWER, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_WATT);
    this->initVariableDefine(SolarTracerVariables::BATTERY_TEMP, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_TEMPERATURE_C);
    this->initVariableDefine(SolarTracerVariables::BATTERY_VOLTAGE, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::BATTERY_SOC, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_PERCENT);
    this->initVariableDefine(SolarTracerVariables::CONTROLLER_TEMP, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_TEMPERATURE_C);
    this->initVariableDefine(SolarTracerVariables::BATTERY_CHARGE_CURRENT, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_AMPERE);
    this->initVariableDefine(SolarTracerVariables::BATTERY_CHARGE_POWER, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_WATT);
    this->initVariableDefine(SolarTracerVariables::BATTERY_OVERALL_CURRENT, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_AMPERE);
    this->initVariableDefine(SolarTracerVariables::REALTIME_CLOCK, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_UNDEFINED);
    this->initVariableDefine(SolarTracerVariables::LOAD_FORCE_ONOFF, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_UNDEFINED);
    this->initVariableDefine(SolarTracerVariables::LOAD_MANUAL_ONOFF, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_UNDEFINED);
    this->initVariableDefine(SolarTracerVariables::REMOTE_BATTERY_TEMP, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_TEMPERATURE_C);
    this->initVariableDefine(SolarTracerVariables::GENERATED_ENERGY_TODAY, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_KILOWATTHOUR);
    this->initVariableDefine(SolarTracerVariables::GENERATED_ENERGY_MONTH, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_KILOWATTHOUR);
    this->initVariableDefine(SolarTracerVariables::GENERATED_ENERGY_YEAR, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_KILOWATTHOUR);
    this->initVariableDefine(SolarTracerVariables::GENERATED_ENERGY_TOTAL, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_KILOWATTHOUR);
    this->initVariableDefine(SolarTracerVariables::MAXIMUM_PV_VOLTAGE_TODAY, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::MINIMUM_PV_VOLTAGE_TODAY, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::MAXIMUM_BATTERY_VOLTAGE_TODAY, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::MINIMUM_BATTERY_VOLTAGE_TODAY, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::BATTERY_BOOST_VOLTAGE, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::BATTERY_EQUALIZATION_VOLTAGE, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::BATTERY_FLOAT_VOLTAGE, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::BATTERY_FLOAT_MIN_VOLTAGE, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::BATTERY_CHARGING_LIMIT_VOLTAGE, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::BATTERY_DISCHARGING_LIMIT_VOLTAGE, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::BATTERY_LOW_VOLTAGE_DISCONNECT, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::BATTERY_LOW_VOLTAGE_RECONNECT, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::BATTERY_OVER_VOLTAGE_DISCONNECT, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::BATTERY_OVER_VOLTAGE_RECONNECT, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::BATTERY_UNDER_VOLTAGE_SET, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::BATTERY_UNDER_VOLTAGE_RESET, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_VOLT);
    this->initVariableDefine(SolarTracerVariables::BATTERY_STATUS_TEXT, SolarTracerVariablesDataType::DT_STRING, SolarTracerVariablesUOM::UOM_UNDEFINED);
    this->initVariableDefine(SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT, SolarTracerVariablesDataType::DT_STRING, SolarTracerVariablesUOM::UOM_UNDEFINED);
    this->initVariableDefine(SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT, SolarTracerVariablesDataType::DT_STRING, SolarTracerVariablesUOM::UOM_UNDEFINED);
    this->initVariableDefine(SolarTracerVariables::CHARGING_DEVICE_ONOFF, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_UNDEFINED);
    this->initVariableDefine(SolarTracerVariables::HEATSINK_TEMP, SolarTracerVariablesDataType::DT_FLOAT, SolarTracerVariablesUOM::UOM_TEMPERATURE_C);
  }

  SolarTracerVariablesDataType getVariableDatatype(SolarTracerVariables variable)
  {
    if (variable < SolarTracerVariables::VARIABLES_COUNT)
    {
      return this->variableDefine[variable].datatype;
    }
    return SolarTracerVariablesDataType::DT_UNDEFINED;
  }

  SolarTracerVariablesUOM getVariableUOM(SolarTracerVariables variable)
  {
    if (variable < SolarTracerVariables::VARIABLES_COUNT)
    {
      return this->variableDefine[variable].uom;
    }
    return SolarTracerVariablesUOM::UOM_UNDEFINED;
  }

  void setVariableEnabled(SolarTracerVariables variable)
  {
    this->setVariableEnable(variable, true);
  }
  void setVariableEnable(SolarTracerVariables variable, bool enable)
  {
    if (variable < SolarTracerVariables::VARIABLES_COUNT && this->isVariableEnabled(variable) != enable)
    {
      this->variableDefine[variable].status += (enable ? 1 : -1);
    }
  }

  bool isVariableEnabled(SolarTracerVariables variable)
  {
    if (variable < SolarTracerVariables::VARIABLES_COUNT)
    {
      return this->variableDefine[variable].status & 1 > 0;
    }
    return false;
  }

  void setVariableReadReady(SolarTracerVariables variable, bool enable)
  {
    if (variable < SolarTracerVariables::VARIABLES_COUNT && this->isVariableReadReady(variable) != enable)
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
      this->setVariableReadReady((SolarTracerVariables)va_arg(args, int), ready);
    }

    va_end(args);
  }

  bool isVariableReadReady(SolarTracerVariables variable)
  {
    if (variable < SolarTracerVariables::VARIABLES_COUNT)
    {
      return this->variableDefine[variable].status & 2 > 0;
    }
    return false;
  }

  const char *getStringValue(SolarTracerVariables variable)
  {
    if (!this->isVariableEnabled(variable))
    {
      return "";
    }

    if (this->getVariableDatatype(variable) == SolarTracerVariablesDataType::DT_STRING)
    {
      return this->variableDefine[variable].textValue;
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

    if (this->getVariableDatatype(variable) == SolarTracerVariablesDataType::DT_FLOAT)
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

  virtual bool fetchValue(SolarTracerVariables variable) = 0;
  virtual bool syncRealtimeClock(struct tm *ti) = 0;
  virtual void fetchAllValues() = 0;
  virtual bool updateRun() = 0;
  virtual bool writeValue(SolarTracerVariables variable, float *value) = 0;
  virtual bool writeValue(SolarTracerVariables variable, bool *value) = 0;
  virtual bool testConnection() = 0;

protected:
  bool setFloatVariable(SolarTracerVariables variable, float value)
  {
    if (this->getVariableDatatype(variable) == SolarTracerVariablesDataType::DT_FLOAT)
    {
      this->variableDefine[variable].floatValue = value;
      this->setVariableReadReady(variable, true);
      return true;
    }
    return false;
  }

  bool setVariableValue(SolarTracerVariables variable, const void *value)
  {
    bool valueOk = value != nullptr;
    if (valueOk)
    {
      switch (this->getVariableDatatype(variable))
      {
      case SolarTracerVariablesDataType::DT_FLOAT:
        this->variableDefine[variable].floatValue = *(float *)value;
        break;
      case SolarTracerVariablesDataType::DT_STRING:
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
  void initVariableDefine(SolarTracerVariables variable, SolarTracerVariablesDataType datatype, SolarTracerVariablesUOM uom)
  {
    this->variableDefine[variable] = {
        0,
        datatype == SolarTracerVariablesDataType::DT_STRING ? new char[20] : nullptr,
        0, // not enabled, not ready
        datatype,
        uom};
  }

  SolarTracerVariableDefinition *variableDefine;
};

#endif

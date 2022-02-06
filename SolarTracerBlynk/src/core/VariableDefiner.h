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

#ifndef VariableDefiner_h
#define VariableDefiner_h

//#include "../incl/project_core_config.h"


typedef enum
{
  SR_REALTIME,
  SR_STATS,
  SR_INTERNAL
} VariableSource;

typedef enum
{
  DT_UNDEFINED,
  DT_STRING,
  DT_FLOAT
} VariableDatatype;

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
} VariableUOM;

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
  INTERNAL_STATUS,
  //----------------
  VARIABLES_COUNT
} Variable;

struct VariableDefinition
{
  Variable variable;
  const char* text;
  VariableDatatype datatype;
  VariableUOM uom;
  VariableSource source;
  const uint8_t *blynkVPin;
  const char *mqttTopic;
};

class VariableDefiner
{
public:
    static VariableDefiner &getInstance(){
         static VariableDefiner definer;
        return definer;
    }

    
    const VariableDefinition* getDefinition(Variable variable){
        return &(variables[variable]);
    }

    const VariableDefinition* getDefinitionByBlynkVPin(uint8_t pin){
         for (uint8_t index = 0; index < Variable::VARIABLES_COUNT; index ++){
             if(*(variables[index].blynkVPin) == pin){
                 return &(variables[index]);
             }
         }
         return nullptr;
    }

    VariableDatatype getDatatype(Variable variable){
        return variables[variable].datatype;
    }


  VariableDefinition *variables = new VariableDefinition[Variable::VARIABLES_COUNT + 1]();
private: 


  VariableDefiner()
  {
    variables = new VariableDefinition[Variable::VARIABLES_COUNT + 1]();

    this->initializeVariable(Variable::PV_VOLTAGE, "PV voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::PV_POWER, "PV power", VariableDatatype::DT_FLOAT, VariableUOM::UOM_WATT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::PV_CURRENT, "PV current", VariableDatatype::DT_FLOAT, VariableUOM::UOM_AMPERE, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::LOAD_CURRENT, "Load current",VariableDatatype::DT_FLOAT, VariableUOM::UOM_AMPERE, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::LOAD_POWER, "Load power", VariableDatatype::DT_FLOAT, VariableUOM::UOM_WATT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_TEMP, "Battery temp", VariableDatatype::DT_FLOAT, VariableUOM::UOM_TEMPERATURE_C, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_VOLTAGE, "Battery voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_SOC, "Battery SOC", VariableDatatype::DT_FLOAT, VariableUOM::UOM_PERCENT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::CONTROLLER_TEMP, "Controller temperature", VariableDatatype::DT_FLOAT, VariableUOM::UOM_TEMPERATURE_C, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_CHARGE_CURRENT, "Charging current", VariableDatatype::DT_FLOAT, VariableUOM::UOM_AMPERE, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_CHARGE_POWER, "Charging power", VariableDatatype::DT_FLOAT, VariableUOM::UOM_WATT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_OVERALL_CURRENT, "Overall current", VariableDatatype::DT_FLOAT, VariableUOM::UOM_AMPERE, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::REALTIME_CLOCK, "Date and time", VariableDatatype::DT_FLOAT, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::LOAD_FORCE_ONOFF, "Load force switch", VariableDatatype::DT_FLOAT, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::LOAD_MANUAL_ONOFF, "Load switch", VariableDatatype::DT_FLOAT, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::REMOTE_BATTERY_TEMP, "Remote battery temperature", VariableDatatype::DT_FLOAT, VariableUOM::UOM_TEMPERATURE_C, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::GENERATED_ENERGY_TODAY, "Energy generated today", VariableDatatype::DT_FLOAT, VariableUOM::UOM_KILOWATTHOUR, VariableSource::SR_STATS);
    this->initializeVariable(Variable::GENERATED_ENERGY_MONTH, "Energy generated month", VariableDatatype::DT_FLOAT, VariableUOM::UOM_KILOWATTHOUR, VariableSource::SR_STATS);
    this->initializeVariable(Variable::GENERATED_ENERGY_YEAR, "Energy generated year", VariableDatatype::DT_FLOAT, VariableUOM::UOM_KILOWATTHOUR, VariableSource::SR_STATS);
    this->initializeVariable(Variable::GENERATED_ENERGY_TOTAL, "Energy generated", VariableDatatype::DT_FLOAT, VariableUOM::UOM_KILOWATTHOUR, VariableSource::SR_STATS);
    this->initializeVariable(Variable::MAXIMUM_PV_VOLTAGE_TODAY, "Today PV min. voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS);
    this->initializeVariable(Variable::MINIMUM_PV_VOLTAGE_TODAY, "Today PV min. voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS);
    this->initializeVariable(Variable::MAXIMUM_BATTERY_VOLTAGE_TODAY, "Today battery max. voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS);
    this->initializeVariable(Variable::MINIMUM_BATTERY_VOLTAGE_TODAY, "Today battery min. voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS);
    this->initializeVariable(Variable::BATTERY_BOOST_VOLTAGE, "Boost voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_EQUALIZATION_VOLTAGE, "Equalization voltage",  VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_FLOAT_VOLTAGE, "Float voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_FLOAT_MIN_VOLTAGE, "Float min. voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_CHARGING_LIMIT_VOLTAGE, "Charging voltage limit", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_DISCHARGING_LIMIT_VOLTAGE, "Discharging voltage limit", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_LOW_VOLTAGE_DISCONNECT, "Low voltage disconnect", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_LOW_VOLTAGE_RECONNECT, "Low voltage reconnect", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_OVER_VOLTAGE_DISCONNECT, "Over voltage disconnect", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_OVER_VOLTAGE_RECONNECT, "Over voltage reconnect",  VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_UNDER_VOLTAGE_SET, "Under voltage alarm set",  VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_UNDER_VOLTAGE_RESET, "Under voltage alarm reset", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::BATTERY_STATUS_TEXT, "Battery status", VariableDatatype::DT_STRING, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::CHARGING_EQUIPMENT_STATUS_TEXT, "Charging status", VariableDatatype::DT_STRING, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "Discharging status", VariableDatatype::DT_STRING, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::CHARGING_DEVICE_ONOFF, "Charging switch", VariableDatatype::DT_FLOAT, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::HEATSINK_TEMP, "Heatsink temperature", VariableDatatype::DT_FLOAT, VariableUOM::UOM_TEMPERATURE_C, VariableSource::SR_REALTIME);
    this->initializeVariable(Variable::INTERNAL_STATUS, "Internal status", VariableDatatype::DT_FLOAT, VariableUOM::UOM_UNDEFINED, VariableSource::SR_INTERNAL);

/**
   * 
   *  BLYNK
   * 
   */
#if defined vPIN_PV_POWER
    variables[Variable::PV_POWER].blynkVPin = new uint8_t(vPIN_PV_POWER);
#endif
#ifdef vPIN_PV_CURRENT
    variables[Variable::PV_CURRENT].blynkVPin = new uint8_t(vPIN_PV_CURRENT);
#endif
#ifdef vPIN_PV_VOLTAGE
    variables[Variable::PV_VOLTAGE].blynkVPin = new uint8_t(vPIN_PV_VOLTAGE);
#endif
#ifdef vPIN_LOAD_CURRENT
    variables[Variable::LOAD_CURRENT].blynkVPin = new uint8_t(vPIN_LOAD_CURRENT);
#endif
#ifdef vPIN_LOAD_POWER
    variables[Variable::LOAD_POWER].blynkVPin = new uint8_t(vPIN_LOAD_POWER);
#endif
#ifdef vPIN_BATT_TEMP
    variables[Variable::BATTERY_TEMP].blynkVPin = new uint8_t(vPIN_BATT_TEMP);
#endif
#ifdef vPIN_BATT_VOLTAGE
    variables[Variable::BATTERY_VOLTAGE].blynkVPin = new uint8_t(vPIN_BATT_VOLTAGE);
#endif
#ifdef vPIN_BATT_REMAIN
    variables[Variable::BATTERY_SOC].blynkVPin = new uint8_t(vPIN_BATT_REMAIN);
#endif
#ifdef vPIN_CONTROLLER_TEMP
    variables[Variable::CONTROLLER_TEMP].blynkVPin = new uint8_t(vPIN_CONTROLLER_TEMP);
#endif
#ifdef vPIN_BATTERY_CHARGE_CURRENT
    variables[Variable::BATTERY_CHARGE_CURRENT].blynkVPin = new uint8_t(vPIN_BATTERY_CHARGE_CURRENT);
#endif
#ifdef vPIN_BATTERY_CHARGE_POWER
    variables[Variable::BATTERY_CHARGE_POWER].blynkVPin = new uint8_t(vPIN_BATTERY_CHARGE_POWER);
#endif
#ifdef vPIN_BATTERY_OVERALL_CURRENT
    variables[Variable::BATTERY_OVERALL_CURRENT].blynkVPin = new uint8_t(vPIN_BATTERY_OVERALL_CURRENT);
#endif
#ifdef vPIN_LOAD_ENABLED
    variables[Variable::LOAD_MANUAL_ONOFF].blynkVPin = new uint8_t(vPIN_LOAD_ENABLED);
#endif
#ifdef vPIN_CHARGE_DEVICE_ENABLED
    variables[Variable::CHARGING_DEVICE_ONOFF].blynkVPin = new uint8_t(vPIN_CHARGE_DEVICE_ENABLED);
#endif
#ifdef vPIN_BATTERY_STATUS_TEXT
    variables[Variable::BATTERY_STATUS_TEXT].blynkVPin = new uint8_t(vPIN_BATTERY_STATUS_TEXT);
#endif
#ifdef vPIN_CHARGING_EQUIPMENT_STATUS_TEXT
    variables[Variable::CHARGING_EQUIPMENT_STATUS_TEXT].blynkVPin = new uint8_t(vPIN_CHARGING_EQUIPMENT_STATUS_TEXT);
#endif
#ifdef vPIN_DISCHARGING_EQUIPMENT_STATUS_TEXT
    variables[Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT].blynkVPin = new uint8_t(vPIN_DISCHARGING_EQUIPMENT_STATUS_TEXT);
#endif
#ifdef vPIN_CONTROLLER_HEATSINK_TEMP
    variables[Variable::HEATSINK_TEMP].blynkVPin = new uint8_t(vPIN_CONTROLLER_HEATSINK_TEMP);
#endif
#ifdef vPIN_STAT_ENERGY_GENERATED_TODAY
    variables[Variable::GENERATED_ENERGY_TODAY].blynkVPin = new uint8_t(vPIN_STAT_ENERGY_GENERATED_TODAY);
#endif
#ifdef vPIN_STAT_ENERGY_GENERATED_THIS_MONTH
    variables[Variable::GENERATED_ENERGY_MONTH].blynkVPin = new uint8_t(vPIN_STAT_ENERGY_GENERATED_THIS_MONTH);
#endif
#ifdef vPIN_STAT_ENERGY_GENERATED_THIS_YEAR
    variables[Variable::GENERATED_ENERGY_YEAR].blynkVPin = new uint8_t(vPIN_STAT_ENERGY_GENERATED_THIS_YEAR);
#endif
#ifdef vPIN_STAT_ENERGY_GENERATED_TOTAL
    variables[Variable::GENERATED_ENERGY_TOTAL].blynkVPin = new uint8_t(vPIN_STAT_ENERGY_GENERATED_TOTAL);
#endif
#ifdef vPIN_MIN_BATTERY_VOLTAGE_TODAY
    variables[Variable::MINIMUM_BATTERY_VOLTAGE_TODAY].blynkVPin = new uint8_t(vPIN_MIN_BATTERY_VOLTAGE_TODAY);
#endif
#ifdef vPIN_MAX_BATTERY_VOLTAGE_TODAY
    variables[Variable::MAXIMUM_BATTERY_VOLTAGE_TODAY].blynkVPin = new uint8_t(vPIN_MAX_BATTERY_VOLTAGE_TODAY);
#endif
#ifdef vPIN_MIN_PV_VOLTAGE_TODAY
    variables[Variable::MINIMUM_PV_VOLTAGE_TODAY].blynkVPin = new uint8_t(vPIN_MIN_PV_VOLTAGE_TODAY);
#endif
#ifdef vPIN_MAX_PV_VOLTAGE_TODAY
    variables[Variable::MAXIMUM_PV_VOLTAGE_TODAY].blynkVPin = new uint8_t(vPIN_MAX_PV_VOLTAGE_TODAY);
#endif
#ifdef vPIN_BATTERY_BOOST_VOLTAGE
    variables[Variable::BATTERY_BOOST_VOLTAGE].blynkVPin = new uint8_t(vPIN_BATTERY_BOOST_VOLTAGE);
#endif
#ifdef vPIN_BATTERY_EQUALIZATION_VOLTAGE
    variables[Variable::BATTERY_EQUALIZATION_VOLTAGE].blynkVPin = new uint8_t(vPIN_BATTERY_EQUALIZATION_VOLTAGE);
#endif
#ifdef vPIN_BATTERY_FLOAT_VOLTAGE
    variables[Variable::BATTERY_FLOAT_VOLTAGE].blynkVPin = new uint8_t(vPIN_BATTERY_FLOAT_VOLTAGE);
#endif
#ifdef vPIN_BATTERY_FLOAT_MIN_VOLTAGE
    variables[Variable::BATTERY_FLOAT_MIN_VOLTAGE].blynkVPin = new uint8_t(vPIN_BATTERY_FLOAT_MIN_VOLTAGE);
#endif
#ifdef vPIN_BATTERY_CHARGING_LIMIT_VOLTAGE
    variables[Variable::BATTERY_CHARGING_LIMIT_VOLTAGE].blynkVPin = new uint8_t(vPIN_BATTERY_CHARGING_LIMIT_VOLTAGE);
#endif
#ifdef vPIN_BATTERY_DISCHARGING_LIMIT_VOLTAGE
    variables[Variable::BATTERY_DISCHARGING_LIMIT_VOLTAGE].blynkVPin = new uint8_t(vPIN_BATTERY_DISCHARGING_LIMIT_VOLTAGE);
#endif
#ifdef vPIN_BATTERY_LOW_VOLTAGE_DISCONNECT
    variables[Variable::BATTERY_LOW_VOLTAGE_DISCONNECT].blynkVPin = new uint8_t(vPIN_BATTERY_LOW_VOLTAGE_DISCONNECT);
#endif
#ifdef vPIN_BATTERY_LOW_VOLTAGE_RECONNECT
    variables[Variable::BATTERY_LOW_VOLTAGE_RECONNECT].blynkVPin = new uint8_t(vPIN_BATTERY_LOW_VOLTAGE_RECONNECT);
#endif
#ifdef vPIN_BATTERY_OVER_VOLTAGE_DISCONNECT
    variables[Variable::BATTERY_OVER_VOLTAGE_DISCONNECT].blynkVPin = new uint8_t(vPIN_BATTERY_OVER_VOLTAGE_DISCONNECT);
#endif
#ifdef vPIN_BATTERY_OVER_VOLTAGE_RECONNECT
    variables[Variable::BATTERY_OVER_VOLTAGE_RECONNECT].blynkVPin = new uint8_t(vPIN_BATTERY_OVER_VOLTAGE_RECONNECT);
#endif
#ifdef vPIN_BATTERY_UNDER_VOLTAGE_RESET
    variables[Variable::BATTERY_UNDER_VOLTAGE_RESET].blynkVPin = new uint8_t(vPIN_BATTERY_UNDER_VOLTAGE_RESET);
#endif
#ifdef vPIN_BATTERY_UNDER_VOLTAGE_SET
    variables[Variable::BATTERY_UNDER_VOLTAGE_SET].blynkVPin = new uint8_t(vPIN_BATTERY_UNDER_VOLTAGE_SET);
#endif

    /**
   * 
   *  MQTT
   * 
   */

#if defined MQTT_TOPIC_PV_POWER
    variables[Variable::PV_POWER].mqttTopic = MQTT_TOPIC_PV_POWER;
#endif
#ifdef MQTT_TOPIC_PV_CURRENT
    variables[Variable::PV_CURRENT].mqttTopic = MQTT_TOPIC_PV_CURRENT;
#endif
#ifdef MQTT_TOPIC_PV_VOLTAGE
    variables[Variable::PV_VOLTAGE].mqttTopic = MQTT_TOPIC_PV_VOLTAGE;
#endif
#ifdef MQTT_TOPIC_LOAD_CURRENT
    variables[Variable::LOAD_CURRENT].mqttTopic = MQTT_TOPIC_LOAD_CURRENT;
#endif
#ifdef MQTT_TOPIC_LOAD_POWER
    variables[Variable::LOAD_POWER].mqttTopic = MQTT_TOPIC_LOAD_POWER;
#endif
#ifdef MQTT_TOPIC_BATT_TEMP
    variables[Variable::BATTERY_TEMP].mqttTopic = MQTT_TOPIC_BATT_TEMP;
#endif
#ifdef MQTT_TOPIC_BATT_VOLTAGE
    variables[Variable::BATTERY_VOLTAGE].mqttTopic = MQTT_TOPIC_BATT_VOLTAGE;
#endif
#ifdef MQTT_TOPIC_BATT_REMAIN
    variables[Variable::BATTERY_SOC].mqttTopic = MQTT_TOPIC_BATT_REMAIN;
#endif
#ifdef MQTT_TOPIC_CONTROLLER_TEMP
    variables[Variable::CONTROLLER_TEMP].mqttTopic = MQTT_TOPIC_CONTROLLER_TEMP;
#endif
#ifdef MQTT_TOPIC_BATTERY_CHARGE_CURRENT
    variables[Variable::BATTERY_CHARGE_CURRENT].mqttTopic = MQTT_TOPIC_BATTERY_CHARGE_CURRENT;
#endif
#ifdef MQTT_TOPIC_BATTERY_CHARGE_POWER
    variables[Variable::BATTERY_CHARGE_POWER].mqttTopic = MQTT_TOPIC_BATTERY_CHARGE_POWER;
#endif
#ifdef MQTT_TOPIC_BATTERY_OVERALL_CURRENT
    variables[Variable::BATTERY_OVERALL_CURRENT].mqttTopic = MQTT_TOPIC_BATTERY_OVERALL_CURRENT;
#endif
#ifdef MQTT_TOPIC_LOAD_ENABLED
    variables[Variable::LOAD_MANUAL_ONOFF].mqttTopic = MQTT_TOPIC_LOAD_ENABLED;
#endif
#ifdef MQTT_TOPIC_CHARGE_DEVICE_ENABLED
    variables[Variable::CHARGING_DEVICE_ONOFF].mqttTopic = MQTT_TOPIC_CHARGE_DEVICE_ENABLED;
#endif
#ifdef MQTT_TOPIC_BATTERY_STATUS_TEXT
    variables[Variable::BATTERY_STATUS_TEXT].mqttTopic = MQTT_TOPIC_BATTERY_STATUS_TEXT;
#endif
#ifdef MQTT_TOPIC_CHARGING_EQUIPMENT_STATUS_TEXT
    variables[Variable::CHARGING_EQUIPMENT_STATUS_TEXT].mqttTopic = MQTT_TOPIC_CHARGING_EQUIPMENT_STATUS_TEXT;
#endif
#ifdef MQTT_TOPIC_DISCHARGING_EQUIPMENT_STATUS_TEXT
    variables[Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT].mqttTopic = MQTT_TOPIC_DISCHARGING_EQUIPMENT_STATUS_TEXT;
#endif
#ifdef MQTT_TOPIC_CONTROLLER_HEATSINK_TEMP
    variables[Variable::HEATSINK_TEMP].mqttTopic = MQTT_TOPIC_CONTROLLER_HEATSINK_TEMP;
#endif
#ifdef MQTT_TOPIC_STAT_ENERGY_GENERATED_TODAY
    variables[Variable::GENERATED_ENERGY_TODAY].mqttTopic = MQTT_TOPIC_STAT_ENERGY_GENERATED_TODAY;
#endif
#ifdef MQTT_TOPIC_STAT_ENERGY_GENERATED_THIS_MONTH
    variables[Variable::GENERATED_ENERGY_MONTH].mqttTopic = MQTT_TOPIC_STAT_ENERGY_GENERATED_THIS_MONTH;
#endif
#ifdef MQTT_TOPIC_STAT_ENERGY_GENERATED_THIS_YEAR
    variables[Variable::GENERATED_ENERGY_YEAR].mqttTopic = MQTT_TOPIC_STAT_ENERGY_GENERATED_THIS_YEAR;
#endif
#ifdef MQTT_TOPIC_STAT_ENERGY_GENERATED_TOTAL
    variables[Variable::GENERATED_ENERGY_TOTAL].mqttTopic = MQTT_TOPIC_STAT_ENERGY_GENERATED_TOTAL;
#endif
#ifdef MQTT_TOPIC_MIN_BATTERY_VOLTAGE_TODAY
    variables[Variable::MINIMUM_BATTERY_VOLTAGE_TODAY].mqttTopic = MQTT_TOPIC_MIN_BATTERY_VOLTAGE_TODAY;
#endif
#ifdef MQTT_TOPIC_MAX_BATTERY_VOLTAGE_TODAY
    variables[Variable::MAXIMUM_BATTERY_VOLTAGE_TODAY].mqttTopic = MQTT_TOPIC_MAX_BATTERY_VOLTAGE_TODAY;
#endif
#ifdef MQTT_TOPIC_MIN_PV_VOLTAGE_TODAY
    variables[Variable::MINIMUM_PV_VOLTAGE_TODAY].mqttTopic = MQTT_TOPIC_MIN_PV_VOLTAGE_TODAY;
#endif
#ifdef MQTT_TOPIC_MAX_PV_VOLTAGE_TODAY
    variables[Variable::MAXIMUM_PV_VOLTAGE_TODAY].mqttTopic = MQTT_TOPIC_MAX_PV_VOLTAGE_TODAY;
#endif
#ifdef MQTT_TOPIC_BATTERY_BOOST_VOLTAGE
    variables[Variable::BATTERY_BOOST_VOLTAGE].mqttTopic = MQTT_TOPIC_BATTERY_BOOST_VOLTAGE;
#endif
#ifdef MQTT_TOPIC_BATTERY_EQUALIZATION_VOLTAGE
    variables[Variable::BATTERY_EQUALIZATION_VOLTAGE].mqttTopic = MQTT_TOPIC_BATTERY_EQUALIZATION_VOLTAGE;
#endif
#ifdef MQTT_TOPIC_BATTERY_FLOAT_VOLTAGE
    variables[Variable::BATTERY_FLOAT_VOLTAGE].mqttTopic = MQTT_TOPIC_BATTERY_FLOAT_VOLTAGE;
#endif
#ifdef MQTT_TOPIC_BATTERY_FLOAT_MIN_VOLTAGE
    variables[Variable::BATTERY_FLOAT_MIN_VOLTAGE].mqttTopic = MQTT_TOPIC_BATTERY_FLOAT_MIN_VOLTAGE;
#endif
#ifdef MQTT_TOPIC_BATTERY_CHARGING_LIMIT_VOLTAGE
    variables[Variable::BATTERY_CHARGING_LIMIT_VOLTAGE].mqttTopic = MQTT_TOPIC_BATTERY_CHARGING_LIMIT_VOLTAGE;
#endif
#ifdef MQTT_TOPIC_BATTERY_DISCHARGING_LIMIT_VOLTAGE
    variables[Variable::BATTERY_DISCHARGING_LIMIT_VOLTAGE].mqttTopic = MQTT_TOPIC_BATTERY_DISCHARGING_LIMIT_VOLTAGE;
#endif
#ifdef MQTT_TOPIC_BATTERY_LOW_VOLTAGE_DISCONNECT
    variables[Variable::BATTERY_LOW_VOLTAGE_DISCONNECT].mqttTopic = MQTT_TOPIC_BATTERY_LOW_VOLTAGE_DISCONNECT;
#endif
#ifdef MQTT_TOPIC_BATTERY_LOW_VOLTAGE_RECONNECT
    variables[Variable::BATTERY_LOW_VOLTAGE_RECONNECT].mqttTopic = MQTT_TOPIC_BATTERY_LOW_VOLTAGE_RECONNECT;
#endif
#ifdef MQTT_TOPIC_BATTERY_OVER_VOLTAGE_DISCONNECT
    variables[Variable::BATTERY_OVER_VOLTAGE_DISCONNECT].mqttTopic = MQTT_TOPIC_BATTERY_OVER_VOLTAGE_DISCONNECT;
#endif
#ifdef MQTT_TOPIC_BATTERY_OVER_VOLTAGE_RECONNECT
    variables[Variable::BATTERY_OVER_VOLTAGE_RECONNECT].mqttTopic = MQTT_TOPIC_BATTERY_OVER_VOLTAGE_RECONNECT;
#endif
#ifdef MQTT_TOPIC_BATTERY_UNDER_VOLTAGE_RESET
    variables[Variable::BATTERY_UNDER_VOLTAGE_RESET].mqttTopic = MQTT_TOPIC_BATTERY_UNDER_VOLTAGE_RESET;
#endif
#ifdef MQTT_TOPIC_BATTERY_UNDER_VOLTAGE_SET
    variables[Variable::BATTERY_UNDER_VOLTAGE_SET].mqttTopic = MQTT_TOPIC_BATTERY_UNDER_VOLTAGE_SET;
#endif
  }

  void initializeVariable(Variable variable, const char * text, VariableDatatype datatype, VariableUOM uom, VariableSource source)
  {
    variables[variable] = {variable, text, datatype, uom, source, nullptr, nullptr};
  }

};

#endif
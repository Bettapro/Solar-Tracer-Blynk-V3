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

#include "../incl/project_core_config.h"

#ifndef vPIN_PV_POWER
#define vPIN_PV_POWER_DF nullptr
#else
#define vPIN_PV_POWER_DF new uint8_t(vPIN_PV_POWER)
#endif
#ifndef vPIN_PV_CURRENT
#define vPIN_PV_CURRENT_DF nullptr
#else
#define vPIN_PV_CURRENT_DF new uint8_t(vPIN_PV_CURRENT)
#endif
#ifndef vPIN_PV_VOLTAGE
#define vPIN_PV_VOLTAGE_DF nullptr
#else
#define vPIN_PV_VOLTAGE_DF new uint8_t(vPIN_PV_VOLTAGE)
#endif
#ifndef vPIN_LOAD_CURRENT
#define vPIN_LOAD_CURRENT_DF nullptr
#else
#define vPIN_LOAD_CURRENT_DF new uint8_t(vPIN_LOAD_CURRENT)
#endif
#ifndef vPIN_LOAD_POWER
#define vPIN_LOAD_POWER_DF nullptr
#else
#define vPIN_LOAD_POWER_DF new uint8_t(vPIN_LOAD_POWER)
#endif
#ifndef vPIN_BATT_TEMP
#define vPIN_BATT_TEMP_DF nullptr
#else
#define vPIN_BATT_TEMP_DF new uint8_t(vPIN_BATT_TEMP)
#endif
#ifndef vPIN_BATT_VOLTAGE
#define vPIN_BATT_VOLTAGE_DF nullptr
#else
#define vPIN_BATT_VOLTAGE_DF new uint8_t(vPIN_BATT_VOLTAGE)
#endif
#ifndef vPIN_BATT_REMAIN
#define vPIN_BATT_REMAIN_DF nullptr
#else
#define vPIN_BATT_REMAIN_DF new uint8_t(vPIN_BATT_REMAIN)
#endif
#ifndef vPIN_CONTROLLER_TEMP
#define vPIN_CONTROLLER_TEMP_DF nullptr
#else
#define vPIN_CONTROLLER_TEMP_DF new uint8_t(vPIN_CONTROLLER_TEMP)
#endif
#ifndef vPIN_BATTERY_CHARGE_CURRENT
#define vPIN_BATTERY_CHARGE_CURRENT_DF nullptr
#else
#define vPIN_BATTERY_CHARGE_CURRENT_DF new uint8_t(vPIN_BATTERY_CHARGE_CURRENT)
#endif
#ifndef vPIN_BATTERY_CHARGE_POWER
#define vPIN_BATTERY_CHARGE_POWER_DF nullptr
#else
#define vPIN_BATTERY_CHARGE_POWER_DF new uint8_t(vPIN_BATTERY_CHARGE_POWER)
#endif
#ifndef vPIN_BATTERY_OVERALL_CURRENT
#define vPIN_BATTERY_OVERALL_CURRENT_DF nullptr
#else
#define vPIN_BATTERY_OVERALL_CURRENT_DF new uint8_t(vPIN_BATTERY_OVERALL_CURRENT)
#endif
#ifndef vPIN_LOAD_ENABLED
#define vPIN_LOAD_ENABLED_DF nullptr
#else
#define vPIN_LOAD_ENABLED_DF new uint8_t(vPIN_LOAD_ENABLED)
#endif
#ifndef vPIN_CHARGE_DEVICE_ENABLED
#define vPIN_CHARGE_DEVICE_ENABLED_DF nullptr
#else
#define vPIN_CHARGE_DEVICE_ENABLED_DF new uint8_t(vPIN_CHARGE_DEVICE_ENABLED)
#endif
#ifndef vPIN_BATTERY_STATUS_TEXT
#define vPIN_BATTERY_STATUS_TEXT_DF nullptr
#else
#define vPIN_BATTERY_STATUS_TEXT_DF new uint8_t(vPIN_BATTERY_STATUS_TEXT)
#endif
#ifndef vPIN_CHARGING_EQUIPMENT_STATUS_TEXT
#define vPIN_CHARGING_EQUIPMENT_STATUS_TEXT_DF nullptr
#else
#define vPIN_CHARGING_EQUIPMENT_STATUS_TEXT_DF new uint8_t(vPIN_CHARGING_EQUIPMENT_STATUS_TEXT)
#endif
#ifndef vPIN_DISCHARGING_EQUIPMENT_STATUS_TEXT
#define vPIN_DISCHARGING_EQUIPMENT_STATUS_TEXT_DF nullptr
#else
#define vPIN_DISCHARGING_EQUIPMENT_STATUS_TEXT_DF new uint8_t(vPIN_DISCHARGING_EQUIPMENT_STATUS_TEXT)
#endif
#ifndef vPIN_CONTROLLER_HEATSINK_TEMP
#define vPIN_CONTROLLER_HEATSINK_TEMP_DF nullptr
#else
#define vPIN_CONTROLLER_HEATSINK_TEMP_DF new uint8_t(vPIN_CONTROLLER_HEATSINK_TEMP)
#endif
#ifndef vPIN_STAT_ENERGY_GENERATED_TODAY
#define vPIN_STAT_ENERGY_GENERATED_TODAY_DF nullptr
#else
#define vPIN_STAT_ENERGY_GENERATED_TODAY_DF new uint8_t(vPIN_STAT_ENERGY_GENERATED_TODAY)
#endif
#ifndef vPIN_STAT_ENERGY_GENERATED_THIS_MONTH
#define vPIN_STAT_ENERGY_GENERATED_THIS_MONTH_DF nullptr
#else
#define vPIN_STAT_ENERGY_GENERATED_THIS_MONTH_DF new uint8_t(vPIN_STAT_ENERGY_GENERATED_THIS_MONTH)
#endif
#ifndef vPIN_STAT_ENERGY_GENERATED_THIS_YEAR
#define vPIN_STAT_ENERGY_GENERATED_THIS_YEAR_DF nullptr
#else
#define vPIN_STAT_ENERGY_GENERATED_THIS_YEAR_DF new uint8_t(vPIN_STAT_ENERGY_GENERATED_THIS_YEAR)
#endif
#ifndef vPIN_STAT_ENERGY_GENERATED_TOTAL
#define vPIN_STAT_ENERGY_GENERATED_TOTAL_DF nullptr
#else
#define vPIN_STAT_ENERGY_GENERATED_TOTAL_DF new uint8_t(vPIN_STAT_ENERGY_GENERATED_TOTAL)
#endif
#ifndef vPIN_MIN_BATTERY_VOLTAGE_TODAY
#define vPIN_MIN_BATTERY_VOLTAGE_TODAY_DF nullptr
#else
#define vPIN_MIN_BATTERY_VOLTAGE_TODAY_DF new uint8_t(vPIN_MIN_BATTERY_VOLTAGE_TODAY)
#endif
#ifndef vPIN_MAX_BATTERY_VOLTAGE_TODAY
#define vPIN_MAX_BATTERY_VOLTAGE_TODAY_DF nullptr
#else
#define vPIN_MAX_BATTERY_VOLTAGE_TODAY_DF new uint8_t(vPIN_MAX_BATTERY_VOLTAGE_TODAY)
#endif
#ifndef vPIN_MIN_PV_VOLTAGE_TODAY
#define vPIN_MIN_PV_VOLTAGE_TODAY_DF nullptr
#else
#define vPIN_MIN_PV_VOLTAGE_TODAY_DF new uint8_t(vPIN_MIN_PV_VOLTAGE_TODAY)
#endif
#ifndef vPIN_MAX_PV_VOLTAGE_TODAY
#define vPIN_MAX_PV_VOLTAGE_TODAY_DF nullptr
#else
#define vPIN_MAX_PV_VOLTAGE_TODAY_DF new uint8_t(vPIN_MAX_PV_VOLTAGE_TODAY)
#endif
#ifndef vPIN_BATTERY_BOOST_VOLTAGE
#define vPIN_BATTERY_BOOST_VOLTAGE_DF nullptr
#else
#define vPIN_BATTERY_BOOST_VOLTAGE_DF new uint8_t(vPIN_BATTERY_BOOST_VOLTAGE)
#endif
#ifndef vPIN_BATTERY_EQUALIZATION_VOLTAGE
#define vPIN_BATTERY_EQUALIZATION_VOLTAGE_DF nullptr
#else
#define vPIN_BATTERY_EQUALIZATION_VOLTAGE_DF new uint8_t(vPIN_BATTERY_EQUALIZATION_VOLTAGE)
#endif
#ifndef vPIN_BATTERY_FLOAT_VOLTAGE
#define vPIN_BATTERY_FLOAT_VOLTAGE_DF nullptr
#else
#define vPIN_BATTERY_FLOAT_VOLTAGE_DF new uint8_t(vPIN_BATTERY_FLOAT_VOLTAGE)
#endif
#ifndef vPIN_BATTERY_FLOAT_MIN_VOLTAGE
#define vPIN_BATTERY_FLOAT_MIN_VOLTAGE_DF nullptr
#else
#define vPIN_BATTERY_FLOAT_MIN_VOLTAGE_DF new uint8_t(vPIN_BATTERY_FLOAT_MIN_VOLTAGE)
#endif
#ifndef vPIN_BATTERY_CHARGING_LIMIT_VOLTAGE
#define vPIN_BATTERY_CHARGING_LIMIT_VOLTAGE_DF nullptr
#else
#define vPIN_BATTERY_CHARGING_LIMIT_VOLTAGE_DF new uint8_t(vPIN_BATTERY_CHARGING_LIMIT_VOLTAGE)
#endif
#ifndef vPIN_BATTERY_DISCHARGING_LIMIT_VOLTAGE
#define vPIN_BATTERY_DISCHARGING_LIMIT_VOLTAGE_DF nullptr
#else
#define vPIN_BATTERY_DISCHARGING_LIMIT_VOLTAGE_DF new uint8_t(vPIN_BATTERY_DISCHARGING_LIMIT_VOLTAGE)
#endif
#ifndef vPIN_BATTERY_LOW_VOLTAGE_DISCONNECT
#define vPIN_BATTERY_LOW_VOLTAGE_DISCONNECT_DF nullptr
#else
#define vPIN_BATTERY_LOW_VOLTAGE_DISCONNECT_DF new uint8_t(vPIN_BATTERY_LOW_VOLTAGE_DISCONNECT)
#endif
#ifndef vPIN_BATTERY_LOW_VOLTAGE_RECONNECT
#define vPIN_BATTERY_LOW_VOLTAGE_RECONNECT_DF nullptr
#else
#define vPIN_BATTERY_LOW_VOLTAGE_RECONNECT_DF new uint8_t(vPIN_BATTERY_LOW_VOLTAGE_RECONNECT)
#endif
#ifndef vPIN_BATTERY_OVER_VOLTAGE_DISCONNECT
#define vPIN_BATTERY_OVER_VOLTAGE_DISCONNECT_DF nullptr
#else
#define vPIN_BATTERY_OVER_VOLTAGE_DISCONNECT_DF new uint8_t(vPIN_BATTERY_OVER_VOLTAGE_DISCONNECT)
#endif
#ifndef vPIN_BATTERY_OVER_VOLTAGE_RECONNECT
#define vPIN_BATTERY_OVER_VOLTAGE_RECONNECT_DF nullptr
#else
#define vPIN_BATTERY_OVER_VOLTAGE_RECONNECT_DF new uint8_t(vPIN_BATTERY_OVER_VOLTAGE_RECONNECT)
#endif
#ifndef vPIN_BATTERY_UNDER_VOLTAGE_RESET
#define vPIN_BATTERY_UNDER_VOLTAGE_RESET_DF nullptr
#else
#define vPIN_BATTERY_UNDER_VOLTAGE_RESET_DF new uint8_t(vPIN_BATTERY_UNDER_VOLTAGE_RESET)
#endif
#ifndef vPIN_BATTERY_UNDER_VOLTAGE_SET
#define vPIN_BATTERY_UNDER_VOLTAGE_SET_DF nullptr
#else
#define vPIN_BATTERY_UNDER_VOLTAGE_SET_DF new uint8_t(vPIN_BATTERY_UNDER_VOLTAGE_SET)
#endif
#ifndef vPIN_INTERNAL_STATUS
#define vPIN_INTERNAL_STATUS_DF nullptr
#else
#define vPIN_INTERNAL_STATUS_DF new uint8_t(vPIN_INTERNAL_STATUS)
#endif
#ifndef vPIN_INTERNAL_DEBUG_TERMINAL
#define vPIN_INTERNAL_DEBUG_TERMINAL_DF nullptr
#else
#define vPIN_INTERNAL_DEBUG_TERMINAL_DF new uint8_t(vPIN_INTERNAL_DEBUG_TERMINAL)
#endif
#ifndef vPIN_UPDATE_CONTROLLER_DATETIME
#define vPIN_UPDATE_CONTROLLER_DATETIME_DF nullptr
#else
#define vPIN_UPDATE_CONTROLLER_DATETIME_DF new uint8_t(vPIN_UPDATE_CONTROLLER_DATETIME)
#endif
#ifndef vPIN_UPDATE_ALL_CONTROLLER_DATA
#define vPIN_UPDATE_ALL_CONTROLLER_DATA_DF nullptr
#else
#define vPIN_UPDATE_ALL_CONTROLLER_DATA_DF new uint8_t(vPIN_UPDATE_ALL_CONTROLLER_DATA)
#endif

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
        DT_FLOAT,
        DT_BOOL
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
        INTERNAL_DEBUG,
        //----------------
        UPDATE_ALL_CONTROLLER_DATA,
        //----------------
        VARIABLES_COUNT
} Variable;

struct VariableDefinition
{
        Variable variable;
        const char *text;
        VariableDatatype datatype;
        VariableUOM uom;
        VariableSource source;
        const uint8_t *blynkVPin;
        const char *mqttTopic;
};

class VariableDefiner
{
public:
        static VariableDefiner &getInstance()
        {
                static VariableDefiner instance;
                return instance;
        }

        const VariableDefinition *getDefinition(Variable variable);

        const VariableDefinition *getDefinitionByBlynkVPin(uint8_t pin);

        VariableDatatype getDatatype(Variable variable);

private:
        VariableDefiner();

        void initializeVariable(Variable variable, const char *text, VariableDatatype datatype, VariableUOM uom, VariableSource source, uint8_t *blynkPin);

        VariableDefinition *variables = new VariableDefinition[Variable::VARIABLES_COUNT + 1]();
};




#endif
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

#include "../incl/include_all_core.h"

#include "../incl/include_all_blynk_vpin.h"
#include "../incl/include_all_mqtt_topic.h"

typedef enum
{
        SR_REALTIME,
        SR_STATS,
        SR_INTERNAL
} VariableSource;

typedef enum
{
        MD_READ,
        MD_READWRITE
} VariableMode;

typedef enum
{
        DT_UNDEFINED,
        DT_STRING,
        DT_FLOAT,
        DT_BOOL,
        DT_UINT16
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
        UOM_AMPEREHOUR,
        UOM_TEMPERATURE_C,
        UOM_MINUTE
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
        BATTERY_RATED_VOLTAGE, // 0 - auto detect, otherwise specify the voltage of the battery
        BATTERY_TYPE, // 0 - custom, 1 - Sealed / AGM, 2 - GEL, 3 - Flooded / Wet Cell
        BATTERY_CAPACITY,
        BATTERY_EQUALIZATION_DURATION,
        BATTERY_BOOST_DURATION,
        BATTERY_TEMPERATURE_COMPENSATION_COEFFICIENT,
        BATTERY_MANAGEMENT_MODE, // 0 - voltage compensation, 1 - SOC
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
        VariableMode mode;
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

        const VariableDefinition *getDefinitionByMqttTopic(const char *mqttTopic);

        VariableDatatype getDatatype(Variable variable);

        bool isFromScc(const Variable variable)
        {
                return this->variables[variable].source != VariableSource::SR_INTERNAL;
        }

        bool isValueEqual(Variable variable, const void *val1, const void *val2)
        {
                uint8_t valueSize = this->getVariableSize(variable);
                if (valueSize > 0)
                {
                        return memcmp(val1, val2, valueSize) == 0;
                }
                return false;
        }

        uint8_t getVariableSize(Variable variable);

private:
        VariableDefiner();

        void initializeVariable(Variable variable, const char *text, VariableDatatype datatype, VariableUOM uom, VariableSource source, VariableMode mode, uint8_t *blynkPin, const char *mqttTopic);

        VariableDefinition *variables = new VariableDefinition[Variable::VARIABLES_COUNT]();
};

#endif
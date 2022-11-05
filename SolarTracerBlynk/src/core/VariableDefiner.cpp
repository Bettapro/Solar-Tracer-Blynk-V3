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

#include "VariableDefiner.h"
#include "../incl/include_all_core.h"

VariableDefiner::VariableDefiner()
{
    this->variables = new VariableDefinition[Variable::VARIABLES_COUNT]();

    this->initializeVariable(Variable::PV_VOLTAGE, "PV volt.", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_PV_VOLTAGE_DF, MQTT_TOPIC_PV_VOLTAGE_DF);
    this->initializeVariable(Variable::PV_POWER, "PV power", VariableDatatype::DT_FLOAT, VariableUOM::UOM_WATT, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_PV_POWER_DF, MQTT_TOPIC_PV_POWER_DF);
    this->initializeVariable(Variable::PV_CURRENT, "PV current", VariableDatatype::DT_FLOAT, VariableUOM::UOM_AMPERE, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_PV_CURRENT_DF, MQTT_TOPIC_PV_CURRENT_DF);
    this->initializeVariable(Variable::LOAD_CURRENT, "Load current", VariableDatatype::DT_FLOAT, VariableUOM::UOM_AMPERE, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_LOAD_CURRENT_DF, MQTT_TOPIC_LOAD_CURRENT_DF);
    this->initializeVariable(Variable::LOAD_POWER, "Load power", VariableDatatype::DT_FLOAT, VariableUOM::UOM_WATT, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_LOAD_POWER_DF, MQTT_TOPIC_LOAD_POWER_DF);
    this->initializeVariable(Variable::BATTERY_TEMP, "Batt. temp", VariableDatatype::DT_FLOAT, VariableUOM::UOM_TEMPERATURE_C, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_BATT_TEMP_DF, MQTT_TOPIC_BATT_TEMP_DF);
    this->initializeVariable(Variable::BATTERY_VOLTAGE, "Batt. volt.", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_BATT_VOLTAGE_DF, MQTT_TOPIC_BATT_VOLTAGE_DF);
    this->initializeVariable(Variable::BATTERY_SOC, "Batt. SOC", VariableDatatype::DT_FLOAT, VariableUOM::UOM_PERCENT, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_BATT_REMAIN_DF, MQTT_TOPIC_BATT_REMAIN_DF);
    this->initializeVariable(Variable::CONTROLLER_TEMP, "Controller temp.", VariableDatatype::DT_FLOAT, VariableUOM::UOM_TEMPERATURE_C, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_CONTROLLER_TEMP_DF, MQTT_TOPIC_CONTROLLER_TEMP_DF);
    this->initializeVariable(Variable::BATTERY_CHARGE_CURRENT, "Charging current", VariableDatatype::DT_FLOAT, VariableUOM::UOM_AMPERE, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_BATTERY_CHARGE_CURRENT_DF, MQTT_TOPIC_BATTERY_CHARGE_CURRENT_DF);
    this->initializeVariable(Variable::BATTERY_CHARGE_POWER, "Charging power", VariableDatatype::DT_FLOAT, VariableUOM::UOM_WATT, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_BATTERY_CHARGE_POWER_DF, MQTT_TOPIC_BATTERY_CHARGE_POWER_DF);
    this->initializeVariable(Variable::BATTERY_OVERALL_CURRENT, "Overall current", VariableDatatype::DT_FLOAT, VariableUOM::UOM_AMPERE, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_BATTERY_OVERALL_CURRENT_DF, MQTT_TOPIC_BATTERY_OVERALL_CURRENT_DF);
    this->initializeVariable(Variable::REALTIME_CLOCK, "Date and time", VariableDatatype::DT_FLOAT, VariableUOM::UOM_UNDEFINED, VariableSource::SR_INTERNAL, VariableMode::MD_READWRITE, vPIN_UPDATE_CONTROLLER_DATETIME_DF, MQTT_TOPIC_UPDATE_CONTROLLER_DATETIME_DF);
    this->initializeVariable(Variable::LOAD_FORCE_ONOFF, "Load force switch", VariableDatatype::DT_BOOL, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME, VariableMode::MD_READ, nullptr, nullptr);
    this->initializeVariable(Variable::LOAD_MANUAL_ONOFF, "Load switch", VariableDatatype::DT_BOOL, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME, VariableMode::MD_READWRITE, vPIN_LOAD_ENABLED_DF, MQTT_TOPIC_LOAD_ENABLED_DF);
    this->initializeVariable(Variable::REMOTE_BATTERY_TEMP, "Remote batt. temp.", VariableDatatype::DT_FLOAT, VariableUOM::UOM_TEMPERATURE_C, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_BATT_TEMP_DF, MQTT_TOPIC_BATT_TEMP_DF);
    this->initializeVariable(Variable::GENERATED_ENERGY_TODAY, "Energy generated today", VariableDatatype::DT_FLOAT, VariableUOM::UOM_KILOWATTHOUR, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_STAT_ENERGY_GENERATED_TODAY_DF, MQTT_TOPIC_STAT_ENERGY_GENERATED_TODAY_DF);
    this->initializeVariable(Variable::GENERATED_ENERGY_MONTH, "Energy generated month", VariableDatatype::DT_FLOAT, VariableUOM::UOM_KILOWATTHOUR, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_STAT_ENERGY_GENERATED_THIS_MONTH_DF, MQTT_TOPIC_STAT_ENERGY_GENERATED_THIS_MONTH_DF);
    this->initializeVariable(Variable::GENERATED_ENERGY_YEAR, "Energy generated year", VariableDatatype::DT_FLOAT, VariableUOM::UOM_KILOWATTHOUR, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_STAT_ENERGY_GENERATED_THIS_YEAR_DF, MQTT_TOPIC_STAT_ENERGY_GENERATED_THIS_YEAR_DF);
    this->initializeVariable(Variable::GENERATED_ENERGY_TOTAL, "Energy generated", VariableDatatype::DT_FLOAT, VariableUOM::UOM_KILOWATTHOUR, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_STAT_ENERGY_GENERATED_TOTAL_DF, MQTT_TOPIC_STAT_ENERGY_GENERATED_TOTAL_DF);
    this->initializeVariable(Variable::MAXIMUM_PV_VOLTAGE_TODAY, "Today PV max. volt.", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_MAX_PV_VOLTAGE_TODAY_DF, MQTT_TOPIC_MAX_PV_VOLTAGE_TODAY_DF);
    this->initializeVariable(Variable::MINIMUM_PV_VOLTAGE_TODAY, "Today PV min. volt.", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_MIN_PV_VOLTAGE_TODAY_DF, MQTT_TOPIC_MIN_PV_VOLTAGE_TODAY_DF);
    this->initializeVariable(Variable::MAXIMUM_BATTERY_VOLTAGE_TODAY, "Today batt. max. volt.", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_MAX_BATTERY_VOLTAGE_TODAY_DF, MQTT_TOPIC_MAX_BATTERY_VOLTAGE_TODAY_DF);
    this->initializeVariable(Variable::MINIMUM_BATTERY_VOLTAGE_TODAY, "Today batt. min. volt.", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_MIN_BATTERY_VOLTAGE_TODAY_DF, MQTT_TOPIC_MIN_BATTERY_VOLTAGE_TODAY_DF);
    this->initializeVariable(Variable::BATTERY_BOOST_VOLTAGE, "Boost volt.", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_BOOST_VOLTAGE_DF, MQTT_TOPIC_BATTERY_BOOST_VOLTAGE_DF);
    this->initializeVariable(Variable::BATTERY_EQUALIZATION_VOLTAGE, "Equalization volt.", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_EQUALIZATION_VOLTAGE_DF, MQTT_TOPIC_BATTERY_EQUALIZATION_VOLTAGE_DF);
    this->initializeVariable(Variable::BATTERY_FLOAT_VOLTAGE, "Float volt.", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_FLOAT_VOLTAGE_DF, MQTT_TOPIC_BATTERY_FLOAT_VOLTAGE_DF);
    this->initializeVariable(Variable::BATTERY_FLOAT_MIN_VOLTAGE, "Float min. volt.", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_FLOAT_MIN_VOLTAGE_DF, MQTT_TOPIC_BATTERY_FLOAT_MIN_VOLTAGE_DF);
    this->initializeVariable(Variable::BATTERY_CHARGING_LIMIT_VOLTAGE, "Charging volt. limit", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_CHARGING_LIMIT_VOLTAGE_DF, MQTT_TOPIC_BATTERY_CHARGING_LIMIT_VOLTAGE_DF);
    this->initializeVariable(Variable::BATTERY_DISCHARGING_LIMIT_VOLTAGE, "Discharging volt. limit", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_DISCHARGING_LIMIT_VOLTAGE_DF, MQTT_TOPIC_BATTERY_DISCHARGING_LIMIT_VOLTAGE_DF);
    this->initializeVariable(Variable::BATTERY_LOW_VOLTAGE_DISCONNECT, "Low volt. disconnect", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_LOW_VOLTAGE_DISCONNECT_DF, MQTT_TOPIC_BATTERY_LOW_VOLTAGE_DISCONNECT_DF);
    this->initializeVariable(Variable::BATTERY_LOW_VOLTAGE_RECONNECT, "Low volt. reconnect", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_LOW_VOLTAGE_RECONNECT_DF, MQTT_TOPIC_BATTERY_LOW_VOLTAGE_RECONNECT_DF);
    this->initializeVariable(Variable::BATTERY_OVER_VOLTAGE_DISCONNECT, "Over volt. disconnect", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_OVER_VOLTAGE_DISCONNECT_DF, MQTT_TOPIC_BATTERY_OVER_VOLTAGE_DISCONNECT_DF);
    this->initializeVariable(Variable::BATTERY_OVER_VOLTAGE_RECONNECT, "Over volt. reconnect", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_OVER_VOLTAGE_RECONNECT_DF, MQTT_TOPIC_BATTERY_OVER_VOLTAGE_RECONNECT_DF);
    this->initializeVariable(Variable::BATTERY_UNDER_VOLTAGE_SET, "Under volt. alarm set", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_UNDER_VOLTAGE_SET_DF, MQTT_TOPIC_BATTERY_UNDER_VOLTAGE_SET_DF);
    this->initializeVariable(Variable::BATTERY_UNDER_VOLTAGE_RESET, "Under volt. alarm reset", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_UNDER_VOLTAGE_RESET_DF, MQTT_TOPIC_BATTERY_UNDER_VOLTAGE_RESET_DF);
    this->initializeVariable(Variable::BATTERY_STATUS_TEXT, "Batt. status", VariableDatatype::DT_STRING, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_BATTERY_STATUS_TEXT_DF, MQTT_TOPIC_BATTERY_STATUS_TEXT_DF);
    this->initializeVariable(Variable::CHARGING_EQUIPMENT_STATUS_TEXT, "Charging status", VariableDatatype::DT_STRING, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_CHARGING_EQUIPMENT_STATUS_TEXT_DF, MQTT_TOPIC_CHARGING_EQUIPMENT_STATUS_TEXT_DF);
    this->initializeVariable(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "Discharging status", VariableDatatype::DT_STRING, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_DISCHARGING_EQUIPMENT_STATUS_TEXT_DF, MQTT_TOPIC_DISCHARGING_EQUIPMENT_STATUS_TEXT_DF);
    this->initializeVariable(Variable::CHARGING_DEVICE_ONOFF, "Charging switch", VariableDatatype::DT_BOOL, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME, VariableMode::MD_READWRITE, vPIN_CHARGE_DEVICE_ENABLED_DF, MQTT_TOPIC_CHARGE_DEVICE_ENABLED_DF);
    this->initializeVariable(Variable::HEATSINK_TEMP, "Heatsink temp.", VariableDatatype::DT_FLOAT, VariableUOM::UOM_TEMPERATURE_C, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_CONTROLLER_HEATSINK_TEMP_DF, MQTT_TOPIC_CONTROLLER_HEATSINK_TEMP_DF);
    this->initializeVariable(Variable::INTERNAL_STATUS, "Internal status", VariableDatatype::DT_UINT16, VariableUOM::UOM_UNDEFINED, VariableSource::SR_INTERNAL, VariableMode::MD_READ, vPIN_INTERNAL_STATUS_DF, MQTT_TOPIC_INTERNAL_STATUS_DF);
    this->initializeVariable(Variable::INTERNAL_DEBUG, "Internal debug", VariableDatatype::DT_STRING, VariableUOM::UOM_UNDEFINED, VariableSource::SR_INTERNAL, VariableMode::MD_READ, vPIN_INTERNAL_DEBUG_TERMINAL_DF, MQTT_TOPIC_INTERNAL_DEBUG_TERMINAL_DF);
    this->initializeVariable(Variable::UPDATE_ALL_CONTROLLER_DATA, "Full refresh from scc", VariableDatatype::DT_BOOL, VariableUOM::UOM_UNDEFINED, VariableSource::SR_INTERNAL, VariableMode::MD_READWRITE, vPIN_UPDATE_ALL_CONTROLLER_DATA_DF, MQTT_TOPIC_UPDATE_ALL_CONTROLLER_DATA_DF);
    this->initializeVariable(Variable::BATTERY_RATED_VOLTAGE, "Batt. rated volt.", VariableDatatype::DT_UINT16, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_RATED_VOLTAGE_DF, MQTT_TOPIC_BATTERY_RATED_VOLTAGE_DF);
    this->initializeVariable(Variable::BATTERY_TYPE, "Batt. type", VariableDatatype::DT_UINT16, VariableUOM::UOM_UNDEFINED, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_TYPE_DF, MQTT_TOPIC_BATTERY_TYPE_DF);
    this->initializeVariable(Variable::BATTERY_CAPACITY, "Batt. capacity", VariableDatatype::DT_UINT16, VariableUOM::UOM_AMPEREHOUR, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_CAPACITY_DF, MQTT_TOPIC_BATTERY_CAPACITY_DF);
    this->initializeVariable(Variable::BATTERY_EQUALIZATION_DURATION, "Equalization duration", VariableDatatype::DT_UINT16, VariableUOM::UOM_MINUTE, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_EQUALIZATION_DURATION_DF, MQTT_TOPIC_BATTERY_EQUALIZATION_DURATION_DF);
    this->initializeVariable(Variable::BATTERY_BOOST_DURATION, "Boost duration", VariableDatatype::DT_UINT16, VariableUOM::UOM_MINUTE, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_BOOST_DURATION_DF, MQTT_TOPIC_BATTERY_BOOST_DURATION_DF);
    this->initializeVariable(Variable::BATTERY_TEMPERATURE_COMPENSATION_COEFFICIENT, "Batt. temp. compensation coeff.", VariableDatatype::DT_FLOAT, VariableUOM::UOM_UNDEFINED, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_TEMPERATURE_COMPENSATION_COEFF_DF, MQTT_TOPIC_BATTERY_TEMPERATURE_COMPENSATION_COEFF_DF);
    this->initializeVariable(Variable::BATTERY_MANAGEMENT_MODE, "Batt. management mode", VariableDatatype::DT_UINT16, VariableUOM::UOM_UNDEFINED, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_MANAGEMENT_MODE_DF, MQTT_TOPIC_BATTERY_MANAGEMENT_MODE_DF);
    this->initializeVariable(Variable::CONSUMED_ENERGY_TOTAL, "Energy consumed", VariableDatatype::DT_FLOAT, VariableUOM::UOM_KILOWATTHOUR, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_STAT_ENERGY_CONSUMED_TOTAL_DF, MQTT_TOPIC_STAT_ENERGY_CONSUMED_TOTAL_DF);

}

const VariableDefinition *VariableDefiner::getDefinitionByBlynkVPin(uint8_t pin)
{
    for (uint8_t index = 0; index < Variable::VARIABLES_COUNT; index++)
    {
        if (variables[index].blynkVPin != nullptr && *(variables[index].blynkVPin) == pin)
        {
            return &(variables[index]);
        }
    }
    return nullptr;
}

const VariableDefinition *VariableDefiner::getDefinitionByMqttTopic(const char *topic)
{
    for (uint8_t index = 0; index < Variable::VARIABLES_COUNT; index++)
    {
        if (variables[index].mqttTopic != nullptr && strcmp(variables[index].mqttTopic, topic) == 0)
        {
            return &(variables[index]);
        }
    }
    return nullptr;
}

void VariableDefiner::initializeVariable(Variable variable, const char *text, VariableDatatype datatype, VariableUOM uom, VariableSource source, VariableMode mode, uint8_t *blynkPin, const char *mqttTopic)
{
    variables[variable] = {variable, text, datatype, uom, source, mode, blynkPin, mqttTopic};
}

const VariableDefinition *VariableDefiner::getDefinition(Variable variable)
{
    return &(variables[variable]);
}

VariableDatatype VariableDefiner::getDatatype(Variable variable)
{
    return variables[variable].datatype;
}

uint8_t VariableDefiner::getVariableSize(Variable variable)
{
    switch (this->getDatatype(variable))
    {
    case VariableDatatype::DT_BOOL:
        return sizeof(bool);
    case VariableDatatype::DT_FLOAT:
        return sizeof(float);
    case VariableDatatype::DT_UINT16:
        return sizeof(uint16_t);
    case VariableDatatype::DT_STRING:
        return 20;

    }
    return 0;
}


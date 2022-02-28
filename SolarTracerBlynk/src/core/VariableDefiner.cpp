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

    this->initializeVariable(Variable::PV_VOLTAGE, "PV voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_PV_VOLTAGE_DF, MQTT_TOPIC_PV_VOLTAGE_DF);
    this->initializeVariable(Variable::PV_POWER, "PV power", VariableDatatype::DT_FLOAT, VariableUOM::UOM_WATT, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_PV_POWER_DF, MQTT_TOPIC_PV_POWER_DF);
    this->initializeVariable(Variable::PV_CURRENT, "PV current", VariableDatatype::DT_FLOAT, VariableUOM::UOM_AMPERE, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_PV_CURRENT_DF, MQTT_TOPIC_PV_CURRENT_DF);
    this->initializeVariable(Variable::LOAD_CURRENT, "Load current", VariableDatatype::DT_FLOAT, VariableUOM::UOM_AMPERE, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_LOAD_CURRENT_DF, MQTT_TOPIC_LOAD_CURRENT_DF);
    this->initializeVariable(Variable::LOAD_POWER, "Load power", VariableDatatype::DT_FLOAT, VariableUOM::UOM_WATT, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_LOAD_POWER_DF, MQTT_TOPIC_LOAD_POWER_DF);
    this->initializeVariable(Variable::BATTERY_TEMP, "Battery temp", VariableDatatype::DT_FLOAT, VariableUOM::UOM_TEMPERATURE_C, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_BATT_TEMP_DF, MQTT_TOPIC_BATT_TEMP_DF);
    this->initializeVariable(Variable::BATTERY_VOLTAGE, "Battery voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_BATT_VOLTAGE_DF, MQTT_TOPIC_BATT_VOLTAGE_DF);
    this->initializeVariable(Variable::BATTERY_SOC, "Battery SOC", VariableDatatype::DT_FLOAT, VariableUOM::UOM_PERCENT, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_BATT_REMAIN_DF, MQTT_TOPIC_BATT_REMAIN_DF);
    this->initializeVariable(Variable::CONTROLLER_TEMP, "Controller temperature", VariableDatatype::DT_FLOAT, VariableUOM::UOM_TEMPERATURE_C, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_CONTROLLER_TEMP_DF, MQTT_TOPIC_CONTROLLER_TEMP_DF);
    this->initializeVariable(Variable::BATTERY_CHARGE_CURRENT, "Charging current", VariableDatatype::DT_FLOAT, VariableUOM::UOM_AMPERE, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_BATTERY_CHARGE_CURRENT_DF, MQTT_TOPIC_BATTERY_CHARGE_CURRENT_DF);
    this->initializeVariable(Variable::BATTERY_CHARGE_POWER, "Charging power", VariableDatatype::DT_FLOAT, VariableUOM::UOM_WATT, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_BATTERY_CHARGE_POWER_DF, MQTT_TOPIC_BATTERY_CHARGE_POWER_DF);
    this->initializeVariable(Variable::BATTERY_OVERALL_CURRENT, "Overall current", VariableDatatype::DT_FLOAT, VariableUOM::UOM_AMPERE, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_BATTERY_OVERALL_CURRENT_DF, MQTT_TOPIC_BATTERY_OVERALL_CURRENT_DF);
    this->initializeVariable(Variable::REALTIME_CLOCK, "Date and time", VariableDatatype::DT_FLOAT, VariableUOM::UOM_UNDEFINED, VariableSource::SR_INTERNAL, VariableMode::MD_READWRITE, vPIN_UPDATE_CONTROLLER_DATETIME_DF, MQTT_TOPIC_UPDATE_CONTROLLER_DATETIME_DF);
    this->initializeVariable(Variable::LOAD_FORCE_ONOFF, "Load force switch", VariableDatatype::DT_BOOL, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME, VariableMode::MD_READ, nullptr, nullptr);
    this->initializeVariable(Variable::LOAD_MANUAL_ONOFF, "Load switch", VariableDatatype::DT_BOOL, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME, VariableMode::MD_READWRITE, vPIN_LOAD_ENABLED_DF, MQTT_TOPIC_LOAD_ENABLED_DF);
    this->initializeVariable(Variable::REMOTE_BATTERY_TEMP, "Remote battery temperature", VariableDatatype::DT_FLOAT, VariableUOM::UOM_TEMPERATURE_C, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_BATT_TEMP_DF, MQTT_TOPIC_BATT_TEMP_DF);
    this->initializeVariable(Variable::GENERATED_ENERGY_TODAY, "Energy generated today", VariableDatatype::DT_FLOAT, VariableUOM::UOM_KILOWATTHOUR, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_STAT_ENERGY_GENERATED_TODAY_DF, MQTT_TOPIC_STAT_ENERGY_GENERATED_TODAY_DF);
    this->initializeVariable(Variable::GENERATED_ENERGY_MONTH, "Energy generated month", VariableDatatype::DT_FLOAT, VariableUOM::UOM_KILOWATTHOUR, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_STAT_ENERGY_GENERATED_THIS_MONTH_DF, MQTT_TOPIC_STAT_ENERGY_GENERATED_THIS_MONTH_DF);
    this->initializeVariable(Variable::GENERATED_ENERGY_YEAR, "Energy generated year", VariableDatatype::DT_FLOAT, VariableUOM::UOM_KILOWATTHOUR, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_STAT_ENERGY_GENERATED_THIS_YEAR_DF, MQTT_TOPIC_STAT_ENERGY_GENERATED_THIS_YEAR_DF);
    this->initializeVariable(Variable::GENERATED_ENERGY_TOTAL, "Energy generated", VariableDatatype::DT_FLOAT, VariableUOM::UOM_KILOWATTHOUR, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_STAT_ENERGY_GENERATED_TOTAL_DF, MQTT_TOPIC_STAT_ENERGY_GENERATED_TOTAL_DF);
    this->initializeVariable(Variable::MAXIMUM_PV_VOLTAGE_TODAY, "Today PV min. voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_MIN_PV_VOLTAGE_TODAY_DF, MQTT_TOPIC_MIN_PV_VOLTAGE_TODAY_DF);
    this->initializeVariable(Variable::MINIMUM_PV_VOLTAGE_TODAY, "Today PV min. voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_MAX_PV_VOLTAGE_TODAY_DF, MQTT_TOPIC_MAX_PV_VOLTAGE_TODAY_DF);
    this->initializeVariable(Variable::MAXIMUM_BATTERY_VOLTAGE_TODAY, "Today battery max. voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_MAX_BATTERY_VOLTAGE_TODAY_DF, MQTT_TOPIC_MAX_BATTERY_VOLTAGE_TODAY_DF);
    this->initializeVariable(Variable::MINIMUM_BATTERY_VOLTAGE_TODAY, "Today battery min. voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READ, vPIN_MIN_BATTERY_VOLTAGE_TODAY_DF, MQTT_TOPIC_MIN_BATTERY_VOLTAGE_TODAY_DF);
    this->initializeVariable(Variable::BATTERY_BOOST_VOLTAGE, "Boost voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_BOOST_VOLTAGE_DF, MQTT_TOPIC_BATTERY_BOOST_VOLTAGE_DF);
    this->initializeVariable(Variable::BATTERY_EQUALIZATION_VOLTAGE, "Equalization voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_EQUALIZATION_VOLTAGE_DF, MQTT_TOPIC_BATTERY_EQUALIZATION_VOLTAGE_DF);
    this->initializeVariable(Variable::BATTERY_FLOAT_VOLTAGE, "Float voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_FLOAT_VOLTAGE_DF, MQTT_TOPIC_BATTERY_FLOAT_VOLTAGE_DF);
    this->initializeVariable(Variable::BATTERY_FLOAT_MIN_VOLTAGE, "Float min. voltage", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_FLOAT_MIN_VOLTAGE_DF, MQTT_TOPIC_BATTERY_FLOAT_MIN_VOLTAGE_DF);
    this->initializeVariable(Variable::BATTERY_CHARGING_LIMIT_VOLTAGE, "Charging voltage limit", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_CHARGING_LIMIT_VOLTAGE_DF, MQTT_TOPIC_BATTERY_CHARGING_LIMIT_VOLTAGE_DF);
    this->initializeVariable(Variable::BATTERY_DISCHARGING_LIMIT_VOLTAGE, "Discharging voltage limit", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_DISCHARGING_LIMIT_VOLTAGE_DF, MQTT_TOPIC_BATTERY_DISCHARGING_LIMIT_VOLTAGE_DF);
    this->initializeVariable(Variable::BATTERY_LOW_VOLTAGE_DISCONNECT, "Low voltage disconnect", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_LOW_VOLTAGE_DISCONNECT_DF, MQTT_TOPIC_BATTERY_LOW_VOLTAGE_DISCONNECT_DF);
    this->initializeVariable(Variable::BATTERY_LOW_VOLTAGE_RECONNECT, "Low voltage reconnect", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_LOW_VOLTAGE_RECONNECT_DF, MQTT_TOPIC_BATTERY_LOW_VOLTAGE_RECONNECT_DF);
    this->initializeVariable(Variable::BATTERY_OVER_VOLTAGE_DISCONNECT, "Over voltage disconnect", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_OVER_VOLTAGE_DISCONNECT_DF, MQTT_TOPIC_BATTERY_OVER_VOLTAGE_DISCONNECT_DF);
    this->initializeVariable(Variable::BATTERY_OVER_VOLTAGE_RECONNECT, "Over voltage reconnect", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_OVER_VOLTAGE_RECONNECT_DF, MQTT_TOPIC_BATTERY_OVER_VOLTAGE_RECONNECT_DF);
    this->initializeVariable(Variable::BATTERY_UNDER_VOLTAGE_SET, "Under voltage alarm set", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_UNDER_VOLTAGE_SET_DF, MQTT_TOPIC_BATTERY_UNDER_VOLTAGE_SET_DF);
    this->initializeVariable(Variable::BATTERY_UNDER_VOLTAGE_RESET, "Under voltage alarm reset", VariableDatatype::DT_FLOAT, VariableUOM::UOM_VOLT, VariableSource::SR_STATS, VariableMode::MD_READWRITE, vPIN_BATTERY_UNDER_VOLTAGE_RESET_DF, MQTT_TOPIC_BATTERY_UNDER_VOLTAGE_RESET_DF);
    this->initializeVariable(Variable::BATTERY_STATUS_TEXT, "Battery status", VariableDatatype::DT_STRING, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME, VariableMode::MD_READWRITE, vPIN_BATTERY_STATUS_TEXT_DF, MQTT_TOPIC_BATTERY_STATUS_TEXT_DF);
    this->initializeVariable(Variable::CHARGING_EQUIPMENT_STATUS_TEXT, "Charging status", VariableDatatype::DT_STRING, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_CHARGING_EQUIPMENT_STATUS_TEXT_DF, MQTT_TOPIC_CHARGING_EQUIPMENT_STATUS_TEXT_DF);
    this->initializeVariable(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "Discharging status", VariableDatatype::DT_STRING, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_DISCHARGING_EQUIPMENT_STATUS_TEXT_DF, MQTT_TOPIC_DISCHARGING_EQUIPMENT_STATUS_TEXT_DF);
    this->initializeVariable(Variable::CHARGING_DEVICE_ONOFF, "Charging switch", VariableDatatype::DT_BOOL, VariableUOM::UOM_UNDEFINED, VariableSource::SR_REALTIME, VariableMode::MD_READWRITE, vPIN_CHARGE_DEVICE_ENABLED_DF, MQTT_TOPIC_CHARGE_DEVICE_ENABLED_DF);
    this->initializeVariable(Variable::HEATSINK_TEMP, "Heatsink temperature", VariableDatatype::DT_FLOAT, VariableUOM::UOM_TEMPERATURE_C, VariableSource::SR_REALTIME, VariableMode::MD_READ, vPIN_CONTROLLER_HEATSINK_TEMP_DF, MQTT_TOPIC_CONTROLLER_HEATSINK_TEMP_DF);
    this->initializeVariable(Variable::INTERNAL_STATUS, "Internal status", VariableDatatype::DT_FLOAT, VariableUOM::UOM_UNDEFINED, VariableSource::SR_INTERNAL, VariableMode::MD_READ, vPIN_INTERNAL_STATUS_DF, MQTT_TOPIC_INTERNAL_STATUS_DF);
    this->initializeVariable(Variable::INTERNAL_DEBUG, "Internal debug", VariableDatatype::DT_STRING, VariableUOM::UOM_UNDEFINED, VariableSource::SR_INTERNAL, VariableMode::MD_READ, vPIN_INTERNAL_DEBUG_TERMINAL_DF, MQTT_TOPIC_INTERNAL_DEBUG_TERMINAL_DF);
    this->initializeVariable(Variable::UPDATE_ALL_CONTROLLER_DATA, "Full refresh from scc", VariableDatatype::DT_BOOL, VariableUOM::UOM_UNDEFINED, VariableSource::SR_INTERNAL, VariableMode::MD_READWRITE, vPIN_UPDATE_ALL_CONTROLLER_DATA_DF, MQTT_TOPIC_UPDATE_ALL_CONTROLLER_DATA_DF);
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
    case VariableDatatype::DT_STRING:
        return 20;
    }
    return 0;
}


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

#include "project_config.h"


struct blynkSolarVariableMap{
  uint8_t virtualPin;
  SolarTracerVariables solarVariable;
};

const blynkSolarVariableMap realTimeVirtualBlynkSolarVariables [] = {
 #ifdef vPIN_PV_POWER
  {vPIN_PV_POWER, SolarTracerVariables::PV_POWER},
#endif
#ifdef vPIN_PV_CURRENT
  {vPIN_PV_CURRENT, SolarTracerVariables::PV_CURRENT},
#endif
#ifdef vPIN_PV_VOLTAGE
  {vPIN_PV_VOLTAGE, SolarTracerVariables::PV_VOLTAGE},
#endif
#ifdef vPIN_LOAD_CURRENT
  {vPIN_LOAD_CURRENT, SolarTracerVariables::LOAD_CURRENT},
#endif
#ifdef vPIN_LOAD_POWER
  {vPIN_LOAD_POWER, SolarTracerVariables::LOAD_POWER},
#endif
#ifdef vPIN_BATT_TEMP
  {vPIN_BATT_TEMP, SolarTracerVariables::BATTERY_TEMP},
#endif
#ifdef vPIN_BATT_VOLTAGE
  {vPIN_BATT_VOLTAGE, SolarTracerVariables::BATTERY_VOLTAGE},
#endif
#ifdef vPIN_BATT_REMAIN
  {vPIN_BATT_REMAIN, SolarTracerVariables::BATTERY_SOC},
#endif
#ifdef vPIN_CONTROLLER_TEMP
  {vPIN_CONTROLLER_TEMP, SolarTracerVariables::CONTROLLER_TEMP},
#endif
#ifdef vPIN_BATTERY_CHARGE_CURRENT
  {vPIN_BATTERY_CHARGE_CURRENT, SolarTracerVariables::BATTERY_CHARGE_CURRENT},
#endif
#ifdef vPIN_BATTERY_CHARGE_POWER
  {vPIN_BATTERY_CHARGE_POWER, SolarTracerVariables::BATTERY_CHARGE_POWER},
#endif
#ifdef vPIN_BATTERY_OVERALL_CURRENT
  {vPIN_BATTERY_OVERALL_CURRENT, SolarTracerVariables::BATTERY_OVERALL_CURRENT},
#endif
#ifdef vPIN_LOAD_ENABLED
  {vPIN_LOAD_ENABLED, SolarTracerVariables::LOAD_MANUAL_ONOFF},
#endif
#ifdef vPIN_CHARGE_DEVICE_ENABLED
  {vPIN_CHARGE_DEVICE_ENABLED, SolarTracerVariables::CHARGING_DEVICE_ONOFF},
#endif
#ifdef vPIN_BATTERY_STATUS_TEXT
  {vPIN_BATTERY_STATUS_TEXT, SolarTracerVariables::BATTERY_STATUS_TEXT},
#endif
#ifdef vPIN_CHARGING_EQUIPMENT_STATUS_TEXT
  {vPIN_CHARGING_EQUIPMENT_STATUS_TEXT, SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT},
#endif
#ifdef vPIN_DISCHARGING_EQUIPMENT_STATUS_TEXT
  {vPIN_DISCHARGING_EQUIPMENT_STATUS_TEXT, SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT},
#endif
};

const uint8_t  realTimeVirtualBlynkSolarVariablesCount = sizeof(realTimeVirtualBlynkSolarVariables) / sizeof(blynkSolarVariableMap);

const blynkSolarVariableMap statVirtualBlynkSolarVariables [] = {
#ifdef vPIN_STAT_ENERGY_GENERATED_TODAY
  {vPIN_STAT_ENERGY_GENERATED_TODAY, SolarTracerVariables::GENERATED_ENERGY_TODAY},
#endif
#ifdef vPIN_STAT_ENERGY_GENERATED_THIS_MONTH
  {vPIN_STAT_ENERGY_GENERATED_THIS_MONTH, SolarTracerVariables::GENERATED_ENERGY_MONTH},
#endif
#ifdef vPIN_STAT_ENERGY_GENERATED_THIS_YEAR
  {vPIN_STAT_ENERGY_GENERATED_THIS_YEAR, SolarTracerVariables::GENERATED_ENERGY_YEAR},
#endif
#ifdef vPIN_STAT_ENERGY_GENERATED_TOTAL
  {vPIN_STAT_ENERGY_GENERATED_TOTAL, SolarTracerVariables::GENERATED_ENERGY_TOTAL},
#endif
};

const uint8_t  statVirtualBlynkSolarVariablesCount = sizeof(statVirtualBlynkSolarVariables) / sizeof(blynkSolarVariableMap);

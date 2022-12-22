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

#ifndef LOAD_CURRENT_OVERWRITE_H
#define LOAD_CURRENT_OVERWRITE_H

#include "../../incl/include_all_core.h"

#ifdef USE_EXTERNAL_HEAVY_LOAD_CURRENT_METER

#include "../../core/Environment.h"
#include "../../incl/include_all_lib.h"
#include "../SolarTracer.h"

class LoadCurrentOverwrite
{
public:
    static void setup(SolarTracer *tracer)
    {

        totalLoadEnergy = 0;
        lastRunMillis = 0;

#ifdef USE_EXTERNAL_HEAVY_LOAD_CURRENT_METER_ADS1015_ADC
        ads1015 = new ADS1015(0x48);
        ads1015->begin();

        ads1015->setGain(0);

        sensor = new LinearSensHallCurrent(LinearSensHallCurrentType::CUSTOM, 0, 6.144, 11);
        sensor->setRawReadFn([]()
                             {
        int adp_value =  ads1015->readADC(ADS1015_LOAD_CURRENT_CHANNEL);
        return adp_value; });
#endif
#ifdef USE_EXTERNAL_HEAVY_LOAD_CURRENT_ADC
        sensor = new LinearSensHallCurrent(LinearSensHallCurrentType::CUSTOM, ADC_LOAD_CURRENT_PIN, 3.3, 12);
#endif

        sensor->setZeroVout(Environment::getData()->hlZeroVOff);
        sensor->setSampleNumber(EXTERNAL_HEAVY_LOAD_CURRENT_METER_SAMPLES_NUMBER);
        sensor->setSampleDelay(EXTERNAL_HEAVY_LOAD_CURRENT_METER_SAMPLE_INTERVAL);
        sensor->setVASensitivity(EXTERNAL_HEAVY_LOAD_CURRENT_METER_VOLTAGE_AMP_VOLT);

        tracer->setVariableEnabled(Variable::LOAD_CURRENT);
        tracer->setVariableEnable(Variable::LOAD_POWER, tracer->isVariableEnabled(Variable::BATTERY_VOLTAGE));
        tracer->setVariableEnable(Variable::CONSUMED_ENERGY_TOTAL, tracer->isVariableEnabled(Variable::BATTERY_VOLTAGE));
        tracer->setVariableEnable(Variable::BATTERY_OVERALL_CURRENT, tracer->isVariableEnabled(Variable::BATTERY_CHARGE_CURRENT));

        tracer->setVariableOverWritten(Variable::LOAD_CURRENT, true);
        tracer->setVariableOverWritten(Variable::LOAD_POWER, true);
        tracer->setVariableOverWritten(Variable::CONSUMED_ENERGY_TOTAL, true);
        tracer->setVariableOverWritten(Variable::BATTERY_OVERALL_CURRENT, true);
    }

    static void overWrite(SolarTracer *tracer)
    {
        float current = sensor->readCurrent();
        if (current > 1000)
        {
            // not happening
            return;
        }
        if (current < 0)
        {
            // loads do not produce any current, must be a false reading
            current = 0;
        }
        tracer->setVariableValue(Variable::LOAD_CURRENT, &current, true);

        if (tracer->isVariableReadReady(Variable::BATTERY_CHARGE_CURRENT))
        {
            const void *chargingCurrent = tracer->getValue(Variable::BATTERY_CHARGE_CURRENT);
            float overallCurrent = *(float *)chargingCurrent - current;
            tracer->setVariableValue(Variable::BATTERY_OVERALL_CURRENT, &overallCurrent, true);
        }

        const void *loadVoltage = tracer->getValue(Variable::BATTERY_VOLTAGE);

        if (!tracer->isVariableReadReady(Variable::BATTERY_VOLTAGE) || loadVoltage == nullptr)
        {
            return;
        }

        unsigned long currentRunMillis = millis();

        float loadPower = current * *((float *)loadVoltage);
        float loadEnergy = lastRunMillis > 0 ? (currentRunMillis - lastRunMillis) / 3600000000.0 * loadPower : 0;
        lastRunMillis = currentRunMillis;

        tracer->setVariableValue(Variable::LOAD_POWER, &loadPower, true);

        totalLoadEnergy += loadEnergy;
        tracer->setVariableValue(Variable::CONSUMED_ENERGY_TOTAL, &totalLoadEnergy, true);
    }

private:
    static float totalLoadEnergy;
    static LinearSensHallCurrent *sensor;
#ifdef USE_EXTERNAL_HEAVY_LOAD_CURRENT_METER_ADS1015_ADC
    static ADS1015 *ads1015;
#endif
    static unsigned long lastRunMillis;
};

#endif
#endif
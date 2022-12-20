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

#ifndef DUMMYSOLARTRACER_H
#define DUMMYSOLARTRACER_H

#include "../SolarTracer.h"
#include "../../core/VariableDefiner.h"

class DummySolarTracer : public SolarTracer
{
public:
    DummySolarTracer() : SolarTracer()
    {
        this->dummyTextValue = new char[20];

        bool enabled;
        for (uint8_t index = 0; index < Variable::VARIABLES_COUNT; index++)
        {
            if (VariableDefiner::getInstance().isFromScc((Variable)index))
            {
                this->setVariableEnabled((Variable)index);
                this->setVariableReadReady((Variable)index, true);
            }
        }
    };

    virtual bool fetchValue(Variable variable)
    {
        return true;
    };
    virtual bool syncRealtimeClock(struct tm *ti)
    {
        return true;
    }
    virtual void fetchAllValues(){};
    virtual bool updateRun()
    {
        return true;
    };
    virtual bool writeValue(Variable variable, const void *value)
    {
        return true;
    };
    virtual bool testConnection()
    {
        return true;
    };

    virtual const void *getValue(Variable variable)
    {
        switch (VariableDefiner::getInstance().getDatatype(variable))
        {
        case VariableDatatype::DT_BOOL:
            this->dummyBoolValue = random(2) > 0;
            return &this->dummyBoolValue;
        case VariableDatatype::DT_FLOAT:
            this->dummyFloatValue = random(10000) / 100.00;
            return &this->dummyFloatValue;
        case VariableDatatype::DT_STRING:
            strcpy(this->dummyTextValue, "Text");
            return this->dummyTextValue;
        case VariableDatatype::DT_UINT16:
            this->dummyUInt16 = random(100) > 0;
            return &this->dummyUInt16;
        }
        return nullptr;
    }

private:
    bool dummyBoolValue;
    float dummyFloatValue;
    char *dummyTextValue;
    uint16_t dummyUInt16;
};
#endif
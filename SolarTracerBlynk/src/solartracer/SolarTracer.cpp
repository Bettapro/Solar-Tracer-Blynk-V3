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

#include "SolarTracer.h"

SolarTracer::SolarTracer()
{
    this->variableDefine = new SolarTracerVariableDefinition[Variable::VARIABLES_COUNT]();
    const VariableDefinition *def;
    for (uint8_t varIndex = 0; varIndex < Variable::VARIABLES_COUNT; varIndex++)
    {
        def = VariableDefiner::getInstance().getDefinition((Variable)varIndex);
        if (def->source == VariableSource::SR_STATS || def->source == VariableSource::SR_REALTIME)
        {
            this->variableDefine[varIndex] = {0, def->datatype == VariableDatatype::DT_STRING ? new char[20] : nullptr, 0};
        }
    }
}

void SolarTracer::setVariableEnable(Variable variable, bool enable)
{
    if (variable < Variable::VARIABLES_COUNT && this->isVariableEnabled(variable) != enable)
    {
        this->variableDefine[variable].status += (enable ? 1 : -1);
    }
}

bool SolarTracer::isVariableEnabled(Variable variable)
{
    if (variable < Variable::VARIABLES_COUNT)
    {
        return this->variableDefine[variable].status & 1 > 0;
    }
    return false;
}

void SolarTracer::setVariableReadReady(Variable variable, bool enable)
{
    if (variable < Variable::VARIABLES_COUNT && this->isVariableReadReady(variable) != enable)
    {
        this->variableDefine[variable].status += (enable ? 1 : -1) * 2;
    }
}

void SolarTracer::setVariableReadReady(uint8_t count, bool ready, ...)
{
    va_list args;
    va_start(args, ready);

    for (uint8_t i = 1; i <= count; i++)
    {
        this->setVariableReadReady((Variable)va_arg(args, int), ready);
    }

    va_end(args);
}

bool SolarTracer::isVariableReadReady(Variable variable)
{
    if (variable < Variable::VARIABLES_COUNT)
    {
        return this->variableDefine[variable].status & 2 > 0;
    }
    return false;
}

const void *SolarTracer::getValue(Variable variable)
{
    if (!this->isVariableEnabled(variable))
    {
        return nullptr;
    }

    switch (VariableDefiner::getInstance().getDatatype(variable))
    {
    case VariableDatatype::DT_STRING:
        return this->variableDefine[variable].textValue;
        break;
    case VariableDatatype::DT_FLOAT:
        return &(this->variableDefine[variable].floatValue);
        break;
    }

    // should raise an exception?
    return nullptr;
}

bool SolarTracer::setVariableValue(Variable variable, const void *value)
{
    bool valueOk = value != nullptr;
    if (valueOk)
    {
        switch (VariableDefiner::getInstance().getDatatype(variable))
        {
        case VariableDatatype::DT_FLOAT:
        {
            this->variableDefine[variable].floatValue = *(float *)value;
        }
        break;
        case VariableDatatype::DT_STRING:
            // should specialize depending on the text
            strcpy(this->variableDefine[variable].textValue, (const char *)value);
            break;
        }
    }
    this->setVariableReadReady(variable, valueOk);

    return valueOk;
}

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

#include "BaseSync.h"

#include "../core/debug.h"
#include "../core/VariableDefiner.h"
#include "../core/Controller.h"


void BaseSync::applyUpdateToVariable(Variable variable, const void *value, bool silent)
{
    const VariableDefinition *def = VariableDefiner::getInstance().getDefinition(variable);
    if (def != nullptr)
    {
        SolarTracer *solarT = Controller::getInstance().getSolarController();
        if (!silent)
        {
            switch (def->datatype)
            {
            case VariableDatatype::DT_FLOAT:
            {
                debugPrintf("WRITE REQUEST : \"%s\" to %.4f", def->text, *(const float *)value);
            }
            case VariableDatatype::DT_BOOL:
            {
                debugPrintf("WRITE REQUEST : \"%s\" to %i", def->text, *(const bool *)value);
            }
            case VariableDatatype::DT_STRING:
            {
                debugPrintf("WRITE REQUEST : \"%s\" to \"%s\"", def->text, (const char *)value);
            }
            default:
                debugPrintf("WRITE REQUEST : \"%s\"", def->text);
                break;
            }
        }
        if (!solarT->writeValue(variable, value) && !silent)
        {
            debugPrintf(" - FAILED! [err=%i]", solarT->getLastControllerCommunicationStatus());
            debugPrintln();
        }
    }
}

uint8_t BaseSync::sendUpdateAllBySource(VariableSource allowedSource, bool silent)
{

    uint8_t varNotReady = 0;
    const VariableDefinition *def;
    int pin;
    SolarTracer *solarT = Controller::getInstance().getSolarController();
    for (uint8_t index = 0; index < Variable::VARIABLES_COUNT; index++)
    {
        def = VariableDefiner::getInstance().getDefinition((Variable)index);
        if (def->source == allowedSource && this->isVariableAllowed(def) && solarT->isVariableEnabled(def->variable))
        {
            if (!solarT->isVariableReadReady(def->variable) || !this->sendUpdateToVariable(def->variable, solarT->getValue(def->variable)))
            {
                varNotReady++;
            }
        }
    }

    switch (allowedSource)
    {
    case VariableSource::SR_STATS:
        if (!silent && varNotReady > 0)
        {
            debugPrintf("WARNING %i ST var. are not ready & synced!\r\n", varNotReady);
        }
        Controller::getInstance().setStatusFlag(STATUS_ERR_SOLAR_TRACER_NO_SYNC_ST, varNotReady > 0);
        break;
    case VariableSource::SR_REALTIME:
        if (!silent && varNotReady > 0)
        {
            debugPrintf("WARNING %i RT var. are not ready & synced!\r\n", varNotReady);
        }
        Controller::getInstance().setStatusFlag(STATUS_ERR_SOLAR_TRACER_NO_SYNC_RT, varNotReady > 0);
        break;
    }

    return varNotReady;
}
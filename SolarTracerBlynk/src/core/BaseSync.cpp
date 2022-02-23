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
    SolarTracer *solarT = Controller::getInstance().getSolarController();
    const VariableDefinition *def = VariableDefiner::getInstance().getDefinition(variable);
    if (def != nullptr)
    {

        if (!silent)
        {
            debugPrint("WRITE REQUEST ");
            switch (def->datatype)
            {
            case VariableDatatype::DT_FLOAT:
                debugPrintf(false, "\"%s\" to %.4f ", def->text, *(const float *)value);
                break;
            case VariableDatatype::DT_BOOL:
                debugPrintf(false, "\"%s\" to %i ", def->text, *(const bool *)value);
                break;
            case VariableDatatype::DT_STRING:
                debugPrintf(false, "\"%s\" to \"%s\" ", def->text, (const char *)value);
                break;
            }
        }
        bool writeDone = solarT->writeValue(variable, value);
        if (!silent)
        {
           writeDone ? debugPrintln(Text::ok) : debugPrintf(true, Text::errorWithCode, solarT->getLastControllerCommunicationStatus());
        }
    }
}

uint8_t BaseSync::sendUpdateAllBySource(VariableSource allowedSource, bool silent)
{
    SolarTracer *solarT = Controller::getInstance().getSolarController();
    uint8_t varNotReady = 0;
    const VariableDefinition *def;
    const char *sourceText = allowedSource == VariableSource::SR_STATS ? "ST" : "RT";
    uint8_t sourceFlag = allowedSource == VariableSource::SR_STATS ? STATUS_ERR_SOLAR_TRACER_NO_SYNC_ST : STATUS_ERR_SOLAR_TRACER_NO_SYNC_RT;

    for (uint8_t index = 0; index < Variable::VARIABLES_COUNT; index++)
    {
        def = VariableDefiner::getInstance().getDefinition((Variable)index);
        if (def->source == allowedSource && this->isVariableAllowed(def) && solarT->isVariableEnabled(def->variable))
        {
            if (!solarT->isVariableReadReady(def->variable) || !this->sendUpdateToVariable(def->variable, solarT->getValue(def->variable)))
            {
#ifdef USE_DEBUG_SERIAL_VERBOSE_SYNC_ERROR_VARIABLE
                debugPrintf(true, Text::syncErrorWithVariable, def->text);
#endif
                varNotReady++;
            }
        }
    }

    if (!silent && varNotReady > 0)
    {
        debugPrintf(true, Text::syncErrorWithCountAndType, varNotReady, sourceText);
    }
    Controller::getInstance().setErrorFlag(sourceFlag, varNotReady > 0);

    return varNotReady;
}
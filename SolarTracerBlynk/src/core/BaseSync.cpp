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

#include "../core/Controller.h"
#include "../core/VariableDefiner.h"
#include "../core/debug.h"

#define VALUE_CACHES_UNTIL_COUNT 100;

BaseSync::BaseSync() {
    this->renewValuesCount = new uint8_t[Variable::VARIABLES_COUNT]();
    this->lastValuesCache = new void *[Variable::VARIABLES_COUNT]();

    uint8_t varSize;
    for (uint8_t index = 0; index < Variable::VARIABLES_COUNT; index++) {
        this->renewValuesCount[index] = 0;
        varSize = VariableDefiner::getInstance().getVariableSize((Variable)index);
        if (varSize > 0) {
            this->lastValuesCache[index] = malloc(varSize);
            memset(this->lastValuesCache[index], 0, varSize);
        }
    }
}

void BaseSync::applyUpdateToVariable(Variable variable, const void *value, bool silent) {
    SolarTracer *solarT = Controller::getInstance().getSolarController();
    const VariableDefinition *def = VariableDefiner::getInstance().getDefinition(variable);
    if (def != nullptr) {
        if (!silent) {
            debugPrint("WRITE REQUEST ");
            switch (def->datatype) {
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
        if (!silent) {
            writeDone ? debugPrintln(Text::ok) : debugPrintf(true, Text::errorWithCode, solarT->getLastControllerCommunicationStatus());
        }
    }
}

uint8_t BaseSync::sendUpdateAllBySource(VariableSource allowedSource, bool silent) {
    SolarTracer *solarT = Controller::getInstance().getSolarController();
    uint8_t varNotReady = 0;
    const VariableDefinition *def;

    for (uint8_t index = 0; index < Variable::VARIABLES_COUNT; index++) {
        def = VariableDefiner::getInstance().getDefinition((Variable)index);
        if (def->source == allowedSource && this->isVariableAllowed(def) && (solarT->isVariableEnabled(def->variable) || solarT->isVariableOverWritten(def->variable))) {
            if (!solarT->isVariableReadReady(def->variable) || !this->syncVariable(def, solarT->getValue(def->variable))) {
#ifdef USE_DEBUG_SERIAL_VERBOSE_SYNC_ERROR_VARIABLE
                debugPrintf(true, Text::syncErrorWithVariable, def->text);
#endif
                varNotReady++;
            }
        }
    }

    if (!silent && varNotReady > 0) {
        debugPrintf(true, Text::syncErrorWithCountAndType, varNotReady, allowedSource == VariableSource::SR_STATS ? "ST" : "RT");
    }
    Controller::getInstance().setErrorFlag(allowedSource == VariableSource::SR_STATS ? STATUS_ERR_SOLAR_TRACER_NO_SYNC_ST : STATUS_ERR_SOLAR_TRACER_NO_SYNC_RT, varNotReady > 0);

    return varNotReady;
}

bool BaseSync::syncVariable(const VariableDefinition *def, const void *value) {
    uint8_t *cachedUntil = &(this->renewValuesCount[def->variable]);
    if (this->isVariableAllowed(def)) {
        if (!(
                this->renewValueCount == 1 || (*cachedUntil) <= 0                                                            // sync always enabled?
                || !VariableDefiner::getInstance().isValueEqual(def->variable, value, this->lastValuesCache[def->variable])  // value has changed?
                || (this->renewValueCount > 1 && ((*cachedUntil)--) <= 0)                                                    // renew required?
                )                                                                                                            // -> should sync evaluation?
            || this->sendUpdateToVariable(def, value)) {
            memcpy(this->lastValuesCache[def->variable], value, VariableDefiner::getInstance().getVariableSize(def->variable));
            if (this->renewValueCount > 1 || (*cachedUntil) <= 0) {
                (*cachedUntil) = this->renewValueCount + 1;
            }
            return true;
        }
    }
    return false;
}
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

#ifndef SOLARTRACER_H
#define SOLARTRACER_H

#include "../core/VariableDefiner.h"

typedef void (*OnUpdateRunCompletedCallback)();

struct SolarTracerVariableDefinition {
        /**
         *  byte 0 - enabled
         *  byte 1 - ready
         *  byte 2 - overwritten
         */
        uint8_t status;
        void *value;
};

class SolarTracer {
    public:
        SolarTracer();

        /**
         * Check if the variable is supporter by the SCC
         */
        bool isVariableEnabled(Variable variable);

        /**
         * Check if variable is ready to be read (value has been fetched from the scc)
         */
        bool isVariableReadReady(Variable variable);

        /**
         * Check if variable has been overwritten
         */
        bool isVariableOverWritten(Variable variable);

        /**
         * Set variable as overwritten (not managed by the scc)
         */
        void setVariableOverWritten(Variable variable, bool enable);

        /**
         * Get the value of the variable, null if variable is not ready
         */
        virtual const void *getValue(Variable variable);

        /**
         * Set the value of the variable
         */
        bool setVariableValue(Variable variable, const void *value, bool ignoreOverWriteLock = false);

        void setOnUpdateRunCompleted(OnUpdateRunCompletedCallback fn) {
            this->onUpdateRunCompleted = fn;
        }

        /**
         * Return last status code, 0 means the controller is responding to requests correctly.
         */
        inline const uint16_t getLastControllerCommunicationStatus();

        virtual bool fetchValue(Variable variable) = 0;
        virtual bool syncRealtimeClock(struct tm *ti) = 0;
        virtual void fetchAllValues() = 0;
        virtual bool updateRun() = 0;
        virtual bool writeValue(Variable variable, const void *value) = 0;
        virtual bool testConnection() = 0;

    protected:
        inline bool setFloatVariable(Variable variable, float value);

        void updateRunCompleted() {
            if (this->onUpdateRunCompleted) {
                this->onUpdateRunCompleted();
            }
        }

        void setVariableEnable(Variable variable, bool enable = true);

        void setVariableReadReady(Variable variable, bool enable);

        void setVariableReadReady(uint8_t count, bool ready, ...);

        uint16_t lastControllerCommunicationStatus;

    private:
        OnUpdateRunCompletedCallback onUpdateRunCompleted = nullptr;
        SolarTracerVariableDefinition **variableDefine;
};

inline const uint16_t SolarTracer::getLastControllerCommunicationStatus() {
    return this->lastControllerCommunicationStatus;
}

inline bool SolarTracer::setFloatVariable(Variable variable, float value) {
    return this->setVariableValue(variable, &value);
}

#endif

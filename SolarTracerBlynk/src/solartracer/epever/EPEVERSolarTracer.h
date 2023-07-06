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

#ifndef EPEVERSolarTracer_h
#define EPEVERSolarTracer_h

#include <ModbusMaster.h>
#include <ModbusMasterCallable.h>
#include <time.h>

#include "../SolarTracer.h"

class EPEVERSolarTracer : public SolarTracer, public ModbusMasterCallable {
    public:
        EPEVERSolarTracer(Stream &serialCom, uint16_t serialTimeoutMs, uint8_t slave, uint8_t max485_de, uint8_t max485_re_neg, uint16_t preTransmitWait);
        EPEVERSolarTracer(Stream &serialCom, uint16_t serialTimeoutMs, uint8_t slave, uint16_t preTransmitWait);

        virtual bool syncRealtimeClock(struct tm *ti);

        virtual void fetchAllValues();

        virtual bool updateRun();

        virtual bool fetchValue(Variable variable);

        virtual bool writeValue(Variable variable, const void *value);

        bool readControllerSingleCoil(uint16_t address);

        void AddressRegistry_3100();

        void AddressRegistry_3110();

        void AddressRegistry_311A();

        void AddressRegistry_331B();

        void AddressRegistry_9003();

        void AddressRegistry_9067();

        void AddressRegistry_906B();

        void fetchAddressStatusVariables();

        void updateStats();

        /*
             Implementation of ModbusMasterCallable
          */
        virtual void onModbusPreTransmission();

        virtual void onModbusIdle();

        virtual void onModbusPostTransmission();

        virtual bool testConnection();

    protected:
        uint8_t max485_re_neg, max485_de;
        uint16_t preTransmitWaitMs;

        bool rs485readSuccess;

        uint16_t globalUpdateCounter = 0;
        uint8_t currentRealtimeUpdateCounter = 0;

        ModbusMaster node;

        // MODBUS FUNCTION

        bool writeControllerSingleCoil(uint16_t address, bool value);
        bool writeControllerHoldingRegister(uint16_t address, uint16_t value);
        bool replaceControllerHoldingRegister(uint16_t address, uint16_t value, uint16_t fromAddress, uint8_t count);

        static constexpr const float ONE_HUNDRED_FLOAT = 100;

    private:
        static const uint8_t voltageLevels[];

        void onPreNodeRequest() {
            if (this->preTransmitWaitMs > 0) {
                delay(this->preTransmitWaitMs);
            }
        }

        static uint16_t getBatteryVoltageLevelFromVoltage(uint16_t voltage) {
            uint8_t index = 0;
            while (true) {
                if (voltageLevels[index] == 0) {
                    return 0;
                }
                if (voltageLevels[index] == voltage) {
                    return index + 1;
                }
                index++;
            }
        }

        static uint16_t getVoltageFromBatteryVoltageLevel(uint16_t index) {
            return voltageLevels[index];
        }
};

#endif

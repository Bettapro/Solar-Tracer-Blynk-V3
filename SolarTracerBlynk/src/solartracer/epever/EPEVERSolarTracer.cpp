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

#include "EPEVERSolarTracer.h"

#include "EPEVER_modbus_address.h"

const uint8_t EPEVERSolarTracer::voltageLevels[] = {12, 24, 36, 48, 60, 110, 120, 220, 240, 0};

#define _EST_RS_POINTER(s, v) (s ? &v : nullptr)

EPEVERSolarTracer::EPEVERSolarTracer(Stream &serialCom, uint16_t serialTimeoutMs, uint8_t slave, uint8_t max485_de, uint8_t max485_re_neg, uint16_t preTransmitWait)
    : EPEVERSolarTracer(serialCom, serialTimeoutMs, slave, preTransmitWait) {
    this->max485_re_neg = max485_re_neg;
    this->max485_de = max485_de;

    pinMode(this->max485_re_neg, OUTPUT);
    pinMode(this->max485_de, OUTPUT);

    digitalWrite(this->max485_re_neg, 0);
    digitalWrite(this->max485_de, 0);

    // set this instance as the callback receiver
    this->node.setTransmissionCallable(this);
}

EPEVERSolarTracer::EPEVERSolarTracer(Stream &serialCom, uint16_t serialTimeoutMs, uint8_t slave, uint16_t preTransmitWait)
    : SolarTracer() {
    this->node.begin(slave, serialCom);

    this->max485_re_neg = this->max485_de = 0;
    this->preTransmitWaitMs = preTransmitWait;

    this->rs485readSuccess = true;

    if (serialTimeoutMs > 0) {
        this->node.setResponseTimeout(serialTimeoutMs);
    };

    // set enabled variables
    this->setVariableEnable(Variable::PV_POWER);
    this->setVariableEnable(Variable::PV_CURRENT);
    this->setVariableEnable(Variable::PV_VOLTAGE);
    this->setVariableEnable(Variable::LOAD_CURRENT);
    this->setVariableEnable(Variable::LOAD_POWER);
    this->setVariableEnable(Variable::BATTERY_TEMP);
    this->setVariableEnable(Variable::BATTERY_VOLTAGE);
    this->setVariableEnable(Variable::BATTERY_SOC);
    this->setVariableEnable(Variable::CONTROLLER_TEMP);
    this->setVariableEnable(Variable::BATTERY_CHARGE_CURRENT);
    this->setVariableEnable(Variable::BATTERY_CHARGE_POWER);
    this->setVariableEnable(Variable::BATTERY_OVERALL_CURRENT);
    this->setVariableEnable(Variable::LOAD_MANUAL_ONOFF);
    this->setVariableEnable(Variable::CHARGING_DEVICE_ONOFF);
    this->setVariableEnable(Variable::BATTERY_STATUS_TEXT);
    this->setVariableEnable(Variable::CHARGING_EQUIPMENT_STATUS_TEXT);
    this->setVariableEnable(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT);
    this->setVariableEnable(Variable::REMOTE_BATTERY_TEMP);
    this->setVariableEnable(Variable::GENERATED_ENERGY_TODAY);
    this->setVariableEnable(Variable::GENERATED_ENERGY_MONTH);
    this->setVariableEnable(Variable::GENERATED_ENERGY_YEAR);
    this->setVariableEnable(Variable::GENERATED_ENERGY_TOTAL);
    this->setVariableEnable(Variable::MAXIMUM_PV_VOLTAGE_TODAY);
    this->setVariableEnable(Variable::MINIMUM_PV_VOLTAGE_TODAY);
    this->setVariableEnable(Variable::MAXIMUM_BATTERY_VOLTAGE_TODAY);
    this->setVariableEnable(Variable::MINIMUM_BATTERY_VOLTAGE_TODAY);
    this->setVariableEnable(Variable::HEATSINK_TEMP);
    this->setVariableEnable(Variable::BATTERY_BOOST_VOLTAGE);
    this->setVariableEnable(Variable::BATTERY_EQUALIZATION_VOLTAGE);
    this->setVariableEnable(Variable::BATTERY_FLOAT_VOLTAGE);
    this->setVariableEnable(Variable::BATTERY_FLOAT_MIN_VOLTAGE);
    this->setVariableEnable(Variable::BATTERY_CHARGING_LIMIT_VOLTAGE);
    this->setVariableEnable(Variable::BATTERY_DISCHARGING_LIMIT_VOLTAGE);
    this->setVariableEnable(Variable::BATTERY_LOW_VOLTAGE_DISCONNECT);
    this->setVariableEnable(Variable::BATTERY_LOW_VOLTAGE_RECONNECT);
    this->setVariableEnable(Variable::BATTERY_OVER_VOLTAGE_DISCONNECT);
    this->setVariableEnable(Variable::BATTERY_OVER_VOLTAGE_RECONNECT);
    this->setVariableEnable(Variable::BATTERY_UNDER_VOLTAGE_SET);
    this->setVariableEnable(Variable::BATTERY_UNDER_VOLTAGE_RESET);
    this->setVariableEnable(Variable::BATTERY_OVER_VOLTAGE_RECONNECT);
    this->setVariableEnable(Variable::BATTERY_UNDER_VOLTAGE_SET);
    this->setVariableEnable(Variable::BATTERY_UNDER_VOLTAGE_RESET);
    this->setVariableEnable(Variable::BATTERY_TYPE);
    this->setVariableEnable(Variable::BATTERY_CAPACITY);
    this->setVariableEnable(Variable::BATTERY_TEMPERATURE_COMPENSATION_COEFFICIENT);

    this->setVariableEnable(Variable::BATTERY_MANAGEMENT_MODE);
    this->setVariableEnable(Variable::BATTERY_RATED_VOLTAGE);
    this->setVariableEnable(Variable::BATTERY_BOOST_DURATION);
    this->setVariableEnable(Variable::BATTERY_EQUALIZATION_DURATION);
}

bool EPEVERSolarTracer::testConnection() {
    this->readControllerSingleCoil(MODBUS_ADDRESS_LOAD_MANUAL_ONOFF);
    return rs485readSuccess;
}

bool EPEVERSolarTracer::syncRealtimeClock(struct tm *ti) {
    node.setTransmitBuffer(0, (ti->tm_min << 8) + ti->tm_sec);
    node.setTransmitBuffer(1, (ti->tm_mday << 8) + ti->tm_hour);
    node.setTransmitBuffer(2, ((ti->tm_year + 1900 - 2000) << 8) + ti->tm_mon + 1);

    this->onPreNodeRequest();
    this->lastControllerCommunicationStatus = node.writeMultipleRegisters(MODBUS_ADDRESS_REALTIME_CLOCK, 3);
    if (this->lastControllerCommunicationStatus == this->node.ku8MBSuccess) {
        this->node.getResponseBuffer(0x00);
        return true;
    }
    return false;
};

void EPEVERSolarTracer::fetchAllValues() {
    // STATS
    this->updateStats();
    this->AddressRegistry_9003();
    this->AddressRegistry_9067();
    this->AddressRegistry_906B();
    // REALTIME
    this->AddressRegistry_3100();
    this->AddressRegistry_3110();
    this->AddressRegistry_311A();
    this->AddressRegistry_331B();
    this->fetchValue(Variable::LOAD_MANUAL_ONOFF);
    this->fetchValue(Variable::CHARGING_DEVICE_ONOFF);
    this->fetchAddressStatusVariables();

    // update run completed
    this->updateRunCompleted();
}

bool EPEVERSolarTracer::updateRun() {
    switch (globalUpdateCounter) {
        case 360:
            // update statistics
            this->updateStats();
            this->AddressRegistry_9003();
            this->AddressRegistry_9067();
            this->AddressRegistry_906B();
            globalUpdateCounter = 0;
            break;
        default:
            switch (currentRealtimeUpdateCounter) {
                case 0:
                    globalUpdateCounter++;
                    currentRealtimeUpdateCounter = 5;
                    this->AddressRegistry_3100();
                    break;
                case 1:
                    this->AddressRegistry_3110();
                    break;
                case 2:
                    this->AddressRegistry_311A();
                    break;
                case 3:
                    this->AddressRegistry_331B();
                    break;
                case 4:
                    this->fetchValue(Variable::LOAD_MANUAL_ONOFF);
                    this->fetchValue(Variable::CHARGING_DEVICE_ONOFF);
                    this->fetchAddressStatusVariables();

                    // update run completed
                    this->updateRunCompleted();
            }
            currentRealtimeUpdateCounter--;
    }
    return rs485readSuccess;
}

bool EPEVERSolarTracer::fetchValue(Variable variable) {
    bool value;
    switch (variable) {
        case Variable::LOAD_FORCE_ONOFF:
            // seems not doing anything
            value = this->readControllerSingleCoil(MODBUS_ADDRESS_LOAD_FORCE_ONOFF);
            return this->setVariableValue(variable, _EST_RS_POINTER(rs485readSuccess, value));
        case Variable::LOAD_MANUAL_ONOFF:
            value = this->readControllerSingleCoil(MODBUS_ADDRESS_LOAD_MANUAL_ONOFF);
            return this->setVariableValue(variable, _EST_RS_POINTER(rs485readSuccess, value));
        case Variable::CHARGING_DEVICE_ONOFF:
            value = this->readControllerSingleCoil(MODBUS_ADDRESS_BATTERY_CHARGE_ONOFF);
            return this->setVariableValue(variable, _EST_RS_POINTER(rs485readSuccess, value));
        default:
            break;
    }

    return false;
}

bool EPEVERSolarTracer::writeValue(Variable variable, const void *value) {
    if (!this->isVariableEnabled(variable) || this->isVariableOverWritten(variable)) {
        return false;
    }
    /**
     * 0x9000 - 0x900E
     *
     * https://diysolarforum.com/threads/setting-registers-in-epever-controller-with-python.25406/
     * Some holding registers cannot be set directly - error 04, it's mandatory to write a block of
     * xx registers at once - direct write not allowed on single holding register
     *
     **/

    bool writeResult = false;
    switch (variable) {
        case Variable::LOAD_FORCE_ONOFF:
            writeResult = this->writeControllerSingleCoil(MODBUS_ADDRESS_LOAD_FORCE_ONOFF, *(bool *)value);
            break;
        case Variable::LOAD_MANUAL_ONOFF:
            writeResult = this->writeControllerSingleCoil(MODBUS_ADDRESS_LOAD_MANUAL_ONOFF, *(bool *)value);
            break;
        case Variable::CHARGING_DEVICE_ONOFF:
            writeResult = this->writeControllerSingleCoil(MODBUS_ADDRESS_BATTERY_CHARGE_ONOFF, *(bool *)value);
            break;
            //
        case Variable::BATTERY_RATED_VOLTAGE:
            writeResult = this->writeControllerHoldingRegister(MODBUS_ADDRESS_BATTERY_RATED_LEVEL, EPEVERSolarTracer::getBatteryVoltageLevelFromVoltage(*(uint16_t *)value));
            break;
        case Variable::BATTERY_MANAGEMENT_MODE:
            writeResult = this->writeControllerHoldingRegister(MODBUS_ADDRESS_CHARGING_MODE, (*(uint16_t *)value));
            break;
        case Variable::BATTERY_EQUALIZATION_DURATION:
            writeResult = this->writeControllerHoldingRegister(MODBUS_ADDRESS_EQUALIZE_DURATION, (*(uint16_t *)value));
            break;
        case Variable::BATTERY_BOOST_DURATION:
            writeResult = this->writeControllerHoldingRegister(MODBUS_ADDRESS_BOOST_DURATION, (*(uint16_t *)value));
            break;
            //
        case Variable::BATTERY_TYPE:
            writeResult = this->replaceControllerHoldingRegister(MODBUS_ADDRESS_BATTERY_TYPE, (*(uint16_t *)value), MODBUS_ADDRESS_BATTERY_TYPE, 15);
            break;
        case Variable::BATTERY_CAPACITY:
            writeResult = this->replaceControllerHoldingRegister(MODBUS_ADDRESS_BATTERY_CAPACITY, (*(uint16_t *)value), MODBUS_ADDRESS_BATTERY_TYPE, 15);
            break;
        case Variable::BATTERY_TEMPERATURE_COMPENSATION_COEFFICIENT:
            writeResult = this->replaceControllerHoldingRegister(MODBUS_ADDRESS_BATTERY_TEMP_COEFF, (*(float *)value) * ONE_HUNDRED_FLOAT, MODBUS_ADDRESS_BATTERY_TYPE, 15);
            break;
        case Variable::BATTERY_OVER_VOLTAGE_DISCONNECT:
            writeResult = this->replaceControllerHoldingRegister(MODBUS_ADDRESS_HIGH_VOLTAGE_DISCONNECT, (*(float *)value) * ONE_HUNDRED_FLOAT, MODBUS_ADDRESS_BATTERY_TYPE, 15);
            break;
        case Variable::BATTERY_CHARGING_LIMIT_VOLTAGE:
            writeResult = this->replaceControllerHoldingRegister(MODBUS_ADDRESS_CHARGING_LIMIT_VOLTAGE, (*(float *)value) * ONE_HUNDRED_FLOAT, MODBUS_ADDRESS_BATTERY_TYPE, 15);
            break;
        case Variable::BATTERY_OVER_VOLTAGE_RECONNECT:
            writeResult = this->replaceControllerHoldingRegister(MODBUS_ADDRESS_OVER_VOLTAGE_RECONNECT, (*(float *)value) * ONE_HUNDRED_FLOAT, MODBUS_ADDRESS_BATTERY_TYPE, 15);
            break;
        case Variable::BATTERY_EQUALIZATION_VOLTAGE:
            writeResult = this->replaceControllerHoldingRegister(MODBUS_ADDRESS_EQUALIZATION_VOLTAGE, (*(float *)value) * ONE_HUNDRED_FLOAT, MODBUS_ADDRESS_BATTERY_TYPE, 15);
            break;
        case Variable::BATTERY_BOOST_VOLTAGE:
            writeResult = this->replaceControllerHoldingRegister(MODBUS_ADDRESS_BOOST_VOLTAGE, (*(float *)value) * ONE_HUNDRED_FLOAT, MODBUS_ADDRESS_BATTERY_TYPE, 15);
            break;
        case Variable::BATTERY_FLOAT_VOLTAGE:
            writeResult = this->replaceControllerHoldingRegister(MODBUS_ADDRESS_FLOAT_VOLTAGE, (*(float *)value) * ONE_HUNDRED_FLOAT, MODBUS_ADDRESS_BATTERY_TYPE, 15);
            break;
        case Variable::BATTERY_FLOAT_MIN_VOLTAGE:
            writeResult = this->replaceControllerHoldingRegister(MODBUS_ADDRESS_BOOST_RECONNECT_VOLTAGE, (*(float *)value) * ONE_HUNDRED_FLOAT, MODBUS_ADDRESS_BATTERY_TYPE, 15);
            break;
        case Variable::BATTERY_LOW_VOLTAGE_RECONNECT:
            writeResult = this->replaceControllerHoldingRegister(MODBUS_ADDRESS_LOW_VOLTAGE_RECONNECT, (*(float *)value) * ONE_HUNDRED_FLOAT, MODBUS_ADDRESS_BATTERY_TYPE, 15);
            break;
        case Variable::BATTERY_UNDER_VOLTAGE_RESET:
            writeResult = this->replaceControllerHoldingRegister(MODBUS_ADDRESS_UNDER_VOLTAGE_RECOVER, (*(float *)value) * ONE_HUNDRED_FLOAT, MODBUS_ADDRESS_BATTERY_TYPE, 15);
            break;
        case Variable::BATTERY_UNDER_VOLTAGE_SET:
            writeResult = this->replaceControllerHoldingRegister(MODBUS_ADDRESS_UNDER_VOLTAGE_WARNING, (*(float *)value) * ONE_HUNDRED_FLOAT, MODBUS_ADDRESS_BATTERY_TYPE, 15);
            break;
        case Variable::BATTERY_LOW_VOLTAGE_DISCONNECT:
            writeResult = this->replaceControllerHoldingRegister(MODBUS_ADDRESS_LOW_VOLTAGE_DISCONNECT, (*(float *)value) * ONE_HUNDRED_FLOAT, MODBUS_ADDRESS_BATTERY_TYPE, 15);
            break;
        case Variable::BATTERY_DISCHARGING_LIMIT_VOLTAGE:
            writeResult = this->replaceControllerHoldingRegister(MODBUS_ADDRESS_DISCHARGING_LIMIT_VOLTAGE, (*(float *)value) * ONE_HUNDRED_FLOAT, MODBUS_ADDRESS_BATTERY_TYPE, 15);
            break;
    }

    return writeResult ? this->setVariableValue(variable, value) : false;
}

bool EPEVERSolarTracer::readControllerSingleCoil(uint16_t address) {
    this->onPreNodeRequest();
    this->lastControllerCommunicationStatus = this->node.readCoils(address, 1);

    rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
    if (rs485readSuccess) {
        return (node.getResponseBuffer(0x00) > 0);
    }
    return false;
}

bool EPEVERSolarTracer::writeControllerSingleCoil(uint16_t address, bool value) {
    this->onPreNodeRequest();
    this->lastControllerCommunicationStatus = this->node.writeSingleCoil(address, value);

    if (this->lastControllerCommunicationStatus == this->node.ku8MBSuccess) {
        this->node.getResponseBuffer(0x00);
        return true;
    }
    return false;
}

bool EPEVERSolarTracer::writeControllerHoldingRegister(uint16_t address, uint16_t value) {
    this->onPreNodeRequest();
    this->lastControllerCommunicationStatus = this->node.writeSingleRegister(address, value);

    if (this->lastControllerCommunicationStatus == this->node.ku8MBSuccess) {
        this->node.getResponseBuffer(0x00);
        return true;
    }
    return false;
}

bool EPEVERSolarTracer::replaceControllerHoldingRegister(uint16_t address, uint16_t value, uint16_t fromAddress, uint8_t count) {
    this->onPreNodeRequest();
    this->lastControllerCommunicationStatus = this->node.readHoldingRegisters(fromAddress, count);
    if (this->lastControllerCommunicationStatus == this->node.ku8MBSuccess) {
        // adding a delay here could help, epever tends to trigger timeouts
        for (uint8_t i = 0; i < count; i++) {
            this->node.setTransmitBuffer(i, this->node.getResponseBuffer(i));
        }
        this->node.setTransmitBuffer(0, 0);
        this->node.setTransmitBuffer(address - fromAddress, value);

        this->lastControllerCommunicationStatus = this->node.writeMultipleRegisters(fromAddress, count);
        return this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
    }
    return false;
}

void EPEVERSolarTracer::AddressRegistry_3100() {
    this->onPreNodeRequest();
    this->lastControllerCommunicationStatus = this->node.readInputRegisters(MODBUS_ADDRESS_PV_VOLTAGE, 16);

    rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
    if (rs485readSuccess) {
        this->setFloatVariable(Variable::PV_VOLTAGE, this->node.getResponseBuffer(0x00) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::PV_CURRENT, this->node.getResponseBuffer(0x01) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::PV_POWER, (this->node.getResponseBuffer(0x02) | this->node.getResponseBuffer(0x03) << 16) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::BATTERY_VOLTAGE, this->node.getResponseBuffer(0x04) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::BATTERY_CHARGE_CURRENT, this->node.getResponseBuffer(0x05) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::BATTERY_CHARGE_POWER, (this->node.getResponseBuffer(0x06) | this->node.getResponseBuffer(0x07) << 16) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::LOAD_CURRENT, this->node.getResponseBuffer(0x0D) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::LOAD_POWER, (this->node.getResponseBuffer(0x0E) | this->node.getResponseBuffer(0x0F) << 16) / ONE_HUNDRED_FLOAT);
        return;
    }

    this->setVariableReadReady(8, rs485readSuccess,
                               Variable::PV_VOLTAGE,
                               Variable::PV_CURRENT,
                               Variable::PV_POWER,
                               Variable::BATTERY_VOLTAGE,
                               Variable::BATTERY_CHARGE_CURRENT,
                               Variable::BATTERY_CHARGE_POWER,
                               Variable::LOAD_CURRENT,
                               Variable::LOAD_POWER);
}

void EPEVERSolarTracer::AddressRegistry_3110() {
    this->onPreNodeRequest();
    // 0x3114,0x3115 -> returns modbus error code 2, must use AddressRegistry_311A
    this->lastControllerCommunicationStatus = this->node.readInputRegisters(MODBUS_ADDRESS_BATT_TEMP, 3);

    rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
    if (rs485readSuccess) {
        this->setFloatVariable(Variable::BATTERY_TEMP, this->node.getResponseBuffer(0x00) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::CONTROLLER_TEMP, this->node.getResponseBuffer(0x01) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::HEATSINK_TEMP, this->node.getResponseBuffer(0x02) / ONE_HUNDRED_FLOAT);

        return;
    }

    this->setVariableReadReady(3, rs485readSuccess,
                               Variable::BATTERY_TEMP,
                               Variable::CONTROLLER_TEMP,
                               Variable::HEATSINK_TEMP);
}

void EPEVERSolarTracer::AddressRegistry_311A() {
    this->onPreNodeRequest();
    this->lastControllerCommunicationStatus = this->node.readInputRegisters(MODBUS_ADDRESS_BATT_SOC, 2);

    rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
    if (rs485readSuccess) {
        this->setFloatVariable(Variable::BATTERY_SOC, this->node.getResponseBuffer(0x00) / 1.0f);
        this->setFloatVariable(Variable::REMOTE_BATTERY_TEMP, this->node.getResponseBuffer(0x01) / ONE_HUNDRED_FLOAT);

        return;
    }

    this->setVariableReadReady(2, rs485readSuccess,
                               Variable::BATTERY_SOC,
                               Variable::REMOTE_BATTERY_TEMP);
}

void EPEVERSolarTracer::AddressRegistry_331B() {
    this->onPreNodeRequest();
    this->lastControllerCommunicationStatus = this->node.readInputRegisters(0x331B, 2);

    rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
    if (rs485readSuccess) {
        this->setFloatVariable(Variable::BATTERY_OVERALL_CURRENT, (this->node.getResponseBuffer(0x00) | this->node.getResponseBuffer(0x01) << 16) / ONE_HUNDRED_FLOAT);
    }

    this->setVariableReadReady(Variable::BATTERY_OVERALL_CURRENT, rs485readSuccess);
}

void EPEVERSolarTracer::AddressRegistry_9003() {
    this->onPreNodeRequest();
    this->lastControllerCommunicationStatus = this->node.readHoldingRegisters(MODBUS_ADDRESS_BATTERY_TYPE, 15);

    rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
    if (rs485readSuccess) {
        uint16_t sharedUInt16 = this->node.getResponseBuffer(0x00);
        this->setVariableValue(Variable::BATTERY_TYPE, &sharedUInt16);
        sharedUInt16 = this->node.getResponseBuffer(0x01);
        this->setVariableValue(Variable::BATTERY_CAPACITY, &sharedUInt16);
        this->setFloatVariable(Variable::BATTERY_TEMPERATURE_COMPENSATION_COEFFICIENT, this->node.getResponseBuffer(0x02) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::BATTERY_OVER_VOLTAGE_DISCONNECT, this->node.getResponseBuffer(0x03) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::BATTERY_CHARGING_LIMIT_VOLTAGE, this->node.getResponseBuffer(0x04) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::BATTERY_OVER_VOLTAGE_RECONNECT, this->node.getResponseBuffer(0x05) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::BATTERY_EQUALIZATION_VOLTAGE, this->node.getResponseBuffer(0x06) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::BATTERY_BOOST_VOLTAGE, this->node.getResponseBuffer(0x07) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::BATTERY_FLOAT_VOLTAGE, this->node.getResponseBuffer(0x08) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::BATTERY_FLOAT_MIN_VOLTAGE, this->node.getResponseBuffer(0x09) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::BATTERY_LOW_VOLTAGE_RECONNECT, this->node.getResponseBuffer(0x0A) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::BATTERY_UNDER_VOLTAGE_RESET, this->node.getResponseBuffer(0x0B) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::BATTERY_UNDER_VOLTAGE_SET, this->node.getResponseBuffer(0x0C) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::BATTERY_LOW_VOLTAGE_DISCONNECT, this->node.getResponseBuffer(0x0D) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::BATTERY_DISCHARGING_LIMIT_VOLTAGE, this->node.getResponseBuffer(0x0E) / ONE_HUNDRED_FLOAT);
        return;
    }

    this->setVariableReadReady(15, rs485readSuccess,
                               Variable::BATTERY_TYPE,
                               Variable::BATTERY_CAPACITY,
                               Variable::BATTERY_TEMPERATURE_COMPENSATION_COEFFICIENT,
                               Variable::BATTERY_OVER_VOLTAGE_DISCONNECT,
                               Variable::BATTERY_CHARGING_LIMIT_VOLTAGE,
                               Variable::BATTERY_OVER_VOLTAGE_RECONNECT,
                               Variable::BATTERY_EQUALIZATION_VOLTAGE,
                               Variable::BATTERY_BOOST_VOLTAGE,
                               Variable::BATTERY_FLOAT_VOLTAGE,
                               Variable::BATTERY_FLOAT_MIN_VOLTAGE,
                               Variable::BATTERY_LOW_VOLTAGE_RECONNECT,
                               Variable::BATTERY_UNDER_VOLTAGE_RESET,
                               Variable::BATTERY_UNDER_VOLTAGE_SET,
                               Variable::BATTERY_LOW_VOLTAGE_DISCONNECT,
                               Variable::BATTERY_DISCHARGING_LIMIT_VOLTAGE);
}

void EPEVERSolarTracer::AddressRegistry_9067() {
    this->onPreNodeRequest();
    // cannot read 9067-9070 -> error illegal data address, must read single 9067
    this->lastControllerCommunicationStatus = this->node.readHoldingRegisters(MODBUS_ADDRESS_BATTERY_RATED_LEVEL, 1);

    rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
    if (rs485readSuccess) {
        uint16_t sharedUInt16 = EPEVERSolarTracer::getVoltageFromBatteryVoltageLevel(this->node.getResponseBuffer(0x00));
        this->setVariableValue(Variable::BATTERY_RATED_VOLTAGE, &sharedUInt16);

        return;
    }

    this->setVariableReadReady(Variable::BATTERY_RATED_VOLTAGE, rs485readSuccess);
}

void EPEVERSolarTracer::AddressRegistry_906B() {
    this->onPreNodeRequest();
    this->lastControllerCommunicationStatus = this->node.readHoldingRegisters(MODBUS_ADDRESS_EQUALIZE_DURATION, 6);
    rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
    if (rs485readSuccess) {
        uint16_t sharedUInt16 = this->node.getResponseBuffer(0x00);
        this->setVariableValue(Variable::BATTERY_EQUALIZATION_DURATION, &sharedUInt16);
        sharedUInt16 = this->node.getResponseBuffer(0x01);
        this->setVariableValue(Variable::BATTERY_BOOST_DURATION, &sharedUInt16);
        sharedUInt16 = this->node.getResponseBuffer(0x05);
        this->setVariableValue(Variable::BATTERY_MANAGEMENT_MODE, &sharedUInt16);
        return;
    }

    this->setVariableReadReady(3, rs485readSuccess,
                               Variable::BATTERY_EQUALIZATION_DURATION,
                               Variable::BATTERY_BOOST_DURATION,
                               Variable::BATTERY_MANAGEMENT_MODE);
}

void EPEVERSolarTracer::fetchAddressStatusVariables() {
    this->onPreNodeRequest();
    this->lastControllerCommunicationStatus = this->node.readInputRegisters(MODBUS_ADDRESS_BATTERY_STATUS, 3);

    rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
    if (rs485readSuccess) {
        uint16_t batteryStatus = node.getResponseBuffer(0x00);
        if (batteryStatus) {
            // fault
            switch (batteryStatus & 3) {
                case 1:
                    this->setVariableValue(Variable::BATTERY_STATUS_TEXT, "! OVER VOLT");
                    break;
                case 2:
                    this->setVariableValue(Variable::BATTERY_STATUS_TEXT, "! UNDER VOLT");
                    break;
                case 3:
                    this->setVariableValue(Variable::BATTERY_STATUS_TEXT, "! LOW VOLT");
                    break;
                case 4:
                    this->setVariableValue(Variable::BATTERY_STATUS_TEXT, "! FAULT");
                    break;
            }

            switch ((batteryStatus >> 4) & 3) {
                case 1:
                    this->setVariableValue(Variable::BATTERY_STATUS_TEXT, "! OVER TEMP");
                    break;
                case 2:
                    this->setVariableValue(Variable::BATTERY_STATUS_TEXT, "! LOW TEMP");
                    break;
            }

            if (batteryStatus >> 8) {
                this->setVariableValue(Variable::BATTERY_STATUS_TEXT, "! ABN BATT. RESIST.");
            }
        } else {
            this->setVariableValue(Variable::BATTERY_STATUS_TEXT, "Normal");
        }

        uint16_t chargingStatus = node.getResponseBuffer(0x01);
        if (chargingStatus & 0xFF0) {
            // doc says that bit2 is 0:Normal, 1:Faul
            // seems like this bit is behaving different
            // 0: Not charging, 1: Charging
            // FAULTs are detected by checking all fault bits bit13-4
            // chargingStatus & 0xFF0 == 0 means no fault
        } else {
            switch ((chargingStatus >> 2) & 3) {
                case 0:
                    this->setVariableValue(Variable::CHARGING_EQUIPMENT_STATUS_TEXT, "Standby");
                    break;
                case 1:
                    this->setVariableValue(Variable::CHARGING_EQUIPMENT_STATUS_TEXT, "Float");
                    break;
                case 2:
                    this->setVariableValue(Variable::CHARGING_EQUIPMENT_STATUS_TEXT, "Boost");
                    break;
                case 3:
                    this->setVariableValue(Variable::CHARGING_EQUIPMENT_STATUS_TEXT, "Equalization");
                    break;
                default:
                    this->setVariableValue(Variable::CHARGING_EQUIPMENT_STATUS_TEXT, "??");
            }
        }

        uint16_t dischargingStatus = node.getResponseBuffer(0x02);
        if (dischargingStatus & 2) {
            // fault
            if (dischargingStatus & 16) {
                this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! OUT OVER VOLT.");
            } else if (dischargingStatus & 32) {
                this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! BOOST OVER VOLT");
            } else if (dischargingStatus & 64) {
                this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! HV SIDE SHORT");
            } else if (dischargingStatus & 128) {
                this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! INPUT OVER VOLT.");
            } else if (dischargingStatus & 256) {
                this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! OUT VOLT. ABN");
            } else if (dischargingStatus & 512) {
                this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! UNABLE STOP DISC.");
            } else if (dischargingStatus & 1024) {
                this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! UNABLE DISC.");
            } else if (dischargingStatus & 2048) {
                this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! SHORT");
            } else {
                this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! ??");
            }
        } else {
            if (dischargingStatus & 1) {
                this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "Running");
            } else {
                this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "Standby");
            }
        }
        return;
    }

    this->setVariableReadReady(3, rs485readSuccess,
                               Variable::BATTERY_STATUS_TEXT,
                               Variable::CHARGING_EQUIPMENT_STATUS_TEXT,
                               Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT);
}

void EPEVERSolarTracer::updateStats() {
    this->onPreNodeRequest();
    this->lastControllerCommunicationStatus = this->node.readInputRegisters(MODBUS_ADDRESS_STAT_MAX_PV_VOLTAGE_TODAY, 29);

    rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
    if (rs485readSuccess) {
        this->setFloatVariable(Variable::MAXIMUM_PV_VOLTAGE_TODAY, this->node.getResponseBuffer(0x00) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::MINIMUM_PV_VOLTAGE_TODAY, this->node.getResponseBuffer(0x01) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::MAXIMUM_BATTERY_VOLTAGE_TODAY, this->node.getResponseBuffer(0x02) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::MINIMUM_BATTERY_VOLTAGE_TODAY, this->node.getResponseBuffer(0x03) / ONE_HUNDRED_FLOAT);

        this->setFloatVariable(Variable::GENERATED_ENERGY_TODAY, (this->node.getResponseBuffer(12) | this->node.getResponseBuffer(13) << 16) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::GENERATED_ENERGY_MONTH, (this->node.getResponseBuffer(14) | this->node.getResponseBuffer(15) << 16) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::GENERATED_ENERGY_YEAR, (this->node.getResponseBuffer(16) | this->node.getResponseBuffer(17) << 16) / ONE_HUNDRED_FLOAT);
        this->setFloatVariable(Variable::GENERATED_ENERGY_TOTAL, (this->node.getResponseBuffer(18) | this->node.getResponseBuffer(19) << 16) / ONE_HUNDRED_FLOAT);
    }

    this->setVariableReadReady(8, rs485readSuccess,
                               Variable::MINIMUM_PV_VOLTAGE_TODAY,
                               Variable::MAXIMUM_PV_VOLTAGE_TODAY,
                               Variable::MINIMUM_BATTERY_VOLTAGE_TODAY,
                               Variable::MAXIMUM_BATTERY_VOLTAGE_TODAY,
                               Variable::GENERATED_ENERGY_TODAY,
                               Variable::GENERATED_ENERGY_MONTH,
                               Variable::GENERATED_ENERGY_YEAR,
                               Variable::GENERATED_ENERGY_TOTAL);
}

void EPEVERSolarTracer::onModbusPreTransmission() {
    digitalWrite(this->max485_re_neg, 1);
    digitalWrite(this->max485_de, 1);
}

void EPEVERSolarTracer::onModbusIdle() {
    // nothing to do here!
}

void EPEVERSolarTracer::onModbusPostTransmission() {
    digitalWrite(this->max485_re_neg, 0);
    digitalWrite(this->max485_de, 0);
}

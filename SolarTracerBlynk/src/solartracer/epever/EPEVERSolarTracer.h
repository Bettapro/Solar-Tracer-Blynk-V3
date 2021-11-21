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
#include "../SolarTracer.h"
#include <time.h>

class EPEVERSolarTracer : public SolarTracer, public ModbusMasterCallable {
  public:
    EPEVERSolarTracer(Stream & SerialCom, uint8_t slave, uint8_t max485_de, uint8_t max485_re_neg);
     EPEVERSolarTracer(Stream & SerialCom, uint8_t slave);

    virtual bool syncRealtimeClock(struct tm *ti);

    virtual void fetchAllValues();

    virtual bool updateRun();

    virtual bool fetchValue(SolarTracerVariables variable);

    virtual bool writeBoolValue(SolarTracerVariables variable, bool value) ;
    virtual bool writeValue(SolarTracerVariables variable, void* value) ;

    bool readControllerSingleCoil(uint16_t address) ;


    void AddressRegistry_3100();

    void AddressRegistry_3110();

    void AddressRegistry_311A();

    void AddressRegistry_331B();

    void AddressRegistry_9003();

    void fetchAddressStatusVariables();

    void updateStats();

    /*
       Implementation of ModbusMasterCallable
    */
    virtual void onModbusPreTransmission();

    virtual void onModbusIdle();

    virtual void onModbusPostTransmission();

  protected:
    uint8_t max485_re_neg, max485_de;

    bool rs485readSuccess;

    uint16_t globalUpdateCounter = 0;
    uint8_t currentRealtimeUpdateCounter = 0;

    ModbusMaster node;

    // MODBUS FUNCTION
    bool writeControllerSingleCoil(uint16_t address, bool value);
    bool writeControllerHoldingRegister(uint16_t address, uint8_t value);

    // MODBUS ADDRESS
    static const uint16_t MODBUS_ADDRESS_PV_VOLTAGE   =           0x3100;
    static const uint16_t MODBUS_ADDRESS_PV_POWER =             0x3102;
    static const uint16_t MODBUS_ADDRESS_PV_CURRENT =           0x3101;
    static const uint16_t MODBUS_ADDRESS_LOAD_CURRENT =           0x310D;
    static const uint16_t MODBUS_ADDRESS_LOAD_POWER =           0x310E;
    static const uint16_t MODBUS_ADDRESS_BATT_TEMP =          0x3110;
    static const uint16_t MODBUS_ADDRESS_BATT_VOLTAGE =           0x3104;
    static const uint16_t MODBUS_ADDRESS_BATT_SOC =             0x311A;
    static const uint16_t MODBUS_ADDRESS_BATTERY_CHARGE_CURRENT =     0x3105;
    static const uint16_t MODBUS_ADDRESS_BATTERY_CHARGE_POWER =       0x3106;
    static const uint16_t MODBUS_ADDRESS_BATTERY_OVERALL_CURRENT =    0x331B;
    static const uint16_t MODBUS_ADDRESS_LOAD_FORCE_ONOFF =       0x0006;
    static const uint16_t MODBUS_ADDRESS_LOAD_MANUAL_ONOFF =        0x0002;
    static const uint16_t MODBUS_ADDRESS_BATTERY_CHARGE_ONOFF =     0x0000;
    static const uint16_t MODBUS_ADDRESS_BATTERY_STATUS =    0x3200;
    static const uint16_t MODBUS_ADDRESS_CHARGING_EQUIPMENT_STATUS =    0x3200;
    static const uint16_t MODBUS_ADDRESS_DISCHARGING_EQUIPMENT_STATUS =    0x3200;
    static const uint16_t MODBUS_ADDRESS_STAT_MAX_PV_VOLTAGE_TODAY =    0x3300;
    static const uint16_t MODBUS_ADDRESS_STAT_GENERATED_ENERGY_TODAY =    0x330C;
    static const uint16_t MODBUS_ADDRESS_STAT_GENERATED_ENERGY_MONTH =    0x330E;
    static const uint16_t MODBUS_ADDRESS_STAT_GENERATED_ENERGY_YEAR =   0x3310;
    static const uint16_t MODBUS_ADDRESS_CHARGING_MODE =    0x3008;
    static const uint16_t MODBUS_ADDRESS_CONTROLLER_TEMP =    0x3111;
    static const uint16_t MODBUS_ADDRESS_REMOTE_BATTERY_TEMP =    0x311B;
    static const uint16_t MODBUS_ADDRESS_REALTIME_CLOCK =    0x9013;
    static const uint16_t MODBUS_ADDRESS_BATTERY_OVERVOLTAGE_DISCONNECT =    0x9003;
};



#endif

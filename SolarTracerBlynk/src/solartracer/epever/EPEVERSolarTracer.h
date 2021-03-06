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

class EPEVERSolarTracer : public SolarTracer, public ModbusMasterCallable
{
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

  static constexpr const float ONE_HUNDRED_FLOAT = 100.0;
  // MODBUS ADDRESS
  static const uint16_t MODBUS_ADDRESS_PV_VOLTAGE = 0x3100;
  static const uint16_t MODBUS_ADDRESS_PV_POWER = 0x3102;
  static const uint16_t MODBUS_ADDRESS_PV_CURRENT = 0x3101;
  static const uint16_t MODBUS_ADDRESS_LOAD_CURRENT = 0x310D;
  static const uint16_t MODBUS_ADDRESS_LOAD_POWER = 0x310E;
  static const uint16_t MODBUS_ADDRESS_BATT_TEMP = 0x3110;
  static const uint16_t MODBUS_ADDRESS_BATT_VOLTAGE = 0x3104;
  static const uint16_t MODBUS_ADDRESS_BATT_SOC = 0x311A;
  static const uint16_t MODBUS_ADDRESS_BATTERY_CHARGE_CURRENT = 0x3105;
  static const uint16_t MODBUS_ADDRESS_BATTERY_CHARGE_POWER = 0x3106;
  static const uint16_t MODBUS_ADDRESS_BATTERY_OVERALL_CURRENT = 0x331B;
  static const uint16_t MODBUS_ADDRESS_LOAD_FORCE_ONOFF = 0x0006;
  static const uint16_t MODBUS_ADDRESS_LOAD_MANUAL_ONOFF = 0x0002;
  static const uint16_t MODBUS_ADDRESS_BATTERY_CHARGE_ONOFF = 0x0000;
  static const uint16_t MODBUS_ADDRESS_BATTERY_STATUS = 0x3200;
  static const uint16_t MODBUS_ADDRESS_CHARGING_EQUIPMENT_STATUS = 0x3200;
  static const uint16_t MODBUS_ADDRESS_DISCHARGING_EQUIPMENT_STATUS = 0x3200;
  static const uint16_t MODBUS_ADDRESS_STAT_MAX_PV_VOLTAGE_TODAY = 0x3300;
  static const uint16_t MODBUS_ADDRESS_STAT_GENERATED_ENERGY_TODAY = 0x330C;
  static const uint16_t MODBUS_ADDRESS_STAT_GENERATED_ENERGY_MONTH = 0x330E;
  static const uint16_t MODBUS_ADDRESS_STAT_GENERATED_ENERGY_YEAR = 0x3310;
  static const uint16_t MODBUS_ADDRESS_CONTROLLER_TEMP = 0x3111;
  static const uint16_t MODBUS_ADDRESS_REMOTE_BATTERY_TEMP = 0x311B;
  static const uint16_t MODBUS_ADDRESS_REALTIME_CLOCK = 0x9013;
  static const uint16_t MODBUS_ADDRESS_BATTERY_TYPE = 0x9000;
  static const uint16_t MODBUS_ADDRESS_BATTERY_CAPACITY = 0x9001;
  static const uint16_t MODBUS_ADDRESS_BATTERY_TEMP_COEFF = 0x9002;
  static const uint16_t MODBUS_ADDRESS_HIGH_VOLTAGE_DISCONNECT = 0x9003;
  static const uint16_t MODBUS_ADDRESS_CHARGING_LIMIT_VOLTAGE = 0x9004;
  static const uint16_t MODBUS_ADDRESS_OVER_VOLTAGE_RECONNECT = 0x9005;
  static const uint16_t MODBUS_ADDRESS_EQUALIZATION_VOLTAGE = 0x9006;
  static const uint16_t MODBUS_ADDRESS_BOOST_VOLTAGE = 0x9007;
  static const uint16_t MODBUS_ADDRESS_FLOAT_VOLTAGE = 0x9008;
  static const uint16_t MODBUS_ADDRESS_BOOST_RECONNECT_VOLTAGE = 0x9009;
  static const uint16_t MODBUS_ADDRESS_LOW_VOLTAGE_RECONNECT = 0x900A;
  static const uint16_t MODBUS_ADDRESS_UNDER_VOLTAGE_RECOVER = 0x900B;
  static const uint16_t MODBUS_ADDRESS_UNDER_VOLTAGE_WARNING = 0x900C;
  static const uint16_t MODBUS_ADDRESS_LOW_VOLTAGE_DISCONNECT = 0x900D;
  static const uint16_t MODBUS_ADDRESS_DISCHARGING_LIMIT_VOLTAGE = 0x900E;
  static const uint16_t MODBUS_ADDRESS_BATTERY_RATED_LEVEL = 0x9067;
  static const uint16_t MODBUS_ADDRESS_EQUALIZE_DURATION = 0x906B;
  static const uint16_t MODBUS_ADDRESS_BOOST_DURATION = 0x906C;
  static const uint16_t MODBUS_ADDRESS_CHARGING_MODE = 0x9070;

private:
  static const uint8_t voltageLevels[];

  void onPreNodeRequest()
  {
    if (this->preTransmitWaitMs > 0)
    {
      delay(this->preTransmitWaitMs);
    }
  }

  static uint16_t getBatteryVoltageLevelFromVoltage(uint16_t voltage)
  {
    uint8_t index = 0;
    while (true)
    {
      if (voltageLevels[index] == 0)
      {
        return 0;
      }
      if (voltageLevels[index] == voltage)
      {
        return index + 1;
      }
      index++;
    }
  }

  static uint16_t getVoltageFromBatteryVoltageLevel(uint16_t index)
  {
    return voltageLevels[index];
  }
};

#endif

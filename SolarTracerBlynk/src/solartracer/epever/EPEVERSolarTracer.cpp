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

#define _EST_RS_POINTER(s, v) (s ? &v : nullptr)


EPEVERSolarTracer::EPEVERSolarTracer(Stream &serialCom, uint16_t serialTimeoutMs, uint8_t slave, uint8_t max485_de, uint8_t max485_re_neg)
    : EPEVERSolarTracer(serialCom, serialTimeoutMs, slave)
{

  this->max485_re_neg = max485_re_neg;
  this->max485_de = max485_de;

  pinMode(this->max485_re_neg, OUTPUT);
  pinMode(this->max485_de, OUTPUT);

  digitalWrite(this->max485_re_neg, 0);
  digitalWrite(this->max485_de, 0);

  // set this instance as the callback receiver
  this->node.setTransmissionCallable(this);

  if (serialTimeoutMs > 0)
  {
    this->node.setResponseTimeout(serialTimeoutMs);
  };
}

EPEVERSolarTracer::EPEVERSolarTracer(Stream &serialCom, uint16_t serialTimeoutMs, uint8_t slave)
    : SolarTracer()
{
  this->node.begin(slave, serialCom);

  // won't be used as it's not registering to transmission callback
  this->max485_re_neg = this->max485_de = 0;

  this->rs485readSuccess = true;

  // set enabled variables
  this->setVariableEnabled(Variable::PV_POWER);
  this->setVariableEnabled(Variable::PV_CURRENT);
  this->setVariableEnabled(Variable::PV_VOLTAGE);
  this->setVariableEnabled(Variable::LOAD_CURRENT);
  this->setVariableEnabled(Variable::LOAD_POWER);
  this->setVariableEnabled(Variable::BATTERY_TEMP);
  this->setVariableEnabled(Variable::BATTERY_VOLTAGE);
  this->setVariableEnabled(Variable::BATTERY_SOC);
  this->setVariableEnabled(Variable::CONTROLLER_TEMP);
  this->setVariableEnabled(Variable::BATTERY_CHARGE_CURRENT);
  this->setVariableEnabled(Variable::BATTERY_CHARGE_POWER);
  this->setVariableEnabled(Variable::BATTERY_OVERALL_CURRENT);
  this->setVariableEnabled(Variable::LOAD_MANUAL_ONOFF);
  this->setVariableEnabled(Variable::CHARGING_DEVICE_ONOFF);
  this->setVariableEnabled(Variable::BATTERY_STATUS_TEXT);
  this->setVariableEnabled(Variable::CHARGING_EQUIPMENT_STATUS_TEXT);
  this->setVariableEnabled(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT);
  this->setVariableEnabled(Variable::GENERATED_ENERGY_TODAY);
  this->setVariableEnabled(Variable::GENERATED_ENERGY_MONTH);
  this->setVariableEnabled(Variable::GENERATED_ENERGY_YEAR);
  this->setVariableEnabled(Variable::GENERATED_ENERGY_TOTAL);
  this->setVariableEnabled(Variable::MAXIMUM_PV_VOLTAGE_TODAY);
  this->setVariableEnabled(Variable::MINIMUM_PV_VOLTAGE_TODAY);
  this->setVariableEnabled(Variable::MAXIMUM_BATTERY_VOLTAGE_TODAY);
  this->setVariableEnabled(Variable::MINIMUM_BATTERY_VOLTAGE_TODAY);
  this->setVariableEnabled(Variable::HEATSINK_TEMP);
  this->setVariableEnabled(Variable::BATTERY_BOOST_VOLTAGE);
  this->setVariableEnabled(Variable::BATTERY_EQUALIZATION_VOLTAGE);
  this->setVariableEnabled(Variable::BATTERY_FLOAT_VOLTAGE);
  this->setVariableEnabled(Variable::BATTERY_FLOAT_MIN_VOLTAGE);
  this->setVariableEnabled(Variable::BATTERY_CHARGING_LIMIT_VOLTAGE);
  this->setVariableEnabled(Variable::BATTERY_DISCHARGING_LIMIT_VOLTAGE);
  this->setVariableEnabled(Variable::BATTERY_LOW_VOLTAGE_DISCONNECT);
  this->setVariableEnabled(Variable::BATTERY_LOW_VOLTAGE_RECONNECT);
  this->setVariableEnabled(Variable::BATTERY_OVER_VOLTAGE_DISCONNECT);
  this->setVariableEnabled(Variable::BATTERY_OVER_VOLTAGE_RECONNECT);
  this->setVariableEnabled(Variable::BATTERY_UNDER_VOLTAGE_SET);
  this->setVariableEnabled(Variable::BATTERY_UNDER_VOLTAGE_RESET);
}

bool EPEVERSolarTracer::testConnection()
{
  return this->fetchValue(Variable::LOAD_MANUAL_ONOFF);
}

bool EPEVERSolarTracer::syncRealtimeClock(struct tm *ti)
{
  node.setTransmitBuffer(0, (ti->tm_min << 8) + ti->tm_sec);
  node.setTransmitBuffer(1, (ti->tm_mday << 8) + ti->tm_hour);
  node.setTransmitBuffer(2, ((ti->tm_year + 1900 - 2000) << 8) + ti->tm_mon + 1);

  this->lastControllerCommunicationStatus = node.writeMultipleRegisters(MODBUS_ADDRESS_REALTIME_CLOCK, 3);
  if (this->lastControllerCommunicationStatus == this->node.ku8MBSuccess)
  {
    this->node.getResponseBuffer(0x00);
    return true;
  }
  return false;
};

void EPEVERSolarTracer::fetchAllValues()
{

  // STATS
  this->updateStats();
  this->AddressRegistry_9003();
  //REALTIME
  this->AddressRegistry_3100();
  this->AddressRegistry_3110();
  this->AddressRegistry_311A();
  this->AddressRegistry_331B();
  this->fetchValue(Variable::LOAD_MANUAL_ONOFF);
  this->fetchValue(Variable::CHARGING_DEVICE_ONOFF);
  this->fetchAddressStatusVariables();
}

bool EPEVERSolarTracer::updateRun()
{
  switch (globalUpdateCounter)
  {
  case 360:
    // update statistics
    this->updateStats();
    this->AddressRegistry_9003();
    globalUpdateCounter = 0;
    break;
  default:
    switch (currentRealtimeUpdateCounter)
    {
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
    }
    currentRealtimeUpdateCounter--;
  }
  return rs485readSuccess;
}

bool EPEVERSolarTracer::fetchValue(Variable variable)
{
  bool value;
  switch (variable)
  {
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


bool EPEVERSolarTracer::writeValue(Variable variable, const void *value)
{
  if (!this->isVariableEnabled(variable))
  {
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

  switch (variable)
  {
  case Variable::LOAD_FORCE_ONOFF:
    return this->writeControllerSingleCoil(MODBUS_ADDRESS_LOAD_FORCE_ONOFF, *(bool *)value);
  case Variable::LOAD_MANUAL_ONOFF:
    return this->writeControllerSingleCoil(MODBUS_ADDRESS_LOAD_MANUAL_ONOFF, *(bool *)value);
  case Variable::CHARGING_DEVICE_ONOFF:
    return this->writeControllerSingleCoil(MODBUS_ADDRESS_BATTERY_CHARGE_ONOFF, *(bool *)value);
    //
  case Variable::BATTERY_OVER_VOLTAGE_DISCONNECT:
    return this->replaceControllerHoldingRegister(MODBUS_ADDRESS_HIGH_VOLTAGE_DISCONNECT, (*(float *)value) * 100, MODBUS_ADDRESS_BATTERY_TYPE, 15);
  case Variable::BATTERY_CHARGING_LIMIT_VOLTAGE:
    return this->replaceControllerHoldingRegister(MODBUS_ADDRESS_CHARGING_LIMIT_VOLTAGE, (*(float *)value) * 100, MODBUS_ADDRESS_BATTERY_TYPE, 15);
  case Variable::BATTERY_OVER_VOLTAGE_RECONNECT:
    return this->replaceControllerHoldingRegister(MODBUS_ADDRESS_OVER_VOLTAGE_RECONNECT, (*(float *)value) * 100, MODBUS_ADDRESS_BATTERY_TYPE, 15);
  case Variable::BATTERY_EQUALIZATION_VOLTAGE:
    return this->replaceControllerHoldingRegister(MODBUS_ADDRESS_EQUALIZATION_VOLTAGE, (*(float *)value) * 100, MODBUS_ADDRESS_BATTERY_TYPE, 15);
  case Variable::BATTERY_BOOST_VOLTAGE:
    return this->replaceControllerHoldingRegister(MODBUS_ADDRESS_BOOST_VOLTAGE, (*(float *)value) * 100, MODBUS_ADDRESS_BATTERY_TYPE, 15);
  case Variable::BATTERY_FLOAT_VOLTAGE:
    return this->replaceControllerHoldingRegister(MODBUS_ADDRESS_FLOAT_VOLTAGE, (*(float *)value) * 100, MODBUS_ADDRESS_BATTERY_TYPE, 15);
  case Variable::BATTERY_FLOAT_MIN_VOLTAGE:
    return this->replaceControllerHoldingRegister(MODBUS_ADDRESS_BOOST_RECONNECT_VOLTAGE, (*(float *)value) * 100, MODBUS_ADDRESS_BATTERY_TYPE, 15);
  case Variable::BATTERY_LOW_VOLTAGE_RECONNECT:
    return this->replaceControllerHoldingRegister(MODBUS_ADDRESS_LOW_VOLTAGE_RECONNECT, (*(float *)value) * 100, MODBUS_ADDRESS_BATTERY_TYPE, 15);
  case Variable::BATTERY_UNDER_VOLTAGE_RESET:
    return this->replaceControllerHoldingRegister(MODBUS_ADDRESS_UNDER_VOLTAGE_RECOVER, (*(float *)value) * 100, MODBUS_ADDRESS_BATTERY_TYPE, 15);
  case Variable::BATTERY_UNDER_VOLTAGE_SET:
    return this->replaceControllerHoldingRegister(MODBUS_ADDRESS_UNDER_VOLTAGE_WARNING, (*(float *)value) * 100, MODBUS_ADDRESS_BATTERY_TYPE, 15);
  case Variable::BATTERY_LOW_VOLTAGE_DISCONNECT:
    return this->replaceControllerHoldingRegister(MODBUS_ADDRESS_LOW_VOLTAGE_DISCONNECT, (*(float *)value) * 100, MODBUS_ADDRESS_BATTERY_TYPE, 15);
  case Variable::BATTERY_DISCHARGING_LIMIT_VOLTAGE:
    return this->replaceControllerHoldingRegister(MODBUS_ADDRESS_DISCHARGING_LIMIT_VOLTAGE, (*(float *)value) * 100, MODBUS_ADDRESS_BATTERY_TYPE, 15);
  }

  return false;
}

bool EPEVERSolarTracer::readControllerSingleCoil(uint16_t address)
{
  this->lastControllerCommunicationStatus = this->node.readCoils(address, 1);

  rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {
    return (node.getResponseBuffer(0x00) > 0);
  }
  return false;
}

bool EPEVERSolarTracer::writeControllerSingleCoil(uint16_t address, bool value)
{
  this->lastControllerCommunicationStatus = this->node.writeSingleCoil(address, value);

  if (this->lastControllerCommunicationStatus == this->node.ku8MBSuccess)
  {
    this->node.getResponseBuffer(0x00);
    return true;
  }
  return false;
}

bool EPEVERSolarTracer::replaceControllerHoldingRegister(uint16_t address, uint16_t value, uint16_t fromAddress, uint8_t count)
{
  this->lastControllerCommunicationStatus = this->node.readHoldingRegisters(fromAddress, count);
  if (this->lastControllerCommunicationStatus == this->node.ku8MBSuccess)
  {
    // adding a delay here could help, epever tends to trigger timeouts
    for (uint8_t i = 0; i < count; i++)
    {
      this->node.setTransmitBuffer(i, this->node.getResponseBuffer(i));
    }
    this->node.setTransmitBuffer(0, 0);
    this->node.setTransmitBuffer(address - fromAddress, value);

    this->lastControllerCommunicationStatus = this->node.writeMultipleRegisters(fromAddress, count);
    return this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
  }
  return false;
}

void EPEVERSolarTracer::AddressRegistry_3100()
{
  this->lastControllerCommunicationStatus = this->node.readInputRegisters(MODBUS_ADDRESS_PV_VOLTAGE, 16);

  rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {

    this->setFloatVariable(Variable::PV_VOLTAGE, this->node.getResponseBuffer(0x00) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::PV_CURRENT, this->node.getResponseBuffer(0x01) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::PV_POWER, (this->node.getResponseBuffer(0x02) | this->node.getResponseBuffer(0x03) << 16) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::BATTERY_VOLTAGE, this->node.getResponseBuffer(0x04) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::BATTERY_CHARGE_CURRENT, this->node.getResponseBuffer(0x05) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::BATTERY_CHARGE_POWER, (this->node.getResponseBuffer(0x06) | this->node.getResponseBuffer(0x07) << 16) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::LOAD_CURRENT, this->node.getResponseBuffer(0x0D) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::LOAD_POWER, (this->node.getResponseBuffer(0x0E) | this->node.getResponseBuffer(0x02) << 16) / ONE_HUNDRED_FLOAT);
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

void EPEVERSolarTracer::AddressRegistry_3110()
{
  // 0x3114,0x3115 -> returns modbus error code 2, must use AddressRegistry_311A
  this->lastControllerCommunicationStatus = this->node.readInputRegisters(MODBUS_ADDRESS_BATT_TEMP, 3);

  rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {
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

void EPEVERSolarTracer::AddressRegistry_311A()
{
  this->lastControllerCommunicationStatus = this->node.readInputRegisters(MODBUS_ADDRESS_BATT_SOC, 2);

  rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {
    this->setFloatVariable(Variable::BATTERY_SOC, this->node.getResponseBuffer(0x00) / 1.0f);
    this->setFloatVariable(Variable::REMOTE_BATTERY_TEMP, this->node.getResponseBuffer(0x01) / ONE_HUNDRED_FLOAT);

    return;
  }

  this->setVariableReadReady(2, rs485readSuccess,
                             Variable::BATTERY_SOC,
                             Variable::REMOTE_BATTERY_TEMP);
}

void EPEVERSolarTracer::AddressRegistry_331B()
{
  this->lastControllerCommunicationStatus = this->node.readInputRegisters(0x331B, 2);

  rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {
    this->setFloatVariable(Variable::BATTERY_OVERALL_CURRENT, (this->node.getResponseBuffer(0x00) | this->node.getResponseBuffer(0x01) << 16) / ONE_HUNDRED_FLOAT);
  }

  this->setVariableReadReady(Variable::BATTERY_OVERALL_CURRENT, rs485readSuccess);
}

void EPEVERSolarTracer::AddressRegistry_9003()
{
  this->lastControllerCommunicationStatus = this->node.readHoldingRegisters(MODBUS_ADDRESS_HIGH_VOLTAGE_DISCONNECT, 12);

  rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {
    this->setFloatVariable(Variable::BATTERY_OVER_VOLTAGE_DISCONNECT, this->node.getResponseBuffer(0x00) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::BATTERY_CHARGING_LIMIT_VOLTAGE, this->node.getResponseBuffer(0x01) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::BATTERY_OVER_VOLTAGE_RECONNECT, this->node.getResponseBuffer(0x02) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::BATTERY_EQUALIZATION_VOLTAGE, this->node.getResponseBuffer(0x03) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::BATTERY_BOOST_VOLTAGE, this->node.getResponseBuffer(0x04) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::BATTERY_FLOAT_VOLTAGE, this->node.getResponseBuffer(0x05) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::BATTERY_FLOAT_MIN_VOLTAGE, this->node.getResponseBuffer(0x06) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::BATTERY_LOW_VOLTAGE_RECONNECT, this->node.getResponseBuffer(0x07) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::BATTERY_UNDER_VOLTAGE_RESET, this->node.getResponseBuffer(0x08) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::BATTERY_UNDER_VOLTAGE_SET, this->node.getResponseBuffer(0x09) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::BATTERY_LOW_VOLTAGE_DISCONNECT, this->node.getResponseBuffer(0x0A) / ONE_HUNDRED_FLOAT);
    this->setFloatVariable(Variable::BATTERY_DISCHARGING_LIMIT_VOLTAGE, this->node.getResponseBuffer(0x0B) / ONE_HUNDRED_FLOAT);
  }

  this->setVariableReadReady(12, rs485readSuccess,
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

void EPEVERSolarTracer::fetchAddressStatusVariables()
{
  this->lastControllerCommunicationStatus = this->node.readInputRegisters(MODBUS_ADDRESS_BATTERY_STATUS, 3);

  rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {
    uint16_t batteryStatus = node.getResponseBuffer(0x00);
    if (batteryStatus)
    {
      // fault
      switch (batteryStatus & 3)
      {
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

      switch ((batteryStatus >> 4) & 3)
      {
      case 1:
        this->setVariableValue(Variable::BATTERY_STATUS_TEXT, "! OVER TEMP");
        break;
      case 2:
        this->setVariableValue(Variable::BATTERY_STATUS_TEXT, "! LOW TEMP");
        break;
      }

      if (batteryStatus >> 8)
      {
        this->setVariableValue(Variable::BATTERY_STATUS_TEXT, "! ABN BATT. RESIST.");
      }
    }
    else
    {
      this->setVariableValue(Variable::BATTERY_STATUS_TEXT, "Normal");
    }

    uint16_t chargingStatus = node.getResponseBuffer(0x01);
    if (chargingStatus & 0xFF0)
    {
      // doc says that bit2 is 0:Normal, 1:Faul
      // seems like this bit is behaving different
      // 0: Not charging, 1: Charging
      // FAULTs are detected by checking all fault bits bit13-4
      // chargingStatus & 0xFF0 == 0 means no fault
    }
    else
    {
      switch ((chargingStatus >> 2) & 3)
      {
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
    if (dischargingStatus & 2)
    {
      // fault
      if (dischargingStatus & 16)
      {
        this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! OUT OVER VOLT.");
      }
      else if (dischargingStatus & 32)
      {
        this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! BOOST OVER VOLT");
      }
      else if (dischargingStatus & 64)
      {
        this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! HV SIDE SHORT");
      }
      else if (dischargingStatus & 128)
      {
        this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! INPUT OVER VOLT.");
      }
      else if (dischargingStatus & 256)
      {
        this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! OUT VOLT. ABN");
      }
      else if (dischargingStatus & 512)
      {
        this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! UNABLE STOP DISC.");
      }
      else if (dischargingStatus & 1024)
      {
        this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! UNABLE DISC.");
      }
      else if (dischargingStatus & 2048)
      {
        this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! SHORT");
      }
      else
      {
        this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "! ??");
      }
    }
    else
    {
      if (dischargingStatus & 1)
      {
        this->setVariableValue(Variable::DISCHARGING_EQUIPMENT_STATUS_TEXT, "Running");
      }
      else
      {
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

void EPEVERSolarTracer::updateStats()
{
  this->lastControllerCommunicationStatus = this->node.readInputRegisters(MODBUS_ADDRESS_STAT_MAX_PV_VOLTAGE_TODAY, 29);

  rs485readSuccess = this->lastControllerCommunicationStatus == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {

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

void EPEVERSolarTracer::onModbusPreTransmission()
{
  digitalWrite(this->max485_re_neg, 1);
  digitalWrite(this->max485_de, 1);
}

void EPEVERSolarTracer::onModbusIdle()
{
  // nothing to do here!
}

void EPEVERSolarTracer::onModbusPostTransmission()
{
  digitalWrite(this->max485_re_neg, 0);
  digitalWrite(this->max485_de, 0);
}

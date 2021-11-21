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


EPEVERSolarTracer::EPEVERSolarTracer(Stream &SerialCom, uint8_t slave, uint8_t max485_de, uint8_t max485_re_neg)
    : EPEVERSolarTracer(SerialCom, slave)
{

  this->max485_re_neg = max485_re_neg;
  this->max485_de = max485_de;

  pinMode(this->max485_re_neg, OUTPUT);
  pinMode(this->max485_de, OUTPUT);

  digitalWrite(this->max485_re_neg, 0);
  digitalWrite(this->max485_de, 0);

  // set this instance as the callback receiver
  this->node.setTransmissionCallable(this);
}

EPEVERSolarTracer::EPEVERSolarTracer(Stream &SerialCom, uint8_t slave)
    : SolarTracer()
{
  this->node.begin(slave, SerialCom);

  // won't be used as it's not registering to transmission callback
  this->max485_re_neg = this->max485_de = 0;

  this->rs485readSuccess = true;

  // set enabled variables
  this->setVariableEnabled(SolarTracerVariables::PV_POWER);
  this->setVariableEnabled(SolarTracerVariables::PV_CURRENT);
  this->setVariableEnabled(SolarTracerVariables::PV_VOLTAGE);
  this->setVariableEnabled(SolarTracerVariables::LOAD_CURRENT);
  this->setVariableEnabled(SolarTracerVariables::LOAD_POWER);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_TEMP);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_VOLTAGE);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_SOC);
  this->setVariableEnabled(SolarTracerVariables::CONTROLLER_TEMP);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_CHARGE_CURRENT);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_CHARGE_POWER);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_OVERALL_CURRENT);
  this->setVariableEnabled(SolarTracerVariables::LOAD_MANUAL_ONOFF);
  this->setVariableEnabled(SolarTracerVariables::CHARGING_DEVICE_ONOFF);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_STATUS_TEXT);
  this->setVariableEnabled(SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT);
  this->setVariableEnabled(SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT);
  this->setVariableEnabled(SolarTracerVariables::GENERATED_ENERGY_TODAY);
  this->setVariableEnabled(SolarTracerVariables::GENERATED_ENERGY_MONTH);
  this->setVariableEnabled(SolarTracerVariables::GENERATED_ENERGY_YEAR);
  this->setVariableEnabled(SolarTracerVariables::GENERATED_ENERGY_TOTAL);
  this->setVariableEnabled(SolarTracerVariables::HEATSINK_TEMP);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_BOOST_VOLTAGE);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_EQUALIZATION_VOLTAGE);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_FLOAT_VOLTAGE);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_FLOAT_MIN_VOLTAGE);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_CHARGING_LIMIT_VOLTAGE);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_DISCHARGING_LIMIT_VOLTAGE);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_LOW_VOLTAGE_DISCONNECT);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_LOW_VOLTAGE_RECONNECT);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_OVER_VOLTAGE_DISCONNECT);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_OVER_VOLTAGE_CONNECT);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_UNDER_VOLTAGE_SET);
  this->setVariableEnabled(SolarTracerVariables::BATTERY_UNDER_VOLTAGE_RESET);
}

bool EPEVERSolarTracer::syncRealtimeClock(struct tm *ti)
{
  node.setTransmitBuffer(0, (ti->tm_min << 8) + ti->tm_sec);
  node.setTransmitBuffer(1, (ti->tm_mday << 8) + ti->tm_hour);
  node.setTransmitBuffer(2, ((ti->tm_year + 1900 - 2000) << 8) + ti->tm_mon + 1);

  uint8_t result = node.writeMultipleRegisters(MODBUS_ADDRESS_REALTIME_CLOCK, 3);

  if (node.writeMultipleRegisters(MODBUS_ADDRESS_REALTIME_CLOCK, 3) == this->node.ku8MBSuccess)
  {
    this->node.getResponseBuffer(0x00);
    return true;
  }
  return false;
};

void EPEVERSolarTracer::fetchAllValues()
{
  // STATS
  updateStats();
  this->AddressRegistry_9003();
  //REALTIME
  this->AddressRegistry_3100();
  this->AddressRegistry_3110();
  this->AddressRegistry_311A();
  this->AddressRegistry_331B();
  this->fetchValue(SolarTracerVariables::LOAD_MANUAL_ONOFF);
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
      this->fetchValue(SolarTracerVariables::LOAD_MANUAL_ONOFF);
      this->fetchValue(SolarTracerVariables::CHARGING_DEVICE_ONOFF);
      this->fetchAddressStatusVariables();
    }
    currentRealtimeUpdateCounter--;
  }
  return rs485readSuccess;
}

bool EPEVERSolarTracer::fetchValue(SolarTracerVariables variable)
{
  bool value;
  switch (variable)
  {
  case SolarTracerVariables::LOAD_FORCE_ONOFF:
    value = this->readControllerSingleCoil(MODBUS_ADDRESS_LOAD_FORCE_ONOFF);
    return this->setVariableValue(variable, _EST_RS_POINTER(rs485readSuccess, value));
  case SolarTracerVariables::LOAD_MANUAL_ONOFF:
    value = this->readControllerSingleCoil(MODBUS_ADDRESS_LOAD_MANUAL_ONOFF);
    return this->setVariableValue(variable, _EST_RS_POINTER(rs485readSuccess, value));
  case SolarTracerVariables::CHARGING_DEVICE_ONOFF:
    value = this->readControllerSingleCoil(MODBUS_ADDRESS_BATTERY_CHARGE_ONOFF);
    return this->setVariableValue(variable, _EST_RS_POINTER(rs485readSuccess, value));
  default:
    break;
  }

  return false;
}


bool EPEVERSolarTracer::writeValue(SolarTracerVariables variable, bool* value)
{
  if (!this->isVariableEnabled(variable))
  {
    return false;
  }

  switch (variable)
  {
  case SolarTracerVariables::LOAD_FORCE_ONOFF:
    return this->writeControllerSingleCoil(MODBUS_ADDRESS_LOAD_FORCE_ONOFF, (*(bool*)value));
  case SolarTracerVariables::LOAD_MANUAL_ONOFF:
    return this->writeControllerSingleCoil(MODBUS_ADDRESS_LOAD_MANUAL_ONOFF, (*(bool*)value));
  case SolarTracerVariables::CHARGING_DEVICE_ONOFF:
    return this->writeControllerSingleCoil(MODBUS_ADDRESS_BATTERY_CHARGE_ONOFF, (*(bool*)value));
  default:
    break;
  }

  return false;
}

bool EPEVERSolarTracer::writeValue(SolarTracerVariables variable, float* value)
{
  if (!this->isVariableEnabled(variable))
  {
    return false;
  }

  switch (variable)
  {
  case SolarTracerVariables::LOAD_FORCE_ONOFF:
    return this->writeControllerSingleCoil(MODBUS_ADDRESS_LOAD_FORCE_ONOFF, (*(float*)value) > 0);
  case SolarTracerVariables::LOAD_MANUAL_ONOFF:
    return this->writeControllerSingleCoil(MODBUS_ADDRESS_LOAD_MANUAL_ONOFF, (*(float*)value) > 0);
  case SolarTracerVariables::CHARGING_DEVICE_ONOFF:
    return this->writeControllerSingleCoil(MODBUS_ADDRESS_BATTERY_CHARGE_ONOFF, (*(float*)value) > 0);
  default:
    break;
  }

  return false;
}

bool EPEVERSolarTracer::readControllerSingleCoil(uint16_t address)
{
  uint8_t result = this->node.readCoils(address, 1);

  rs485readSuccess = result == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {
    return (node.getResponseBuffer(0x00) > 0);
  }
  return false;
}

bool EPEVERSolarTracer::writeControllerSingleCoil(uint16_t address, bool value)
{
  uint8_t result = this->node.writeSingleCoil(address, value);

  if (result == this->node.ku8MBSuccess)
  {
    this->node.getResponseBuffer(0x00);
    return true;
  }
  return false;
}

bool EPEVERSolarTracer::writeControllerHoldingRegister(uint16_t address, uint8_t value)
{
  uint8_t result = this->node.writeSingleRegister(address, value);

  if (result == this->node.ku8MBSuccess)
  {
    this->node.getResponseBuffer(0x00);
    return true;
  }
  return false;
}

void EPEVERSolarTracer::AddressRegistry_3100()
{
  uint8_t result = this->node.readInputRegisters(MODBUS_ADDRESS_PV_VOLTAGE, 16);

  rs485readSuccess = result == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {

    this->floatVars[SolarTracerVariables::PV_VOLTAGE] = this->node.getResponseBuffer(0x00) / 100.0f;
    this->floatVars[SolarTracerVariables::PV_CURRENT] = this->node.getResponseBuffer(0x01) / 100.0f;
    this->floatVars[SolarTracerVariables::PV_POWER] = (this->node.getResponseBuffer(0x02) | this->node.getResponseBuffer(0x03) << 16) / 100.0f;
    this->floatVars[SolarTracerVariables::BATTERY_VOLTAGE] = this->node.getResponseBuffer(0x04) / 100.0f;
    this->floatVars[SolarTracerVariables::BATTERY_CHARGE_CURRENT] = this->node.getResponseBuffer(0x05) / 100.0f;
    this->floatVars[SolarTracerVariables::BATTERY_CHARGE_POWER] = (this->node.getResponseBuffer(0x06) | this->node.getResponseBuffer(0x07) << 16) / 100.0f;

    this->floatVars[SolarTracerVariables::LOAD_CURRENT] = this->node.getResponseBuffer(0x0D) / 100.0f;
    this->floatVars[SolarTracerVariables::LOAD_POWER] = (this->node.getResponseBuffer(0x0E) | this->node.getResponseBuffer(0x02) << 16) / 100.0f;
  }

  this->setVariableReadReady(8, rs485readSuccess,
                             SolarTracerVariables::PV_VOLTAGE,
                             SolarTracerVariables::PV_CURRENT,
                             SolarTracerVariables::PV_POWER,
                             SolarTracerVariables::BATTERY_VOLTAGE,
                             SolarTracerVariables::BATTERY_CHARGE_CURRENT,
                             SolarTracerVariables::BATTERY_CHARGE_POWER,
                             SolarTracerVariables::LOAD_CURRENT,
                             SolarTracerVariables::LOAD_POWER);
}

void EPEVERSolarTracer::AddressRegistry_3110()
{
  // 0x3114,0x3115 -> returns modbus error code 2, must use AddressRegistry_311A
  uint8_t result = this->node.readInputRegisters(MODBUS_ADDRESS_BATT_TEMP, 3);

  rs485readSuccess = result == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {
    this->floatVars[SolarTracerVariables::BATTERY_TEMP] = this->node.getResponseBuffer(0x00) / 100.0f;
    this->floatVars[SolarTracerVariables::CONTROLLER_TEMP] = this->node.getResponseBuffer(0x01) / 100.0f;
    this->floatVars[SolarTracerVariables::HEATSINK_TEMP] = this->node.getResponseBuffer(0x02) / 100.0f;
  }

  this->setVariableReadReady(5, rs485readSuccess,
                             SolarTracerVariables::BATTERY_TEMP,
                             SolarTracerVariables::CONTROLLER_TEMP,
                             SolarTracerVariables::HEATSINK_TEMP);
}

void EPEVERSolarTracer::AddressRegistry_311A()
{
  uint8_t result = this->node.readInputRegisters(MODBUS_ADDRESS_BATT_SOC, 2);

  rs485readSuccess = result == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {
    this->floatVars[SolarTracerVariables::BATTERY_SOC] = this->node.getResponseBuffer(0x00) / 1.0f;
    this->floatVars[SolarTracerVariables::REMOTE_BATTERY_TEMP] = this->node.getResponseBuffer(0x01) / 100.0f;
  }

  this->setVariableReadReady(2, rs485readSuccess,
                             SolarTracerVariables::BATTERY_SOC,
                             SolarTracerVariables::REMOTE_BATTERY_TEMP);
}

void EPEVERSolarTracer::AddressRegistry_331B()
{
  uint8_t result = this->node.readInputRegisters(0x331B, 2);

  rs485readSuccess = result == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {
    this->floatVars[SolarTracerVariables::BATTERY_OVERALL_CURRENT] = (this->node.getResponseBuffer(0x00) | this->node.getResponseBuffer(0x01) << 16) / 100.0f;
  }

  this->setVariableReadReady(SolarTracerVariables::BATTERY_OVERALL_CURRENT, rs485readSuccess);
}

void EPEVERSolarTracer::AddressRegistry_9003()
{
  uint8_t result = this->node.readHoldingRegisters(MODBUS_ADDRESS_BATTERY_OVERVOLTAGE_DISCONNECT, 12);

  rs485readSuccess = result == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {
    this->floatVars[SolarTracerVariables::BATTERY_OVER_VOLTAGE_DISCONNECT] = this->node.getResponseBuffer(0x00) / 100.0f;
    this->floatVars[SolarTracerVariables::BATTERY_CHARGING_LIMIT_VOLTAGE] = this->node.getResponseBuffer(0x01) / 100.0f;
    this->floatVars[SolarTracerVariables::BATTERY_OVER_VOLTAGE_CONNECT] = this->node.getResponseBuffer(0x02) / 100.0f;
    this->floatVars[SolarTracerVariables::BATTERY_EQUALIZATION_VOLTAGE] = this->node.getResponseBuffer(0x03) / 100.0f;
    this->floatVars[SolarTracerVariables::BATTERY_BOOST_VOLTAGE] = this->node.getResponseBuffer(0x04) / 100.0f;
    this->floatVars[SolarTracerVariables::BATTERY_FLOAT_VOLTAGE] = this->node.getResponseBuffer(0x05) / 100.0f;
    this->floatVars[SolarTracerVariables::BATTERY_FLOAT_MIN_VOLTAGE] = this->node.getResponseBuffer(0x06) / 100.0f;
    this->floatVars[SolarTracerVariables::BATTERY_LOW_VOLTAGE_RECONNECT] = this->node.getResponseBuffer(0x07) / 100.0f;
    this->floatVars[SolarTracerVariables::BATTERY_UNDER_VOLTAGE_RESET] = this->node.getResponseBuffer(0x08) / 100.0f;
    this->floatVars[SolarTracerVariables::BATTERY_UNDER_VOLTAGE_SET] = this->node.getResponseBuffer(0x09) / 100.0f;
    this->floatVars[SolarTracerVariables::BATTERY_LOW_VOLTAGE_DISCONNECT] = this->node.getResponseBuffer(0x0A) / 100.0f;
    this->floatVars[SolarTracerVariables::BATTERY_DISCHARGING_LIMIT_VOLTAGE] = this->node.getResponseBuffer(0x0B) / 100.0f;
  }

  this->setVariableReadReady(12, rs485readSuccess,
                             SolarTracerVariables::BATTERY_OVER_VOLTAGE_DISCONNECT,
                             SolarTracerVariables::BATTERY_CHARGING_LIMIT_VOLTAGE,
                             SolarTracerVariables::BATTERY_OVER_VOLTAGE_CONNECT,
                             SolarTracerVariables::BATTERY_EQUALIZATION_VOLTAGE,
                             SolarTracerVariables::BATTERY_BOOST_VOLTAGE,
                             SolarTracerVariables::BATTERY_FLOAT_VOLTAGE,
                             SolarTracerVariables::BATTERY_FLOAT_MIN_VOLTAGE,
                             SolarTracerVariables::BATTERY_LOW_VOLTAGE_RECONNECT,
                             SolarTracerVariables::BATTERY_UNDER_VOLTAGE_RESET,
                             SolarTracerVariables::BATTERY_UNDER_VOLTAGE_SET,
                             SolarTracerVariables::BATTERY_LOW_VOLTAGE_DISCONNECT,
                             SolarTracerVariables::BATTERY_DISCHARGING_LIMIT_VOLTAGE);
}

void EPEVERSolarTracer::fetchAddressStatusVariables()
{
  uint8_t result = this->node.readInputRegisters(MODBUS_ADDRESS_BATTERY_STATUS, 3);

  rs485readSuccess = result == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {
    uint16_t batteryStatus = node.getResponseBuffer(0x00);
    if (batteryStatus)
    {
      // fault
      switch (batteryStatus & 3)
      {
      case 1:
        strcpy(this->charVars[SolarTracerVariables::BATTERY_STATUS_TEXT], "! OVER VOLT");
        break;
      case 2:
        strcpy(this->charVars[SolarTracerVariables::BATTERY_STATUS_TEXT], "! UNDER VOLT");
        break;
      case 3:
        strcpy(this->charVars[SolarTracerVariables::BATTERY_STATUS_TEXT], "! LOW VOLT");
        break;
      case 4:
        strcpy(this->charVars[SolarTracerVariables::BATTERY_STATUS_TEXT], "! FAULT");
        break;
      }

      switch ((batteryStatus >> 4) & 3)
      {
      case 1:
        strcpy(this->charVars[SolarTracerVariables::BATTERY_STATUS_TEXT], "! OVER TEMP");
        break;
      case 2:
        strcpy(this->charVars[SolarTracerVariables::BATTERY_STATUS_TEXT], "! LOW TEMP");
        break;
      }

      if (batteryStatus >> 8)
      {
        strcpy(this->charVars[SolarTracerVariables::BATTERY_STATUS_TEXT], "! ABN BATT. RESIST.");
      }
    }
    else
    {
      strcpy(this->charVars[SolarTracerVariables::BATTERY_STATUS_TEXT], "Normal");
    }

    uint16_t chargingStatus = node.getResponseBuffer(0x01);
    if (chargingStatus & 0xFF0)
    {
      // doc says that bit2 is 0:Normal, 1:Faul
      // seems like this bit is behaving different
      // 0: Not charging, 1: Charging
      // FAULTs are detected by checking all fault bits bit13-4
      // chargingStatus & 0xFF0 == 0 means no fault
      if (chargingStatus & 16)
      {
        strcpy(this->charVars[SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT], "! PV INPUT SHORT");
      }
      else if (chargingStatus & 32)
      {
        strcpy(this->charVars[SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT], "! ?? D5"); // not specified in doc
      }
      else if (chargingStatus & 64)
      {
        strcpy(this->charVars[SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT], "! ?? D6"); // not specified in doc
      }
      else if (chargingStatus & 128)
      {
        strcpy(this->charVars[SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT], "! LOAD MOS. SHORT");
      }
      else if (chargingStatus & 256)
      {
        strcpy(this->charVars[SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT], "! LOAD SHORT");
      }
      else if (chargingStatus & 512)
      {
        strcpy(this->charVars[SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT], "! LOAD OVER CURR.");
      }
      else if (chargingStatus & 1024)
      {
        strcpy(this->charVars[SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT], "! INPUT OVER CURR.");
      }
      else if (chargingStatus & 2048)
      {
        strcpy(this->charVars[SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT], "! ANTI REV. MOS. SHORT");
      }
      else if (chargingStatus & 4096)
      {
        strcpy(this->charVars[SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT], "! CHRG./ ANTI REV. MOS. SHORT");
      }
      else if (chargingStatus & 8192)
      {
        strcpy(this->charVars[SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT], "! CHRG. MOS SHORT");
      }
      else
      {
        strcpy(this->charVars[SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT], "! ??");
      }
    }
    else
    {
      switch ((chargingStatus >> 2) & 3)
      {
      case 0:
        strcpy(this->charVars[SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT], "Standby");
        break;
      case 1:
        strcpy(this->charVars[SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT], "Float");
        break;
      case 2:
        strcpy(this->charVars[SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT], "Boost");
        break;
      case 3:
        strcpy(this->charVars[SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT], "Equalization");
        break;
      default:
        strcpy(this->charVars[SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT], "??");
      }
    }

    uint16_t dischargingStatus = node.getResponseBuffer(0x02);
    if (dischargingStatus & 2)
    {
      // fault
      if (dischargingStatus & 16)
      {
        strcpy(this->charVars[SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT], "! OUT OVER VOLT.");
      }
      else if (dischargingStatus & 32)
      {
        strcpy(this->charVars[SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT], "! BOOST OVER VOLT");
      }
      else if (dischargingStatus & 64)
      {
        strcpy(this->charVars[SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT], "! HV SIDE SHORT");
      }
      else if (dischargingStatus & 128)
      {
        strcpy(this->charVars[SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT], "! INPUT OVER VOLT.");
      }
      else if (dischargingStatus & 256)
      {
        strcpy(this->charVars[SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT], "! OUT VOLT. ABN");
      }
      else if (dischargingStatus & 512)
      {
        strcpy(this->charVars[SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT], "! UNABLE STOP DISC.");
      }
      else if (dischargingStatus & 1024)
      {
        strcpy(this->charVars[SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT], "! UNABLE DISC.");
      }
      else if (dischargingStatus & 2048)
      {
        strcpy(this->charVars[SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT], "! SHORT");
      }
      else
      {
        strcpy(this->charVars[SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT], "! ??");
      }
    }
    else
    {
      if (dischargingStatus & 1)
      {
        strcpy(this->charVars[SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT], "Running");
      }
      else
      {
        strcpy(this->charVars[SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT], "Standby");
      }
    }
  }

  this->setVariableReadReady(3, rs485readSuccess,
                             SolarTracerVariables::BATTERY_STATUS_TEXT,
                             SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT,
                             SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT);
}

void EPEVERSolarTracer::updateStats()
{
  uint8_t result = this->node.readInputRegisters(MODBUS_ADDRESS_STAT_MAX_PV_VOLTAGE_TODAY, 29);

  rs485readSuccess = result == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {

    this->floatVars[SolarTracerVariables::MINIMUM_PV_VOLTAGE_TODAY] = this->node.getResponseBuffer(0x00) / 100.0f;
    this->floatVars[SolarTracerVariables::MAXIMUM_PV_VOLTAGE_TODAY] = this->node.getResponseBuffer(0x01) / 100.0f;
    this->floatVars[SolarTracerVariables::MINIMUM_BATTERY_VOLTAGE_TODAY] = this->node.getResponseBuffer(0x02) / 100.0f;
    this->floatVars[SolarTracerVariables::MAXIMUM_BATTERY_VOLTAGE_TODAY] = this->node.getResponseBuffer(0x03) / 100.0f;

    this->floatVars[SolarTracerVariables::GENERATED_ENERGY_TODAY] = (this->node.getResponseBuffer(12) | this->node.getResponseBuffer(13) << 16) / 100.0f;
    this->floatVars[SolarTracerVariables::GENERATED_ENERGY_MONTH] = (this->node.getResponseBuffer(14) | this->node.getResponseBuffer(15) << 16) / 100.0f;
    this->floatVars[SolarTracerVariables::GENERATED_ENERGY_YEAR] = (this->node.getResponseBuffer(16) | this->node.getResponseBuffer(17) << 16) / 100.0f;
    this->floatVars[SolarTracerVariables::GENERATED_ENERGY_TOTAL] = (this->node.getResponseBuffer(18) | this->node.getResponseBuffer(19) << 16) / 100.0f;
  }

  this->setVariableReadReady(8, rs485readSuccess,
                             SolarTracerVariables::MINIMUM_PV_VOLTAGE_TODAY,
                             SolarTracerVariables::MAXIMUM_PV_VOLTAGE_TODAY,
                             SolarTracerVariables::MINIMUM_BATTERY_VOLTAGE_TODAY,
                             SolarTracerVariables::MAXIMUM_BATTERY_VOLTAGE_TODAY,
                             SolarTracerVariables::GENERATED_ENERGY_TODAY,
                             SolarTracerVariables::GENERATED_ENERGY_MONTH,
                             SolarTracerVariables::GENERATED_ENERGY_YEAR,
                             SolarTracerVariables::GENERATED_ENERGY_TOTAL);
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

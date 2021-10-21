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

#include <time.h>

#include "EPEVERSolarTracer.h"

#include <time.h>

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
  updateStats();
  this->AddressRegistry_3100();
  this->AddressRegistry_310D();
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
    updateStats();
    globalUpdateCounter = 0;
    break;
  default:
    switch (currentRealtimeUpdateCounter)
    {
    case 0:
      globalUpdateCounter++;
      currentRealtimeUpdateCounter = 6;
      this->AddressRegistry_3100();
      break;
    case 1:
      this->AddressRegistry_310D();
      break;
    case 2:
      this->AddressRegistry_3110();
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
    this->setVariableReadReady(variable, rs485readSuccess);
    return rs485readSuccess;
  case SolarTracerVariables::LOAD_MANUAL_ONOFF:
    value = this->readControllerSingleCoil(MODBUS_ADDRESS_LOAD_MANUAL_ONOFF);
    if (rs485readSuccess)
    {
      loadOnOff = value;
    }
    this->setVariableReadReady(variable, rs485readSuccess);
    return rs485readSuccess;
  case SolarTracerVariables::CHARGING_DEVICE_ONOFF:
    value = this->readControllerSingleCoil(MODBUS_ADDRESS_BATTERY_CHARGE_ONOFF);
    if (rs485readSuccess)
    {
      chargingDeviceOnOff = value;
    }
    this->setVariableReadReady(variable, rs485readSuccess);
    return rs485readSuccess;
  default:
    break;
  }

  return false;
}

bool EPEVERSolarTracer::writeBoolValue(SolarTracerVariables variable, bool value)
{
  if (!this->isVariableEnabled(variable))
  {
    return false;
  }

  switch (variable)
  {
  case SolarTracerVariables::LOAD_FORCE_ONOFF:
    return this->writeControllerSingleCoil(MODBUS_ADDRESS_LOAD_FORCE_ONOFF, value);
  case SolarTracerVariables::LOAD_MANUAL_ONOFF:
    return this->writeControllerSingleCoil(MODBUS_ADDRESS_LOAD_MANUAL_ONOFF, value);
  case SolarTracerVariables::CHARGING_DEVICE_ONOFF:
    return this->writeControllerSingleCoil(MODBUS_ADDRESS_BATTERY_CHARGE_ONOFF, value);
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

void EPEVERSolarTracer::AddressRegistry_3100()
{
  uint8_t result = this->node.readInputRegisters(MODBUS_ADDRESS_PV_VOLTAGE, 8);

  rs485readSuccess = result == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {

    pvvoltage = this->node.getResponseBuffer(0x00) / 100.0f;
    pvcurrent = this->node.getResponseBuffer(0x01) / 100.0f;
    pvpower = (this->node.getResponseBuffer(0x02) | this->node.getResponseBuffer(0x03) << 16) / 100.0f;
    bvoltage = this->node.getResponseBuffer(0x04) / 100.0f;
    battChargeCurrent = this->node.getResponseBuffer(0x05) / 100.0f;
    battChargePower = (this->node.getResponseBuffer(0x06) | this->node.getResponseBuffer(0x07) << 16) / 100.0f;

  }

  this->setVariableReadReady(6, rs485readSuccess,
                             SolarTracerVariables::PV_VOLTAGE,
                             SolarTracerVariables::PV_CURRENT,
                             SolarTracerVariables::PV_POWER,
                             SolarTracerVariables::BATTERY_VOLTAGE,
                             SolarTracerVariables::BATTERY_CHARGE_CURRENT,
                             SolarTracerVariables::BATTERY_CHARGE_POWER);
}

void EPEVERSolarTracer::AddressRegistry_310D()
{
  uint8_t result = this->node.readInputRegisters(0x310D, 3);

  rs485readSuccess = result == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {
    lcurrent = this->node.getResponseBuffer(0x00) / 100.0f;
    lpower = (this->node.getResponseBuffer(0x01) | this->node.getResponseBuffer(0x02) << 16) / 100.0f;
  }

  this->setVariableReadReady(2, rs485readSuccess,
                             SolarTracerVariables::LOAD_CURRENT,
                             SolarTracerVariables::LOAD_POWER);
}

void EPEVERSolarTracer::AddressRegistry_3110()
{
  uint8_t result = this->node.readInputRegisters(MODBUS_ADDRESS_BATT_TEMP, 3);

  rs485readSuccess = result == this->node.ku8MBSuccess;
  if (rs485readSuccess)
  {
    btemp = this->node.getResponseBuffer(0x00) / 100.0f;
    ctemp = this->node.getResponseBuffer(0x01) / 100.0f;
    htemp = this->node.getResponseBuffer(0x02) / 100.0f;
  }

  this->setVariableReadReady(3, rs485readSuccess,
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
    bremaining = this->node.getResponseBuffer(0x00) / 1.0f;
    rtemp = this->node.getResponseBuffer(0x01) / 100.0f;
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
    battOverallCurrent = (this->node.getResponseBuffer(0x00) | this->node.getResponseBuffer(0x01) << 16) / 100.0f;
  }

  this->setVariableReadReady(SolarTracerVariables::BATTERY_OVERALL_CURRENT, rs485readSuccess);
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
        strcpy(batteryStatusText, "! OVER VOLT");
        break;
      case 2:
        strcpy(batteryStatusText, "! UNDER VOLT");
        break;
      case 3:
       strcpy( batteryStatusText, "! LOW VOLT");
        break;
      case 4:
        strcpy(batteryStatusText, "! FAULT");
        break;
      }

      switch ((batteryStatus >> 4) & 3)
      {
      case 1:
        strcpy(batteryStatusText, "! OVER TEMP");
        break;
      case 2:
        strcpy(batteryStatusText, "! LOW TEMP");
        break;
      }

      if (batteryStatus >> 8)
      {
        strcpy(batteryStatusText, "! ABN BATT. RESIST.");
      }
    }
    else
    {
      strcpy(batteryStatusText, "Normal");
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
        strcpy(chargingStatusText ,"! PV INPUT SHORT");
      }
      else if (chargingStatus & 32)
      {
        strcpy(chargingStatusText, "! ?? D5"); // not specified in doc
      }
      else if (chargingStatus & 64)
      {
        strcpy(chargingStatusText, "! ?? D6"); // not specified in doc
      }
      else if (chargingStatus & 128)
      {
        strcpy(chargingStatusText, "! LOAD MOS. SHORT");
      }
      else if (chargingStatus & 256)
      {
        strcpy(chargingStatusText, "! LOAD SHORT");
      }
      else if (chargingStatus & 512)
      {
        strcpy(chargingStatusText , "! LOAD OVER CURR.");
      }
      else if (chargingStatus & 1024)
      {
        strcpy(chargingStatusText, "! INPUT OVER CURR.");
      }
      else if (chargingStatus & 2048)
      {
        strcpy(chargingStatusText , "! ANTI REV. MOS. SHORT");
      }
      else if (chargingStatus & 4096)
      {
        strcpy(chargingStatusText, "! CHRG./ ANTI REV. MOS. SHORT");
      }
      else if (chargingStatus & 8192)
      {
        strcpy(chargingStatusText, "! CHRG. MOS SHORT");
      }
      else
      {
        strcpy(chargingStatusText , "! ??");
      }
    }
    else
    {
      switch ((chargingStatus >> 2) & 3)
      {
      case 0:
        strcpy(chargingStatusText, "Standby");
        break;
      case 1:
        strcpy(chargingStatusText, "Float");
        break;
      case 2:
        strcpy(chargingStatusText, "Boost");
        break;
      case 3:
        strcpy(chargingStatusText, "Equalization");
        break;
      default:
        strcpy(chargingStatusText, "??");
      }
    }

    uint16_t dischargingStatus = node.getResponseBuffer(0x02);
    if (dischargingStatus & 2)
    {
      // fault
      if (dischargingStatus & 16)
      {
        strcpy(dischargingStatusText, "! OUT OVER VOLT.");
      }
      else if (dischargingStatus & 32)
      {
        strcpy(dischargingStatusText, "! BOOST OVER VOLT");
      }
      else if (dischargingStatus & 64)
      {
       strcpy( dischargingStatusText, "! HV SIDE SHORT");
      }
      else if (dischargingStatus & 128)
      {
        strcpy(dischargingStatusText, "! INPUT OVER VOLT.");
      }
      else if (dischargingStatus & 256)
      {
        strcpy(dischargingStatusText, "! OUT VOLT. ABN");
      }
      else if (dischargingStatus & 512)
      {
        strcpy(dischargingStatusText, "! UNABLE STOP DISC.");
      }
      else if (dischargingStatus & 1024)
      {
        strcpy(dischargingStatusText, "! UNABLE DISC.");
      }
      else if (dischargingStatus & 2048)
      {
        strcpy(dischargingStatusText, "! SHORT");
      }
      else
      {
        strcpy(dischargingStatusText, "! ??");
      }
    }
    else
    {
      if (dischargingStatus & 1)
      {
        strcpy(dischargingStatusText, "Running");
      }
      else
      {
        strcpy(dischargingStatusText, "Standby");
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

    stats_today_pv_volt_min = this->node.getResponseBuffer(0x00) / 100.0f;
    stats_today_pv_volt_max = this->node.getResponseBuffer(0x01) / 100.0f;
    stats_today_bat_volt_min = this->node.getResponseBuffer(0x02) / 100.0f;
    stats_today_bat_volt_max = this->node.getResponseBuffer(0x03) / 100.0f;

    stats_today_generated_energy = (this->node.getResponseBuffer(12) | this->node.getResponseBuffer(13) << 16) / 100.0f;
    stats_month_generated_energy = (this->node.getResponseBuffer(14) | this->node.getResponseBuffer(15) << 16) / 100.0f;
    stats_year_generated_energy = (this->node.getResponseBuffer(16) | this->node.getResponseBuffer(17) << 16) / 100.0f;
    stats_total_generated_energy = (this->node.getResponseBuffer(18) | this->node.getResponseBuffer(19) << 16) / 100.0f;
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

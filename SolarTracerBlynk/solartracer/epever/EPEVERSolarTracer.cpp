#include "EPEVERSolarTracer.h"

EPEVERSolarTracer::EPEVERSolarTracer(Stream &SerialCom, uint8_t max485_de, uint8_t max485_re_neg) 
: SolarTracer()
{
  this->node.begin(1, SerialCom);

  this->max485_re_neg = max485_re_neg;
  this->max485_de = max485_de;
  this->rs485readSuccess = true;

  pinMode(this->max485_re_neg, OUTPUT);
  pinMode(this->max485_de, OUTPUT);

  digitalWrite(this->max485_re_neg, 0);
  digitalWrite(this->max485_de, 0);

  // set this instance as the callback receiver
  this->node.setTransmissionCallable(this);
}

bool EPEVERSolarTracer::syncRealtimeClock(struct tm *ti)
{
  node.setTransmitBuffer(0, (ti->tm_min << 8) + ti->tm_sec);
  node.setTransmitBuffer(1, (ti->tm_mday << 8) + ti->tm_hour);
  node.setTransmitBuffer(2, ((ti->tm_year + 1900 - 2000) << 8) + ti->tm_mon + 1);

  uint8_t result = node.writeMultipleRegisters(MODBUS_ADDRESS_REALTIME_CLOCK, 3);

  if (result == this->node.ku8MBSuccess)
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
  this->AddressRegistry_3106();
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
      this->AddressRegistry_3106();
      break;
    case 2:
      this->AddressRegistry_310D();
      break;
    case 3:
      this->AddressRegistry_3110();
      this->AddressRegistry_311A();
      break;
    case 4:
      this->AddressRegistry_331B();
      break;
    case 5:
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
    if (rs485readSuccess)
    {
    }
    return rs485readSuccess;

  case SolarTracerVariables::LOAD_MANUAL_ONOFF:
    value = this->readControllerSingleCoil(MODBUS_ADDRESS_LOAD_MANUAL_ONOFF);
    if (rs485readSuccess)
    {
      loadOnOff = value;
    }
    return rs485readSuccess;
  case SolarTracerVariables::CHARGING_DEVICE_ONOFF:
    value = this->readControllerSingleCoil(MODBUS_ADDRESS_BATTERY_CHARGE_ONOFF);
    if (rs485readSuccess)
    {
      chargingDeviceOnOff = value;
    }
    return rs485readSuccess;
  default:
    break;
  }

  return false;
}

bool EPEVERSolarTracer::writeBoolValue(SolarTracerVariables variable, bool value)
{
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
  uint8_t result = this->node.readInputRegisters(MODBUS_ADDRESS_PV_VOLTAGE, 6);

  if (result == this->node.ku8MBSuccess)
  {

    pvvoltage = this->node.getResponseBuffer(0x00) / 100.0f;
    pvcurrent = this->node.getResponseBuffer(0x01) / 100.0f;
    pvpower = (this->node.getResponseBuffer(0x02) | this->node.getResponseBuffer(0x03) << 16) / 100.0f;
    bvoltage = this->node.getResponseBuffer(0x04) / 100.0f;
    battChargeCurrent = this->node.getResponseBuffer(0x05) / 100.0f;
  }
}

void EPEVERSolarTracer::AddressRegistry_3106()
{
  uint8_t result = this->node.readInputRegisters(0x3106, 2);

  if (result == this->node.ku8MBSuccess)
  {
    battChargePower = (this->node.getResponseBuffer(0x00) | this->node.getResponseBuffer(0x01) << 16) / 100.0f;
  }
}

void EPEVERSolarTracer::AddressRegistry_310D()
{
  uint8_t result = this->node.readInputRegisters(0x310D, 3);

  if (result == this->node.ku8MBSuccess)
  {
    lcurrent = this->node.getResponseBuffer(0x00) / 100.0f;
    lpower = (this->node.getResponseBuffer(0x01) | this->node.getResponseBuffer(0x02) << 16) / 100.0f;
  }
  else
  {
    rs485readSuccess = false;
  }
}

void EPEVERSolarTracer::AddressRegistry_3110()
{
  uint8_t result = this->node.readInputRegisters(MODBUS_ADDRESS_BATT_TEMP, 2);

  if (result == this->node.ku8MBSuccess)
  {
    btemp = this->node.getResponseBuffer(0x00) / 100.0f;
    ctemp = this->node.getResponseBuffer(0x01) / 100.0f;
  }
  else
  {
    rs485readSuccess = false;
  }
}

void EPEVERSolarTracer::AddressRegistry_311A()
{
  uint8_t result = this->node.readInputRegisters(MODBUS_ADDRESS_BATT_SOC, 2);

  if (result == this->node.ku8MBSuccess)
  {
    bremaining = this->node.getResponseBuffer(0x00) / 1.0f;
    rtemp = this->node.getResponseBuffer(0x01) / 100.0f;
  }
  else
  {
    rs485readSuccess = false;
  }
}

void EPEVERSolarTracer::AddressRegistry_331B()
{
  uint8_t result = this->node.readInputRegisters(0x331B, 2);

  if (result == this->node.ku8MBSuccess)
  {
    battOverallCurrent = (this->node.getResponseBuffer(0x00) | this->node.getResponseBuffer(0x01) << 16) / 100.0f;
  }
  else
  {
    rs485readSuccess = false;
  }
}

void EPEVERSolarTracer::fetchAddressStatusVariables()
{
  uint8_t result = this->node.readInputRegisters(MODBUS_ADDRESS_BATTERY_STATUS, 3);

  if (result == this->node.ku8MBSuccess)
  {
    uint16_t batteryStatus = node.getResponseBuffer(0x00);
    if (batteryStatus)
    {
      // fault
      batteryStatusText = "!";
      switch (batteryStatus & 3)
      {
      case 1:
        batteryStatusText += " OVER VOLT";
        break;
      case 2:
        batteryStatusText += " UNDER VOLT";
        break;
      case 3:
        batteryStatusText += " LOW VOLT";
        break;
      case 4:
        batteryStatusText += " FAULT";
        break;
      }

      switch ((batteryStatus >> 4) & 3)
      {
      case 1:
        batteryStatusText += " OVER TEMP";
        break;
      case 2:
        batteryStatusText += " LOW TEMP";
        break;
      }

      if (batteryStatus >> 8)
      {
        batteryStatusText += " ABN BATT. RESIST.";
      }
    }
    else
    {
      batteryStatusText = "Normal";
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
        chargingStatusText = "! PV INPUT SHORT";
      }
      else if (chargingStatus & 32)
      {
        chargingStatusText = "! ?? D5"; // not specified in doc
      }
      else if (chargingStatus & 64)
      {
        chargingStatusText = "! ?? D6"; // not specified in doc
      }
      else if (chargingStatus & 128)
      {
        chargingStatusText = "! LOAD MOS. SHORT";
      }
      else if (chargingStatus & 256)
      {
        chargingStatusText = "! LOAD SHORT";
      }
      else if (chargingStatus & 512)
      {
        chargingStatusText = "! LOAD OVER CURR.";
      }
      else if (chargingStatus & 1024)
      {
        chargingStatusText = "! INPUT OVER CURR.";
      }
      else if (chargingStatus & 2048)
      {
        chargingStatusText = "! ANTI REV. MOS. SHORT";
      }
      else if (chargingStatus & 4096)
      {
        chargingStatusText = "! CHRG./ ANTI REV. MOS. SHORT";
      }
      else if (chargingStatus & 8192)
      {
        chargingStatusText = "! CHRG. MOS SHORT";
      }
      else
      {
        chargingStatusText = "! ??";
      }
    }
    else
    {
      switch ((chargingStatus >> 2) & 3)
      {
      case 0:
        chargingStatusText = "Stanby";
        break;
      case 1:
        chargingStatusText = "Float";
        break;
      case 2:
        chargingStatusText = "Boost";
        break;
      case 3:
        chargingStatusText = "Equalisation";
        break;
      }
    }

    uint16_t dischargingStatus = node.getResponseBuffer(0x02);
    if (dischargingStatus & 2)
    {
      // fault
      if (dischargingStatus & 16)
      {
        dischargingStatusText = "! OUT OVER VOLT.";
      }
      else if (dischargingStatus & 32)
      {
        dischargingStatusText = "! BOOST OVER VOLT";
      }
      else if (dischargingStatus & 64)
      {
        dischargingStatusText = "! HV SIDE SHORT";
      }
      else if (dischargingStatus & 128)
      {
        dischargingStatusText = "! INPUT OVER VOLT.";
      }
      else if (dischargingStatus & 256)
      {
        dischargingStatusText = "! OUT VOLT. ABN";
      }
      else if (dischargingStatus & 512)
      {
        dischargingStatusText = "! UNABLE STOP DISC.";
      }
      else if (dischargingStatus & 1024)
      {
        dischargingStatusText = "! UNABLE DISC.";
      }
      else if (dischargingStatus & 2048)
      {
        dischargingStatusText = "! SHORT";
      }
      else
      {
        dischargingStatusText = "! ??";
      }
    }
    else
    {
      if (dischargingStatus & 1)
      {
        dischargingStatusText = "Running";
      }
      else
      {
        dischargingStatusText = "Standby";
      }
    }
  }
  else
  {
    rs485readSuccess = false;
  }
}

void EPEVERSolarTracer::updateStats()
{
  uint8_t result = this->node.readInputRegisters(MODBUS_ADDRESS_STAT_MAX_PV_VOLTAGE_TODAY, 29);

  if (result == this->node.ku8MBSuccess)
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

#include <ModbusMaster_obj.h>
#include <ModbusMasterCallable.h>

#ifndef SolarTracer_h
#define SolarTracer_h



typedef enum {
  PV_VOLTAGE,
  PV_POWER,
  PV_CURRENT,
  LOAD_CURRENT,
  LOAD_POWER,
  BATTERY_TEMP,
  BATTERY_VOLTAGE,
  BATTERY_SOC,
  CONTROLLER_TEMP,
  BATTERY_CHARGE_CURRENT,
  BATTERY_CHARGE_POWER,
  BATTERY_OVERALL_CURRENT,
  REALTIME_CLOCK,
  LOAD_FORCE_ONOFF,
  LOAD_MANUAL_ONOFF,
  // CHARGING_MODE,
  REMOTE_BATTERY_TEMP,
  GENERATED_ENERGY_TODAY,
  GENERATED_ENERGY_MONTH,
  GENERATED_ENERGY_YEAR,
  GENERATED_ENERGY_TOTAL,
  MAXIMUM_PV_VOLTAGE_TODAY,
  MINIMUM_PV_VOLTAGE_TODAY,
  MAXIMUM_BATTERY_VOLTAGE_TODAY,
  MINIMUM_BATTERY_VOLTAGE_TODAY,
} SolarTracerVariables;


class SolarTracer : public ModbusMasterCallable {
  public:

    static const uint16_t MODBUS_ADDRESS_PV_VOLTAGE 	= 					0x3100;
    static const uint16_t MODBUS_ADDRESS_PV_POWER = 						0x3102;
    static const uint16_t MODBUS_ADDRESS_PV_CURRENT = 					0x3101;
    static const uint16_t MODBUS_ADDRESS_LOAD_CURRENT = 					0x310D;
    static const uint16_t MODBUS_ADDRESS_LOAD_POWER = 					0x310E;
    static const uint16_t MODBUS_ADDRESS_BATT_TEMP = 					0x3110;
    static const uint16_t MODBUS_ADDRESS_BATT_VOLTAGE = 					0x3104;
    static const uint16_t MODBUS_ADDRESS_BATT_SOC = 						0x311A;
    static const uint16_t MODBUS_ADDRESS_BATTERY_CHARGE_CURRENT = 		0x3105;
    static const uint16_t MODBUS_ADDRESS_BATTERY_CHARGE_POWER = 			0x3106;
    static const uint16_t MODBUS_ADDRESS_BATTERY_OVERALL_CURRENT = 		0x331B;
    static const uint16_t MODBUS_ADDRESS_LOAD_FORCE_ONOFF =				0x0006;
    static const uint16_t MODBUS_ADDRESS_LOAD_MANUAL_ONOFF =				0x0002;
    static const uint16_t MODBUS_ADDRESS_BATTERY_CHARGE_ONOFF =			0x0000;
    static const uint16_t MODBUS_ADDRESS_STAT_MAX_PV_VOLTAGE_TODAY =    0x3300;
    static const uint16_t MODBUS_ADDRESS_STAT_GENERATED_ENERGY_TODAY =		0x330C;
    static const uint16_t MODBUS_ADDRESS_STAT_GENERATED_ENERGY_MONTH =		0x330E;
    static const uint16_t MODBUS_ADDRESS_STAT_GENERATED_ENERGY_YEAR =		0x3310;
    static const uint16_t MODBUS_ADDRESS_CHARGING_MODE =    0x3008;
    static const uint16_t MODBUS_ADDRESS_CONTROLLER_TEMP =    0x3111;
    static const uint16_t MODBUS_ADDRESS_REMOTE_BATTERY_TEMP =    0x311B;
    static const uint16_t MODBUS_ADDRESS_REALTIME_CLOCK =    0x9013;




    ModbusMaster node;

    SolarTracer(Stream & SerialCom, uint8_t max485_de, uint8_t max485_re_neg) {
      this->node.begin(1, SerialCom);
      // callbacks to toggle DE + RE on MAX485

      this->max485_re_neg = max485_re_neg;
      this->max485_de = max485_de;

      pinMode(this->max485_re_neg, OUTPUT);
      pinMode(this->max485_de, OUTPUT);

      digitalWrite(this->max485_re_neg, 0);
      digitalWrite(this->max485_de, 0);

      this->node.setTransmissionCallable(this);

    }

    bool syncRealtimeClock(struct tm *ti) {
      node.setTransmitBuffer(0, (ti->tm_min << 8) + ti->tm_sec);
      node.setTransmitBuffer(1, (ti->tm_mday << 8) + ti->tm_hour);
      node.setTransmitBuffer(2, ((ti->tm_year + 1900 - 2000) << 8) + ti->tm_mon + 1);

      uint8_t result = node.writeMultipleRegisters(MODBUS_ADDRESS_REALTIME_CLOCK, 3);


      if (result == this->node.ku8MBSuccess) {
        this->node.getResponseBuffer(0x00);
        return true;
      }
      return false;
    }

    void fetchAllValues() {
      updateStats();
      this->AddressRegistry_3100();
      this->AddressRegistry_3106();
      this->AddressRegistry_310D();
      this->AddressRegistry_311A();
      this->AddressRegistry_331B();
      this->fetchValue(SolarTracerVariables::LOAD_MANUAL_ONOFF);
    }


    float battChargeCurrent, battDischargeCurrent, battOverallCurrent, battChargePower;
    float bvoltage, ctemp, btemp, bremaining, lpower, lcurrent, pvvoltage, pvcurrent, pvpower;
    float stats_today_pv_volt_min, stats_today_pv_volt_max,  stats_today_bat_volt_min, stats_today_bat_volt_max, stats_today_generated_energy, stats_month_generated_energy, stats_year_generated_energy, stats_total_generated_energy;
    float loadOnOff;

    uint16_t globalUpdateCounter = 0;
    uint8_t currentRealtimeUpdateCounter = 0;

    void onModbusPreTransmission() {
      digitalWrite(this->max485_re_neg, 1);
      digitalWrite(this->max485_de, 1);
    }

    virtual void onModbusIdle(){
      // nothing to do here!
    }

    void onModbusPostTransmission() {
      digitalWrite(this->max485_re_neg, 0);
      digitalWrite(this->max485_de, 0);
    }

    bool updateRun() {
      switch (globalUpdateCounter) {
        case 360:
          // update statistics
          updateStats();
          globalUpdateCounter = 0;
          break;
        default:
          switch (currentRealtimeUpdateCounter) {
            case 0:
              globalUpdateCounter ++;
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
              this->AddressRegistry_311A();
              break;
            case 4:
              this->AddressRegistry_331B();
              break;
            case 5:
              this->fetchValue(SolarTracerVariables::LOAD_MANUAL_ONOFF);
          }
          currentRealtimeUpdateCounter --;

      }

      return rs485readSuccess;


    }

    float getFloatValue(SolarTracerVariables variable) {
      switch (variable) {
        case SolarTracerVariables::PV_VOLTAGE:
          return pvvoltage;
        case SolarTracerVariables::PV_POWER:
          return pvpower;
        case SolarTracerVariables::PV_CURRENT:
          return pvcurrent;
        case SolarTracerVariables::LOAD_CURRENT:
          return lcurrent;
        case SolarTracerVariables::LOAD_POWER:
          return lpower;
        case SolarTracerVariables::BATTERY_TEMP:
          return btemp;
        case SolarTracerVariables::BATTERY_VOLTAGE:
          return bvoltage;
        case SolarTracerVariables::BATTERY_SOC:
          return bremaining;
        case SolarTracerVariables::CONTROLLER_TEMP:
          return ctemp;
        case SolarTracerVariables::BATTERY_CHARGE_CURRENT:
          return battChargeCurrent;
        case SolarTracerVariables::BATTERY_CHARGE_POWER:
          return battChargePower;
        case SolarTracerVariables::BATTERY_OVERALL_CURRENT:
          return battOverallCurrent;
        case SolarTracerVariables::LOAD_MANUAL_ONOFF:
          return loadOnOff;
        case SolarTracerVariables::GENERATED_ENERGY_TODAY:
          return stats_today_generated_energy;
        case SolarTracerVariables::GENERATED_ENERGY_MONTH:
          return stats_month_generated_energy;
        case SolarTracerVariables::GENERATED_ENERGY_YEAR:
          return stats_year_generated_energy;
        case SolarTracerVariables::GENERATED_ENERGY_TOTAL:
          return stats_total_generated_energy;
        case SolarTracerVariables::MAXIMUM_PV_VOLTAGE_TODAY:
          return stats_today_pv_volt_max;
        case SolarTracerVariables::MINIMUM_PV_VOLTAGE_TODAY:
          return stats_today_pv_volt_min;
        case SolarTracerVariables::MAXIMUM_BATTERY_VOLTAGE_TODAY:
          return stats_today_bat_volt_max;
        case SolarTracerVariables::MINIMUM_BATTERY_VOLTAGE_TODAY:
          return stats_today_bat_volt_min;
        default:
        	break;
      }
      // should raise an exception
      return 0.0;
    }


    bool fetchValue(SolarTracerVariables variable) {
      bool value;
      switch (variable) {
        case SolarTracerVariables::LOAD_FORCE_ONOFF:
          value =  this->readControllerSingleCoil(MODBUS_ADDRESS_LOAD_FORCE_ONOFF);
          if (rs485readSuccess) {

          }
          return rs485readSuccess;

        case SolarTracerVariables::LOAD_MANUAL_ONOFF:
          value =  this->readControllerSingleCoil(MODBUS_ADDRESS_LOAD_MANUAL_ONOFF);
          if (rs485readSuccess) {
            loadOnOff = value;
          }
          return rs485readSuccess;
		default:
			break;

      }

      return false;
    }


    bool writeBoolValue(SolarTracerVariables variable, bool value) {
      switch (variable){
        case SolarTracerVariables::LOAD_FORCE_ONOFF:
          return this->writeControllerSingleCoil(MODBUS_ADDRESS_LOAD_FORCE_ONOFF, value);
        case SolarTracerVariables::LOAD_MANUAL_ONOFF:
          return this->writeControllerSingleCoil(MODBUS_ADDRESS_LOAD_MANUAL_ONOFF, value);
        default:
        	break;
      }

      return false;
    }

    bool readControllerSingleCoil(uint16_t address) {
      uint8_t result = this->node.readCoils(address, 1);

      rs485readSuccess = result == this->node.ku8MBSuccess;
      if (rs485readSuccess) {
        return (node.getResponseBuffer(0x00) > 0);
      }
      return false;
    }

    bool writeControllerSingleCoil(uint16_t address, bool value) {
      uint8_t result = this->node.writeSingleCoil(address, value);

      if (result == this->node.ku8MBSuccess) {
        this->node.getResponseBuffer(0x00);
        return true;
      }
      return false;
    }

    void AddressRegistry_3100() {
      uint8_t result = this->node.readInputRegisters(MODBUS_ADDRESS_PV_VOLTAGE, 6);

      if (result == this->node.ku8MBSuccess) {

        pvvoltage = this->node.getResponseBuffer(0x00) / 100.0f;

        pvcurrent = this->node.getResponseBuffer(0x01) / 100.0f;

        pvpower = (this->node.getResponseBuffer(0x02) | this->node.getResponseBuffer(0x03) << 16) / 100.0f;

        bvoltage = this->node.getResponseBuffer(0x04) / 100.0f;

        battChargeCurrent = this->node.getResponseBuffer(0x05) / 100.0f;
      }
    }

    void AddressRegistry_3106()
    {
      uint8_t result = this->node.readInputRegisters(0x3106, 2);

      if (result == this->node.ku8MBSuccess) {
        battChargePower = (this->node.getResponseBuffer(0x00) | this->node.getResponseBuffer(0x01) << 16)  / 100.0f;
      }
    }

    void AddressRegistry_310D()
    {
      uint8_t result = this->node.readInputRegisters(0x310D, 3);

      if (result == this->node.ku8MBSuccess) {
        lcurrent = this->node.getResponseBuffer(0x00) / 100.0f;

        lpower = (this->node.getResponseBuffer(0x01) | this->node.getResponseBuffer(0x02) << 16) / 100.0f;
      } else {
        rs485DataReceived = false;
      }
    }

    void AddressRegistry_311A() {
      uint8_t result = this->node.readInputRegisters(0x311A, 2);

      if (result == this->node.ku8MBSuccess) {
        bremaining = this->node.getResponseBuffer(0x00) / 1.0f;
        btemp = this->node.getResponseBuffer(0x01) / 100.0f;
      } else {
        rs485DataReceived = false;
      }
    }

    void AddressRegistry_331B() {
      uint8_t result = this->node.readInputRegisters(0x331B, 2);

      if (result == this->node.ku8MBSuccess) {
        battOverallCurrent = (this->node.getResponseBuffer(0x00) | this->node.getResponseBuffer(0x01) << 16) / 100.0f;
      } else {
        rs485DataReceived = false;
      }
    }

    void updateStats() {
      uint8_t result = this->node.readInputRegisters(MODBUS_ADDRESS_STAT_MAX_PV_VOLTAGE_TODAY, 29);

      if (result == this->node.ku8MBSuccess) {

        stats_today_pv_volt_min = this->node.getResponseBuffer(0x00) / 100.0f;
        stats_today_pv_volt_max = this->node.getResponseBuffer(0x01) / 100.0f;
        stats_today_bat_volt_min = this->node.getResponseBuffer(0x02)  / 100.0f;
        stats_today_bat_volt_max = this->node.getResponseBuffer(0x03) / 100.0f;

        stats_today_generated_energy = (this->node.getResponseBuffer(12) | this->node.getResponseBuffer(13) << 16) / 100.0f;
        stats_month_generated_energy = (this->node.getResponseBuffer(14) | this->node.getResponseBuffer(15) << 16) / 100.0f;
        stats_year_generated_energy = (this->node.getResponseBuffer(16) | this->node.getResponseBuffer(17) << 16) / 100.0f;
        stats_total_generated_energy = (this->node.getResponseBuffer(18) | this->node.getResponseBuffer(19) << 16) / 100.0f;
      }
    }

  protected:
    uint16_t max485_re_neg, max485_de;
    bool rs485DataReceived, rs485readSuccess;



};

#endif

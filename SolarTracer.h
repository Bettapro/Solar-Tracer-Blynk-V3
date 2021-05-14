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
  BATTERY_STATUS_TEXT,
  CHARGING_EQUIPMENT_STATUS_TEXT,
  DISCHARGING_EQUIPMENT_STATUS_TEXT,
} SolarTracerVariables;


class SolarTracer {
  public:
    SolarTracer() {}

    String getStringValue(SolarTracerVariables variable) {
      switch (variable) {
        case SolarTracerVariables::BATTERY_STATUS_TEXT:
          return batteryStatusText;
        case SolarTracerVariables::CHARGING_EQUIPMENT_STATUS_TEXT:
          return chargingStatusText;
        case SolarTracerVariables::DISCHARGING_EQUIPMENT_STATUS_TEXT:
          return dischargingStatusText;
        default:
          break;
      }
      // should raise an exception?
      return "";
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
        case SolarTracerVariables::REMOTE_BATTERY_TEMP:
          return rtemp;
        default:
          break;
      }
      // should raise an exception?
      return 0.0;
    }


    virtual bool fetchValue(SolarTracerVariables variable) = 0;
    virtual bool syncRealtimeClock(struct tm *ti) = 0;
    virtual void fetchAllValues() = 0;
    virtual bool updateRun() = 0;
    virtual bool writeBoolValue(SolarTracerVariables variable, bool value) = 0;

  protected:
    float battChargeCurrent, battDischargeCurrent, battOverallCurrent, battChargePower;
    float bvoltage, rtemp, ctemp, btemp, bremaining, lpower, lcurrent, pvvoltage, pvcurrent, pvpower;
    float stats_today_pv_volt_min, stats_today_pv_volt_max,  stats_today_bat_volt_min, stats_today_bat_volt_max, stats_today_generated_energy, stats_month_generated_energy, stats_year_generated_energy, stats_total_generated_energy;
    float loadOnOff;

    String batteryStatusText, dischargingStatusText, chargingStatusText;

};

#endif

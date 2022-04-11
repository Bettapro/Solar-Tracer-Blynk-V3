
#include "../../incl/include_all_core.h"

#ifdef USE_EXTERNAL_HEAVY_LOAD_CURRENT_METER

#include "LoadCurrentOverwrite.h"

float LoadCurrentOverwrite::totalLoadEnergy = 0;
LinearSensHallCurrent *LoadCurrentOverwrite::sensor = nullptr;
#ifdef USE_EXTERNAL_HEAVY_LOAD_CURRENT_METER_ADS1015_ADC
ADS1015 *LoadCurrentOverwrite::ads1015 = nullptr;
#endif
unsigned long LoadCurrentOverwrite::lastRunMillis = 0;

#endif

#pragma once

#include "../incl/project_config.h"

#ifdef USE_BLYNK
    BlynkTimer* mainTimer = new BlynkTimer();
#else
    SimpleTimer * mainTimer = new SimpleTimer();
#endif

void setTimerInterval(){
    
}

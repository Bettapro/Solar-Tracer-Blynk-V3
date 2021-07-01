#pragma once

#include "../incl/project_config.h"

uint8_t internalStatus = 0;


void setStatusError(const uint8_t status)
{
  if ((internalStatus & status) == 0)
  {
    uint8_t tStatus = internalStatus + status;
#ifdef USE_STATUS_LED
    notifyStatusLed(tStatus);
#endif
    internalStatus = tStatus;
  }
}

void clearStatusError(const uint8_t status)
{
  if ((internalStatus & status) > 0)
  {
    uint8_t tStatus = internalStatus - status;
#ifdef USE_STATUS_LED
    notifyStatusLed(tStatus);
#endif
    internalStatus = tStatus;
  }
}
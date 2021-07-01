#pragma once
#include "../incl/project_config.h"

SolarTracer *thisController;


void controllerSetup(){
     thisController = new SOLAR_TRACER_INSTANCE;
}


void updateSolarController()
{

  if (thisController->updateRun())
  {
    debugPrintln("Update Solar-Tracer SUCCESS!");
    clearStatusError(STATUS_ERR_SOLAR_TRACER_NO_COMMUNICATION);
  }
  else
  {
    debugPrintln("Update Solar-Tracer FAILED!");
    setStatusError(STATUS_ERR_SOLAR_TRACER_NO_COMMUNICATION);
  }
}
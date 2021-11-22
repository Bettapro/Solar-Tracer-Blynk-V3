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

#pragma once
#include "../incl/project_config.h"

SolarTracer *thisController;

void controllerSetup()
{
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
    debugPrintf("Update Solar-Tracer FAILED! [err=%i]", thisController->getLastControllerCommunicationStatus());
    debugPrintln();
    setStatusError(STATUS_ERR_SOLAR_TRACER_NO_COMMUNICATION);
  }
}
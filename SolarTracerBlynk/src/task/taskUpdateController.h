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
#ifndef TASK_UPDATE_CONTROLLER_H
#define TASK_UPDATE_CONTROLLER_H

#include "src/incl/include.h"

void updateControllerRun() {
    debugPrint("Update Solar-Tracer ");
    if (Controller::getInstance().getSolarController()->updateRun()) {
        debugPrintln(Text::ok);
        Controller::getInstance().setErrorFlag(STATUS_ERR_SOLAR_TRACER_NO_COMMUNICATION, false);
    } else {
        debugPrintf(true, Text::errorWithCodeInt, STATUS_ERR_SOLAR_TRACER_NO_COMMUNICATION, Controller::getInstance().getSolarController()->getLastControllerCommunicationStatus());
        Controller::getInstance().setErrorFlag(STATUS_ERR_SOLAR_TRACER_NO_COMMUNICATION, true);
    }
}


#endif
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

#ifdef USE_NTP_SERVER
void setupDatetimeFromNTP()
{
    debugPrint("Waiting for NTP time...");
    configTzTime(CURRENT_TIMEZONE, NTP_SERVER_CONNECT_TO);

    while (time(nullptr) < 100000ul)
    {
        debugPrint(".");
        delay(500);
    }
    debugPrintln("OK");
}
#endif

struct tm *getMyNowTm()
{
    time_t tnow = time(nullptr) + 1;
    return localtime(&tnow);
}
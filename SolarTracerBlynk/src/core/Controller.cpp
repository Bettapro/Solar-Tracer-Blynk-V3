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

#include "Controller.h"

void Controller::setErrorFlag(const uint8_t status, bool error)
{
    if (((this->internalStatus & status) > 0) != error)
    {
        uint8_t tStatus = this->internalStatus + (error ? 1 : -1) * status;
#ifdef USE_STATUS_LED
        notifyStatusLed(tStatus);
#endif
        this->internalStatus = tStatus;
    }
}

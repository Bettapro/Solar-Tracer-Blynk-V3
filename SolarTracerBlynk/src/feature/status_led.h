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

extern SimpleTimer* mainTimer;

int ledActTimer = -1;
bool ledStatus = true;

void ledSetupStart()
{
    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, HIGH);
}

void ledSetupStop()
{
    digitalWrite(STATUS_LED_PIN, LOW);
}

void ledTimerCallback()
{
    digitalWrite(STATUS_LED_PIN, ledStatus);
    ledStatus = !ledStatus;
}

void changeStatusLedTimerInterval(unsigned long newInterval)
{
    if (newInterval <= 0)
    {
        if (ledActTimer >= 0)
        {
            mainTimer->deleteTimer(ledActTimer);
            digitalWrite(STATUS_LED_PIN, LOW);
            ledActTimer = -1;
        }
        return;
    }
    if(ledActTimer >= 0){
         mainTimer->deleteTimer(ledActTimer);
    }
    ledActTimer = mainTimer->setInterval(newInterval, ledTimerCallback);
}

void notifyStatusLed(uint8_t newStatus)
{
    changeStatusLedTimerInterval(newStatus == 0 ? 2000 : 0);
}

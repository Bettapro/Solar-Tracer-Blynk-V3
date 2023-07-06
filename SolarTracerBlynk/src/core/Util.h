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

#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>

class Util {
    public:
        static char *intToChar(int v, char *buf);
        static char *intToChar(int v) {
            return intToChar(v, sharedBuffer);
        }
        static char *floatToChar(float v, char *buf);
        static char *floatToChar(float v) {
            return floatToChar(v, sharedBuffer);
        }

        static uint8_t digits(int number);

        /**
         * Shared buffer, use it with caution as it will be reused
         *
         */
        static char *sharedBuffer;
};
#endif
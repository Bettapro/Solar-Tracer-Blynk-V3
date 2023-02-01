

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

#include "Util.h"

#define UTIL_MAX_DEC_DIV 1000


uint8_t Util::digits(int number){
    if (number < 0) return Util::digits(-number);
    if (number < 10) return 1;
    return 1 + Util::digits(number / 10);
}

char * Util::sharedBuffer = new char[256];

char *Util::intToChar(int v, char *buf)
{
    itoa(v, buf, 10);
    return buf;
}

char *Util::floatToChar(float v, char *buf)
{
    uint8_t index = 0;
    if(v < 0){
        buf[index++] = '-';
    }

    int iValue= (int) v;
    int dValue = (v - iValue) * UTIL_MAX_DEC_DIV;
    dValue = dValue < 0 ? -dValue : dValue;

    itoa(iValue, buf + index, 10);
    index += Util::digits(iValue);


    if(dValue >= 1){
        buf[index++] = '.';
        int decMissPosition = Util::digits(UTIL_MAX_DEC_DIV)- Util::digits(dValue);
        while (--decMissPosition > 0)
        {
            buf[index++] = '0';
        }
        itoa(dValue, buf + index, 10);
        
    }
    return buf;
}



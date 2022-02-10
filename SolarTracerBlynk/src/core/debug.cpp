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

#include "debug.h"
#include <stdio.h> // vsnprintf

#include "../incl/include_all_core.h"


#define DEBUG_REGISTER_CALLBACKS_MAX 2



uint8_t regCallbacksIndex = 0;
void (*regCallbacks[DEBUG_REGISTER_CALLBACKS_MAX])(String);

void debugAddRegisterCallback(void (*regCallback)(String))
{
    if (regCallbacksIndex < DEBUG_REGISTER_CALLBACKS_MAX)
    {
        regCallbacks[regCallbacksIndex++] = regCallback;
    }
}

void debugDispactMessageRegisterCallback(String msg){
    for(uint8_t index = 0; index < regCallbacksIndex; index ++){
        (*regCallbacks[index])(msg);
    }
}

void debugPrint(String message)
{
    BOARD_DEBUG_SERIAL_STREAM.print(message);
    for(uint8_t index = 0; index < regCallbacksIndex; index ++){
        (*regCallbacks[index])(message);
    }
}

void debugPrintln()
{
    debugPrint("\r\n");
}

void debugPrintln(String msgString)
{
    debugPrint(msgString +"\r\n");
}

void debugPrintln(const char *msgChar)
{
    debugPrintln(String(msgChar));
}

void debugPrint(const char *msgChar)
{
    debugPrint(String(msgChar));
}

void debugPrintln(int num)
{
    debugPrintln(String(num));
}

void debugPrint(const int num)
{
    debugPrint(String(num));
}

void debugPrintln(unsigned char num)
{
    debugPrintln(String(num));
}

void debugPrint(unsigned char num)
{
    debugPrint(String(num));
}

void debugPrintf(const char *format, ...)
{
    char buffer[256];

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 256, format, args);
    debugPrint(String(buffer));
    va_end(args);
}
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

void debugPrintln()
{
    BOARD_DEBUG_SERIAL_STREAM.println();
}

void debugPrintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    BOARD_DEBUG_SERIAL_STREAM.printf(format, args);
    va_end(args);
}

void debugPrintln(const String msgString)
{
    BOARD_DEBUG_SERIAL_STREAM.println(msgString);
}

void debugPrintln(const char *msgChar)
{
    BOARD_DEBUG_SERIAL_STREAM.println(msgChar);
}

void debugPrint(const char *msgChar)
{
    BOARD_DEBUG_SERIAL_STREAM.print(msgChar);
}

void debugPrintln(const int num)
{
    BOARD_DEBUG_SERIAL_STREAM.println(num);
}

void debugPrint(const int num)
{
    BOARD_DEBUG_SERIAL_STREAM.print(num);
}

void debugPrintln(const unsigned char num)
{
    BOARD_DEBUG_SERIAL_STREAM.println(num);
}

void debugPrint(const unsigned char num)
{
    BOARD_DEBUG_SERIAL_STREAM.print(num);
}

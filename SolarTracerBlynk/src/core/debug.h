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

#pragma once


#ifndef BOARD_DEBUG_SERIAL_STREAM
#define BOARD_DEBUG_SERIAL_STREAM Serial
#endif

#ifndef BOARD_DEBUG_SERIAL_STREAM_BAUDRATE
#define BOARD_DEBUG_SERIAL_STREAM_BAUDRATE 115200
#endif

#if defined(USE_SERIAL_STREAM) && !defined(BOARD_ST_SERIAL_STREAM)
#define BOARD_ST_SERIAL_STREAM Serial2
#endif

#if defined(USE_SERIAL_MAX485) && !defined(MAX485_DE)
#define MAX485_DE 18
#endif

#if defined(USE_SERIAL_MAX485) && !defined(MAX485_RE_NEG)
#define MAX485_RE_NEG 18
#endif



// removes the intellisens error for setenv,tzset
// see: https://community.platformio.org/t/identifier-is-undefined-setenv-tzset/16162
_VOID _EXFUN(tzset, (_VOID));
int _EXFUN(setenv, (const char *__string, const char *__value, int __overwrite));

#pragma once

#ifndef BOARD_DEBUG_SERIAL_STREAM
#define BOARD_DEBUG_SERIAL_STREAM Serial
#endif

#ifndef BOARD_DEBUG_SERIAL_STREAM_BAUDRATE
#define BOARD_DEBUG_SERIAL_STREAM_BAUDRATE 115200
#endif

#if defined(USE_SERIAL_STREAM) && !defined(BOARD_ST_SERIAL_STREAM)
#define BOARD_ST_SERIAL_STREAM Serial
#endif


#if defined(USE_SERIAL_MAX485) && !defined(MAX485_DE)
#define MAX485_DE 5
#endif

#if defined(USE_SERIAL_MAX485) && !defined(MAX485_RE_NEG)
#define MAX485_RE_NEG 5
#endif

#if defined(BOARD_ST_SERIAL_PIN_MAPPING_RX) | defined(BOARD_ST_SERIAL_PIN_MAPPING_TX)
#error "This board does not support HW serial pin mapping! [ disable BOARD_ST_SERIAL_PIN_MAPPING_RX, BOARD_ST_SERIAL_PIN_MAPPING_TX ]"
#endif

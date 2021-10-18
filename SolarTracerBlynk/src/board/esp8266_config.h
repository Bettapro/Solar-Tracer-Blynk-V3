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

#if defined(USE_HALL_AP_CONFIGURATION_TRIGGER)
#error "This board has no hall sensor to use! [ disable USE_HALL_AP_CONFIGURATION_TRIGGER ]"
#endif

#if defined(BOARD_ST_SERIAL_PIN_MAPPING_RX) | defined(BOARD_ST_SERIAL_PIN_MAPPING_TX)
#error "This board does not support HW serial pin mapping! [ disable BOARD_ST_SERIAL_PIN_MAPPING_RX, BOARD_ST_SERIAL_PIN_MAPPING_TX ]"
#endif

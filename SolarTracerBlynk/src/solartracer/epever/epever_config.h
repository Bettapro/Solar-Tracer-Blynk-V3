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

 // specify the serial port to use 
 #pragma once

#if !defined(USE_SERIAL_STREAM)
#error You must enable USE_SERIAL_STREAM !
#endif

#if !defined(BOARD_ST_SERIAL_STREAM)
#error You must specify a serial in BOARD_ST_SERIAL_STREAM!
#endif


#ifndef BOARD_ST_SERIAL_STREAM_BAUDRATE
    #define BOARD_ST_SERIAL_STREAM_BAUDRATE 115200 
#endif

#ifndef MODBUS_SLAVE_ID
    #define MODBUS_SLAVE_ID 1 
#endif

#ifndef CONTROLLER_UPDATE_MS_PERIOD
    #define CONTROLLER_UPDATE_MS_PERIOD 2000L 
#endif

#ifndef SERIAL_COMMUNICATION_TIMEOUT
    // will use the default value from library 
    #define SERIAL_COMMUNICATION_TIMEOUT 0 
#endif



#include "EPEVERSolarTracer.h"
#ifdef USE_SERIAL_MAX485
    #define SOLAR_TRACER_INSTANCE EPEVERSolarTracer(BOARD_ST_SERIAL_STREAM, SERIAL_COMMUNICATION_TIMEOUT, MODBUS_SLAVE_ID, MAX485_DE, MAX485_RE_NEG)
#else
    #define SOLAR_TRACER_INSTANCE EPEVERSolarTracer(BOARD_ST_SERIAL_STREAM, SERIAL_COMMUNICATION_TIMEOUT, MODBUS_SLAVE_ID)
#endif
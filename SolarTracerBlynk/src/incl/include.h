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

// INCL all core dependendecies
#include "src/incl/include_all_core.h"
// INCL all third party libs
#include "src/incl/include_all_lib.h"
// EXCEPTION: SoftwareSerial
#ifdef USE_SOFTWARE_SERIAL
  SoftwareSerial softSerial;
  #define BOARD_ST_SERIAL_STREAM softSerial
#endif

// INCL main components
#include "src/core/datetime.h"
#include "src/core/Controller.h"
// INCL optional features
#include "src/incl/include_all_feature.h"



#if defined(USE_SOFTWARE_SERIAL) & !defined(BOARD_ST_SERIAL_PIN_MAPPING_RX) 
#error "USE_SOFTWARE_SERIAL requires BOARD_ST_SERIAL_PIN_MAPPING_RX"
#endif

#if defined(USE_SOFTWARE_SERIAL) & !defined(BOARD_ST_SERIAL_PIN_MAPPING_TX) 
#error "USE_SOFTWARE_SERIAL requires BOARD_ST_SERIAL_PIN_MAPPING_TX"
#endif



// DIRECTIVE VALIDATION FOR ESP8266 ONLY
#ifdef ESP8266
#if defined(USE_HALL_AP_CONFIGURATION_TRIGGER)
#error "This board has no hall sensor to use! [ disable USE_HALL_AP_CONFIGURATION_TRIGGER ]"
#endif

#if !defined(USE_SOFTWARE_SERIAL) && ( defined(BOARD_ST_SERIAL_PIN_MAPPING_RX) | defined(BOARD_ST_SERIAL_PIN_MAPPING_TX) )
#error "This board does not support HW serial pin mapping! [ disable BOARD_ST_SERIAL_PIN_MAPPING_RX, BOARD_ST_SERIAL_PIN_MAPPING_TX ]"
#endif
#endif

// DIRECTIVE VALIDATION FOR SOLAR TRACER
#if !defined(USE_SERIAL_STREAM) && (SOLAR_TRACER_MODEL == EPEVER_SOLAR_TRACER_A | SOLAR_TRACER_MODEL == EPEVER_SOLAR_TRACER_B | SOLAR_TRACER_MODEL == EPEVER_SOLAR_TRACER_TRITON | SOLAR_TRACER_MODEL == EPEVER_SOLAR_TRACER_XTRA)
#error You must enable USE_SERIAL_STREAM !
#endif

#if defined(USE_SERIAL_STREAM)  &&  !defined(BOARD_ST_SERIAL_STREAM)
#error You must specify a serial in BOARD_ST_SERIAL_STREAM!
#endif
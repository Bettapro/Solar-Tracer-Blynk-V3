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

#include "../epever/epever_config.h"

#if (SOLAR_TRACER_MODEL == EPEVER_SOLAR_TRACER_A | SOLAR_TRACER_MODEL == EPEVER_SOLAR_TRACER_B | SOLAR_TRACER_MODEL == EPEVER_SOLAR_TRACER_TRITON | SOLAR_TRACER_MODEL == EPEVER_SOLAR_TRACER_XTRA)
#include "../epever/EPEVERSolarTracer.h"
#ifdef USE_SERIAL_MAX485
#define SOLAR_TRACER_INSTANCE EPEVERSolarTracer(BOARD_ST_SERIAL_STREAM, SERIAL_COMMUNICATION_TIMEOUT, MODBUS_SLAVE_ID, MAX485_DE, MAX485_RE_NEG)
#else
#define SOLAR_TRACER_INSTANCE EPEVERSolarTracer(BOARD_ST_SERIAL_STREAM, SERIAL_COMMUNICATION_TIMEOUT, MODBUS_SLAVE_ID)
#endif
#elif (SOLAR_TRACER_MODEL == DUMMY_SOLAR_TRACER)
#include "../dummy/DummySolarTracer.h"
#define SOLAR_TRACER_INSTANCE DummySolarTracer()
#endif
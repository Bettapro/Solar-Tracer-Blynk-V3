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

#ifndef project_core_config_h
#define project_core_config_h

#define PROJECT_NAME "SolarTracer"
#define PROJECT_VERSION "v3.0.4"
#define PROJECT_SUBVERSION 1


#include <Arduino.h>

/**
 *  Main defs
 */
#include "solartracer_all.h"
#include "communication_protocol_all.h"
#include "status_all.h"
#include "config_persistence.h"


/**
 * Include user + board + solar tracer configs
 */
#include "../../config.h"



/**
 * Conditional includes depending on the BOARD
 */
#if defined ESP32
#include "../board/esp32_config.h"
#elif defined ESP8266
#include "../board/esp8266_config.h"
#else
#error This board is not supported.
#endif


#endif
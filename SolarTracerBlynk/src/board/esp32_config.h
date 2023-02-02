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

#ifdef ESP32

#ifndef BOARD_DEBUG_SERIAL_STREAM
#define BOARD_DEBUG_SERIAL_STREAM Serial
#endif

#ifndef BOARD_DEBUG_SERIAL_STREAM_BAUDRATE
#define BOARD_DEBUG_SERIAL_STREAM_BAUDRATE 115200
#endif

#if defined USE_PIN_AP_CONFIGURATION_TRIGGER and ! defined(PIN_AP_TRIGGER_PIN)
    #define PIN_AP_TRIGGER_PIN 19
#endif

#if defined(USE_SERIAL_STREAM) && !defined(BOARD_ST_SERIAL_STREAM)
#define BOARD_ST_SERIAL_STREAM Serial2
#endif

#if defined(USE_SERIAL_STREAM)  && !defined(BOARD_ST_SERIAL_PRETRANSMIT_WAIT)
    #define BOARD_ST_SERIAL_PRETRANSMIT_WAIT 0
#endif

#if defined(USE_SERIAL_MAX485) && !defined(MAX485_DE)
#define MAX485_DE 13
#endif

#if defined(USE_SERIAL_MAX485) && !defined(MAX485_RE_NEG)
#define MAX485_RE_NEG 13
#endif

#if defined(USE_WIFI_AP_CONFIGURATION) && defined(USE_HALL_AP_CONFIGURATION_TRIGGER)
  #if !defined(HALL_AP_CONFIGURATION_BASE_VALUE)
    #define HALL_AP_CONFIGURATION_BASE_VALUE 75
  #endif
  #if !defined(HALL_AP_CONFIGURATION_THR_VALUE)
    #define HALL_AP_CONFIGURATION_THR_VALUE 30
  #endif
#endif

#define WIFI_STATION_MODE_DISCONNECTED WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED
#endif
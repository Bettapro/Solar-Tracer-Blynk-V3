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

#ifndef project_config_h
#define project_config_h


#if (SOLAR_TRACER_MODEL == EPEVER_SOLAR_TRACER_A | SOLAR_TRACER_MODEL == EPEVER_SOLAR_TRACER_B | SOLAR_TRACER_MODEL == EPEVER_SOLAR_TRACER_TRITON | SOLAR_TRACER_MODEL == EPEVER_SOLAR_TRACER_XTRA)
#include "../solartracer/epever/epever_config.h"
#else
#error This Solar Tracer is not supported.
#endif

#ifdef USE_STATUS_LED
#include "../feature/status_led.h"
#endif

#ifdef USE_OTA_UPDATE
#include "../feature/arduino_ota.h"
#endif

#if defined USE_WIFI_AP_CONFIGURATION
#include "../feature/wifi_manager.h"
#endif

#if defined USE_BLYNK
#include "../feature/BlynkSync.h"
#endif
#if defined(USE_MQTT) && !defined(USE_MQTT_HOME_ASSISTANT)
#include "../feature/mqtt_sync.h"
#endif
#if defined(USE_MQTT) && defined(USE_MQTT_HOME_ASSISTANT)
#include "../feature/mqtt_homeassistant_sync.h"
#endif

#endif
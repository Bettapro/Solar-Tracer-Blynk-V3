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

#ifndef BLYNK_SYNC_H
#define BLYNK_SYNC_H

#include "../incl/project_core_config.h"

#ifdef USE_BLYNK



#include "../incl/project_include.h"
#include "BaseSync.h"
#include "../core/datetime.h"
#include "../core/VariableDefiner.h"
#include "../core/Controller.h"
#include "../core/debug.h"

#define BLYNK_CONNECT_ATTEMPT 3
#define BLYNK_VALUE_CACHES_UNTIL_COUNT 5
#define BLYNK_VALUE_CACHES_STRING_LEN 20

#ifdef vPIN_INTERNAL_DEBUG_TERMINAL
void blynkDebugCallback(String message);
#endif

class BlynkSync : public BaseSync
{
public:
  static BlynkSync &getInstance()
  {
    static BlynkSync instance;
    return instance;
  }

  void setup();
  void connect();
  void loop();
  inline bool isVariableAllowed(const VariableDefinition *def);
  bool sendUpdateToVariable(Variable variable, const void *value);
  // upload values stats
  void uploadStatsToBlynk();
  // upload values realtime
  void uploadRealtimeToBlynk();

private:
  BlynkSync();

  void *blynkValuesCache[Variable::VARIABLES_COUNT];
  uint8_t blynkValuesCacheValid[Variable::VARIABLES_COUNT];
};

#ifdef vPIN_INTERNAL_DEBUG_TERMINAL
void blynkDebugCallback(String message)
{
  BlynkSync::getInstance().sendUpdateToVariable(Variable::INTERNAL_DEBUG, message.c_str());
}
#endif

  bool BlynkSync::isVariableAllowed(const VariableDefinition *def)
  {
    return def->blynkVPin != nullptr;
  }

#endif
#endif

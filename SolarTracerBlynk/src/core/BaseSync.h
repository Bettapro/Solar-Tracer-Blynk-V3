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

#ifndef BASE_SYNC_H
#define BASE_SYNC_H

#include "../core/VariableDefiner.h"

#define BASE_SYNC_RENEW_VALUE_COUNT 20;

class BaseSync
{
public:
    BaseSync();
    virtual void setup() = 0;
    virtual void connect() = 0;
    virtual void loop() = 0;
    /**
     * @brief Send the update to remote server
     * 
     * @param def variable definition
     * @param value value
     * @return true if the update has been sent with success, false otherwise
     */
    virtual bool sendUpdateToVariable(const VariableDefinition * def, const void *value) = 0;
    virtual bool isVariableAllowed(const VariableDefinition *def) = 0;
    void applyUpdateToVariable(Variable variable, const void *value, bool silent = true);

     bool syncVariable(const VariableDefinition * def, const void *value);

    uint8_t sendUpdateAllBySource(VariableSource allowedSource, bool silent = true);

protected:
    /**
     *  0   sync on change only
     *  1   sync always
     *  ..  use renew count
     */
    uint8_t renewValueCount = BASE_SYNC_RENEW_VALUE_COUNT;

private:
    void **lastValuesCache;

    uint8_t* renewValuesCount;
};
#endif
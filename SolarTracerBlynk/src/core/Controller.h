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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../incl/include_all_lib.h"
#include "../solartracer/SolarTracer.h"

class Controller {
    public:
        static Controller &getInstance() {
            static Controller instance;
            return instance;
        }

        void setup(SolarTracer *tracer, SimpleTimer *timer) {
            this->mainTimer = timer;
            this->thisController = tracer;
        }

        void loop() {
            this->mainTimer->run();
        }

        bool getErrorFlag(uint32_t status);

        void setErrorFlag(uint32_t status, bool error);

        inline uint32_t getStatus();

        inline SolarTracer *getSolarController();

        inline SimpleTimer *getMainTimer();

    private:
        SimpleTimer *mainTimer;
        SolarTracer *thisController;
        uint32_t internalStatus = 0;
};

uint32_t Controller::getStatus() {
    return this->internalStatus;
}

SolarTracer *Controller::getSolarController() {
    return this->thisController;
}

SimpleTimer *Controller::getMainTimer() {
    return this->mainTimer;
}

#endif
 // specify the serial port to use 
 #pragma once

#if !defined(USE_SERIAL_STREAM)
#error You must enable USE_SERIAL_STREAM !
#endif

#if !defined(CONTROLLER_SERIAL)
#error You must specify a serial in CONTROLLER_SERIAL!
#endif


#ifndef CONTROLLER_UPDATE_MS_PERIOD
    #define CONTROLLER_SERIAL_BAUDRATE 115200 
#endif

#ifndef MODBUS_SLAVE_ID
    #define MODBUS_SLAVE_ID 1 
#endif

#ifndef CONTROLLER_UPDATE_MS_PERIOD
    #define CONTROLLER_UPDATE_MS_PERIOD 2000L 
#endif



#include "solartracer/epever/EPEVERSolarTracer.h"
#ifdef USE_SERIAL_MAX485
    #define SOLAR_TRACER_INSTANCE EPEVERSolarTracer(CONTROLLER_SERIAL, MODBUS_SLAVE_ID, MAX485_DE, MAX485_RE_NEG)
#else
    #define SOLAR_TRACER_INSTANCE EPEVERSolarTracer(CONTROLLER_SERIAL, MODBUS_SLAVE_ID)
#endif
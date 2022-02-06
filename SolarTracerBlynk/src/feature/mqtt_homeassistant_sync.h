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
#include "../incl/project_config.h"

#if ! defined(MQTT_HOMEASSISTANT_SYNC_H) && defined(USE_MQTT_HOME_ASSISTANT)
#define MQTT_HOMEASSISTANT_SYNC_H

WiFiClient wifiClient;

HADevice *device;
HAMqtt *mqtt;

#define RETAIN_ALL_MSG false
#define MQTT_CONNECT_ATTEMPT 3

void mqttSetup();
void mqttConnect();
void mqttLoop();

char mqttPublishBuffer[20];

HASensor *haSensors[Variable::VARIABLES_COUNT];

void uploadRealtimeToMqtt();
void uploadStatsToMqtt();

void executeFromMqttFloatWrite(Variable variable, float *value)
{
    if (!thisController->writeValue(variable, value))
    {
        debugPrintf(" - FAILED! [err=%i]", thisController->getLastControllerCommunicationStatus());
    }
    thisController->fetchValue(variable);
    uploadRealtimeToMqtt();
}

void executeFromMqttBoolWrite(Variable variable, bool *value)
{
    if (!thisController->writeValue(variable, value))
    {
        debugPrintf(" - FAILED! [err=%i]", thisController->getLastControllerCommunicationStatus());
    }
    thisController->fetchValue(variable);
    uploadRealtimeToMqtt();
}

bool initialized = false;

boolean mqttAttemptConnection()
{
    if (!initialized)
    {
#ifdef MQTT_PASSWORD
        initialized = mqtt->begin(MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD);
#elif defined MQTT_USERNAME
        initialized = mqtt->begin(MQTT_SERVER, MQTT_PORT, MQTT_USERNAME);
#else
        initialized = mqtt->begin(MQTT_SERVER, MQTT_PORT);
#endif
    }
    mqtt->loop();
    return mqtt->isConnected();
}

void mqttSetup()
{
    device = new HADevice(MQTT_HOME_ASSISTANT_DEVICE_ID);
    mqtt = new HAMqtt(wifiClient, *device);

    // set device's details
    device->setName(MQTT_HOME_ASSISTANT_DEVICE_NAME);
    device->setSoftwareVersion(PROJECT_VERSION);

    //mqttClient.setCallback(mqttCallback);

    for (uint8_t index = 0; index < Variable::VARIABLES_COUNT; index++)
    {

        const VariableDefinition *def = VariableDefiner::getInstance().getDefinition((Variable)index);

        haSensors[index] = new HASensor(def->mqttTopic);
        haSensors[index]->setName(def->text);

        switch (def->uom)
        {
        case UOM_TEMPERATURE_C:
            haSensors[index]->setDeviceClass("temperature");
            haSensors[index]->setUnitOfMeasurement("°C");
            break;
        case UOM_WATT:
            haSensors[index]->setDeviceClass("power");
            haSensors[index]->setUnitOfMeasurement("W");
            break;
        case UOM_KILOWATTHOUR:
            haSensors[index]->setDeviceClass("energy");
            haSensors[index]->setUnitOfMeasurement("kWh");
            break;
        case UOM_PERCENT:
            haSensors[index]->setUnitOfMeasurement("%");
            break;
        case UOM_AMPERE:
            haSensors[index]->setDeviceClass("current");
            haSensors[index]->setUnitOfMeasurement("A");
            break;
        case UOM_VOLT:
            haSensors[index]->setDeviceClass("voltage");
            haSensors[index]->setUnitOfMeasurement("V");
            break;
        }
    }

    mqttConnect();
}

void mqttConnect()
{
    debugPrintln(" ++ Setting up MQTT:");
    debugPrint("Connecting...");

    uint8_t counter = 0;

    while (!mqttAttemptConnection())
    {
        debugPrint(".");
        delay(500);
        counter++;
    }
}

void mqttLoop()
{
    if (!mqtt->isConnected())
    {
        setStatusError(STATUS_ERR_NO_MQTT_CONNECTION);
    }
    else
    {
        clearStatusError(STATUS_ERR_NO_MQTT_CONNECTION);
    }
    mqtt->loop();
}

// upload values stats
uint8_t uploadDataToMqtt(bool uploadRealtime, bool uploadStat, bool uploadInternal)
{

    uint8_t varNotReady = 0;
    for (uint8_t index = 0; index < Variable::VARIABLES_COUNT; index++)
    {
        const VariableDefinition *def = VariableDefiner::getInstance().getDefinition((Variable)index);
        if (def->mqttTopic != nullptr && ((uploadRealtime && def->source == VariableSource::SR_REALTIME) ||
                                          (uploadStat && def->source == VariableSource::SR_INTERNAL) ||
                                          (uploadInternal && def->source == VariableSource::SR_STATS)))

            if (thisController->isVariableReadReady(def->variable))
            {
                HASensor *sensor = haSensors[index];
                switch (def->datatype)
                {
                case VariableDatatype::DT_FLOAT:
                    sensor->setValue(thisController->getFloatValue(def->variable));
                    break;
                case VariableDatatype::DT_STRING:
                    sensor->setValue(thisController->getStringValue(def->variable));
                    break;
                default:
                    break;
                }
            }
        varNotReady++;
    }

    return varNotReady;
}

// upload values stats
void uploadStatsToMqtt()
{
    uint8_t uploadFailedCount = uploadDataToMqtt(false, true, true);
    if (uploadFailedCount > 0)
    {
        debugPrintln("Some ST variables are not ready and not synced!");
        setStatusError(STATUS_ERR_SOLAR_TRACER_NO_SYNC_ST);
    }
    else
    {
        clearStatusError(STATUS_ERR_SOLAR_TRACER_NO_SYNC_ST);
    }
}

// upload values realtime
void uploadRealtimeToMqtt()
{
    uint8_t uploadFailedCount = uploadDataToMqtt(true, true, false);
    if (uploadFailedCount > 0)
    {
        debugPrintln("Some ST variables are not ready and not synced!");
        setStatusError(STATUS_ERR_SOLAR_TRACER_NO_SYNC_ST);
    }
    else
    {
        clearStatusError(STATUS_ERR_SOLAR_TRACER_NO_SYNC_ST);
    }
}


#endif
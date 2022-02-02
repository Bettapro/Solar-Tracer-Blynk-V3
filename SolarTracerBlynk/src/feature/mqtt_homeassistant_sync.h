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

WiFiClient wifiClient;

HADevice *device;
HAMqtt *mqtt;

#define RETAIN_ALL_MSG false
#define MQTT_CONNECT_ATTEMPT 3

void mqttSetup();
void mqttConnect();
void mqttLoop();

char mqttPublishBuffer[20];

HASensor *haSensors[statVirtualMqttSolarVariablesCount + realTimeVirtualMqttSolarVariablesCount];

void uploadRealtimeToMqtt();
void uploadStatsToMqtt();

void executeFromMqttFloatWrite(SolarTracerVariables variable, float *value)
{
    if (!thisController->writeValue(variable, value))
    {
        debugPrintf(" - FAILED! [err=%i]", thisController->getLastControllerCommunicationStatus());
    }
    thisController->fetchValue(variable);
    uploadRealtimeToMqtt();
}

void executeFromMqttBoolWrite(SolarTracerVariables variable, bool *value)
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

    for (uint8_t index = 0; index < statVirtualMqttSolarVariablesCount + realTimeVirtualMqttSolarVariablesCount; index++)
    {

        const mqttSolarVariableMap solarVariable = (index >= realTimeVirtualMqttSolarVariablesCount)
                                                       ? statVirtualMqttSolarVariables[index - realTimeVirtualMqttSolarVariablesCount]
                                                       : realTimeVirtualMqttSolarVariables[index];

        haSensors[index] = new HASensor(solarVariable.topic);
        haSensors[index]->setName(solarVariable.topic);

        switch (thisController->getVariableUOM(solarVariable.solarVariable))
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

bool uploadVariableToMqtt(boolean isRT, uint8_t index, const mqttSolarVariableMap *varDef)
{
    HASensor *sensor = haSensors[index + (isRT ? 0 : realTimeVirtualMqttSolarVariablesCount)];

    if (thisController->isVariableReadReady(varDef->solarVariable))
    {

        switch (thisController->getVariableDatatype(varDef->solarVariable))
        {
        case SolarTracerVariablesDataType::DT_FLOAT:
            sensor->setValue(thisController->getFloatValue(varDef->solarVariable));
            break;
        case SolarTracerVariablesDataType::DT_STRING:
            sensor->setValue(thisController->getStringValue(varDef->solarVariable));
            break;
        default:
            break;
        }
        return true;
    }
    return false;
}

// upload values stats
void uploadStatsToMqtt()
{

    bool varNotReady = false;
    for (uint8_t index = 0; index < statVirtualMqttSolarVariablesCount; index++)
    {
        varNotReady |= !uploadVariableToMqtt(false, index, &statVirtualMqttSolarVariables[index]);
    }
    if (varNotReady)
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
    bool varNotReady = false;
    for (uint8_t index = 0; index < realTimeVirtualMqttSolarVariablesCount; index++)
    {
        varNotReady |= !uploadVariableToMqtt(true, index, &realTimeVirtualMqttSolarVariables[index]);
    }
    if (varNotReady)
    {
        debugPrintln("Some RT variables are not ready and not synced!");
        setStatusError(STATUS_ERR_SOLAR_TRACER_NO_SYNC_RT);
    }
    else
    {
        clearStatusError(STATUS_ERR_SOLAR_TRACER_NO_SYNC_RT);
    }
}

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

#include "MqttHASync.h"

#include "../incl/include_all_core.h"

#ifdef USE_MQTT_HOME_ASSISTANT

char mqttPublishBuffer[20];

#define RETAIN_ALL_MSG false
#define MQTT_CONNECT_ATTEMPT 3

MqttHASync::MqttHASync()
{
    WiFiClient *wifiClient = new WiFiClient;

    device = new HADevice(MQTT_HOME_ASSISTANT_DEVICE_ID);
    mqtt = new HAMqtt(*wifiClient, *device);
}

void MqttHASync::setup()
{

    this->initialized = false;

    // set device's details
    device->setName(MQTT_HOME_ASSISTANT_DEVICE_NAME);
    device->setSoftwareVersion(PROJECT_VERSION);

    // mqttClient.setCallback(mqttCallback);

    for (uint8_t index = 0; index < Variable::VARIABLES_COUNT; index++)
    {

        const VariableDefinition *def = VariableDefiner::getInstance().getDefinition((Variable)index);
        if (def->mqttTopic != nullptr)
        {
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
    }
    this->connect();
}

bool MqttHASync::attemptMqttHASyncConnect()
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

void MqttHASync::connect()
{
    debugPrintf(true, Text::setupWithName, "MQTT-HA");
    debugPrint(Text::connecting);

    uint8_t counter = 0;

    while (!attemptMqttHASyncConnect())
    {
        debugPrint(Text::dot);
        delay(500);
        counter++;
    }
}
void MqttHASync::loop()
{
    Controller::getInstance().setErrorFlag(STATUS_ERR_NO_MQTT_CONNECTION, !mqtt->isConnected());
    mqtt->loop();
}
bool MqttHASync::isVariableAllowed(const VariableDefinition *def)
{
    return def->mqttTopic != nullptr;
}
bool MqttHASync::sendUpdateToVariable(Variable variable, const void *value)
{

    HASensor *sensor = haSensors[variable];
    switch (VariableDefiner::getInstance().getDatatype(variable))
    {
    case VariableDatatype::DT_FLOAT:
        return sensor->setValue(*(const float *)value);
    case VariableDatatype::DT_BOOL:
        return sensor->setValue(*(const bool *)value);
    case VariableDatatype::DT_STRING:
        return sensor->setValue((const char *)value);
    }
    return false;
}

// upload values stats
void MqttHASync::uploadStatsToMqtt()
{
    if (!this->mqtt->isConnected())
    {
        return;
    }

    MqttHASync::getInstance().sendUpdateAllBySource(VariableSource::SR_STATS, false);
}

// upload values realtime
void MqttHASync::uploadRealtimeToMqtt()
{
    if (!this->mqtt->isConnected())
    {
        return;
    }

#ifdef MQTT_TOPIC_INTERNAL_STATUS
    float status = Controller::getInstance().getStatus();
    MqttHASync::getInstance().sendUpdateToVariable(Variable::INTERNAL_STATUS, &(status));
#endif
    MqttHASync::getInstance().sendUpdateAllBySource(VariableSource::SR_REALTIME, false);
}

#endif

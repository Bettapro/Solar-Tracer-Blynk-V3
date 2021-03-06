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
#include "../core/datetime.h"

#ifdef USE_MQTT_HOME_ASSISTANT

char mqttPublishBuffer[20];

#define RETAIN_ALL_MSG false
#define MQTT_CONNECT_ATTEMPT 3

// every setValue/setState will trigger mqttCallbacks,
bool ignoreCallback = false;

inline const char *getVariableUOM(VariableUOM uom)
{
    switch (uom)
    {
    case UOM_TEMPERATURE_C:
        return "°C";
    case UOM_WATT:
        return "W";
    case UOM_KILOWATTHOUR:
        return "kWh";
    case UOM_PERCENT:
        return "%";
    case UOM_AMPERE:
        return "A";
    case UOM_VOLT:
        return "V";
    case UOM_MINUTE:
        return "min";
    }
    return nullptr;
}

void onMqttNumberCallback(float value, HANumber *el)
{
    if (!ignoreCallback)
    {
        MqttHASync haSync = MqttHASync::getInstance();
        Variable var = haSync.findVariableBySensor(el);
        if (var < Variable::VARIABLES_COUNT)
        {

            switch (VariableDefiner::getInstance().getDatatype(var))
            {
            case DT_UINT16:
            {
                uint16_t rValue = round(value);
                haSync.applyUpdateToVariable(var, &rValue, false);
            }
            break;
            default:
                haSync.applyUpdateToVariable(var, &value, false);
            }
        }
    }
}

void onMqttBoolCallback(bool value, HASwitch *el)
{
    if (!ignoreCallback)
    {
        MqttHASync haSync = MqttHASync::getInstance();
        Variable var = haSync.findVariableBySensor(el);
        if (var < Variable::VARIABLES_COUNT)
        {

            switch (var)
            {
            case Variable::REALTIME_CLOCK:
                if (value)
                {
                    debugPrintln("UPDATE CONTROLLER DATETIME");
                    if (Datetime::getMyNowTm() != nullptr)
                    {
                        Controller::getInstance().getSolarController()->syncRealtimeClock(Datetime::getMyNowTm());
                        el->setState(false);
                    }
                }
                break;
            case Variable::UPDATE_ALL_CONTROLLER_DATA:
                if (value)
                {
                    debugPrintln("REQUEST ALL VALUES TO CONTROLLER");
                    Controller::getInstance().getSolarController()->fetchAllValues();
                    MqttHASync::getInstance().uploadRealtimeToMqtt();
                    MqttHASync::getInstance().uploadStatsToMqtt();
                    el->setState(false);
                }
                break;
            default:
                haSync.applyUpdateToVariable(var, &value, false);
            }
        }
    }
}

MqttHASync::MqttHASync() : BaseSync()
{
    this->renewValueCount = 0;

    WiFiClient *wifiClient = new WiFiClient;

    device = new HADevice(Environment::getData()->mqttClientId);
    mqtt = new HAMqtt(*wifiClient, *device);
}

void MqttHASync::setup()
{

    this->initialized = false;

    // set device's details

    device->setName(Environment::getData()->mqttHADeviceName);
    device->setManufacturer(PROJECT_AUTHOR);
    device->setModel(PROJECT_NAME);
    device->setSoftwareVersion(PROJECT_VERSION);

    for (uint8_t index = 0; index < Variable::VARIABLES_COUNT; index++)
    {

        const VariableDefinition *def = VariableDefiner::getInstance().getDefinition((Variable)index);

        if (def->mqttTopic != nullptr)
        {
            haSensors[index] = nullptr;
            switch (def->datatype)
            {
            case DT_BOOL:
                if (def->mode == MD_READWRITE)
                {
                    HASwitch *hASwitch = new HASwitch(def->mqttTopic, false);
                    hASwitch->onStateChanged(onMqttBoolCallback);
                    haSensors[index] = hASwitch;
                }
                else
                {
                    haSensors[index] = new HABinarySensor(def->mqttTopic, false);
                }
                break;
            default:
                if (def->mode == MD_READWRITE)
                {
                    HANumber *haNumber = new HANumber(def->mqttTopic);
                    haNumber->onValueChanged(onMqttNumberCallback);
                    haNumber->setStep(def->datatype == DT_FLOAT ? 0.01 : 1);
                    haNumber->setPrecision(def->datatype == DT_FLOAT ? 2 : 0);
                    haNumber->setUnitOfMeasurement(getVariableUOM(def->uom));
                    haSensors[index] = haNumber;
                }
                else
                {
                    HASensor *hASensor = new HASensor(def->mqttTopic);
                    switch (def->uom)
                    {
                    case UOM_TEMPERATURE_C:
                        hASensor->setDeviceClass("temperature");
                        break;
                    case UOM_WATT:
                        hASensor->setDeviceClass("power");
                        break;
                    case UOM_KILOWATTHOUR:
                        hASensor->setDeviceClass("energy");
                        break;
                    case UOM_PERCENT:
                        hASensor->setUnitOfMeasurement("%");
                        break;
                    case UOM_AMPERE:
                        hASensor->setDeviceClass("current");
                        break;
                    case UOM_VOLT:
                        hASensor->setDeviceClass("voltage");
                        break;
                    }
                    hASensor->setUnitOfMeasurement(getVariableUOM(def->uom));
                    haSensors[index] = hASensor;
                }
            }

            haSensors[index]->setName(def->text);
        }
    }
    this->connect();
}

Variable MqttHASync::findVariableBySensor(BaseDeviceType *haSensor)
{
    for (uint8_t index = 0; index < Variable::VARIABLES_COUNT; index++)
    {
        if (haSensors[index] == haSensor)
        {
            return (Variable)index;
        }
    }
    return Variable::VARIABLES_COUNT;
}

bool MqttHASync::attemptMqttHASyncConnect()
{
    if (!initialized)
    {
        environrmentData *envData = Environment::getData();
        initialized = mqtt->begin(
            envData->mqttServerHostname,
            envData->mqttServerPort,
            strlen(envData->mqttUsername) > 0 ? envData->mqttUsername : nullptr,
            strlen(envData->mqttPassword) > 0 ? envData->mqttPassword : nullptr);
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
    debugPrintln(Text::ok);
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
bool MqttHASync::sendUpdateToVariable(const VariableDefinition *def, const void *value)
{

    BaseDeviceType *sensor = haSensors[def->variable];
    bool result = false;
    ignoreCallback = true;
    switch (def->datatype)
    {
    case VariableDatatype::DT_UINT16:
        result = def->mode == MD_READ ? ((HASensor *)sensor)->setValue(*(uint16_t *)value) : ((HANumber *)sensor)->setValue(*(uint16_t *)value);
        break;
    case VariableDatatype::DT_FLOAT:
        result = def->mode == MD_READ ? ((HASensor *)sensor)->setValue(*(float *)value) : ((HANumber *)sensor)->setValue(*(float *)value);
        break;
    case VariableDatatype::DT_BOOL:
        result = def->mode == MD_READ ? ((HABinarySensor *)sensor)->setState(*(bool *)value) : ((HASwitch *)sensor)->setState(*(bool *)value);
        break;
    case VariableDatatype::DT_STRING:
        result = ((HASensor *)sensor)->setValue((const char *)value);
        break;
    }
    ignoreCallback = false;
    return result;
}

// upload values stats
void MqttHASync::uploadStatsToMqtt()
{
    if (!this->mqtt->isConnected())
    {
        return;
    }

    this->sendUpdateAllBySource(VariableSource::SR_STATS, false);
}

// upload values realtime
void MqttHASync::uploadRealtimeToMqtt()
{
    if (!this->mqtt->isConnected())
    {
        return;
    }

#ifdef MQTT_TOPIC_INTERNAL_STATUS
    uint16_t status = Controller::getInstance().getStatus();
    this->syncVariable(VariableDefiner::getInstance().getDefinition(Variable::INTERNAL_STATUS), &(status));
#endif
    this->sendUpdateAllBySource(VariableSource::SR_REALTIME, false);
}

#endif

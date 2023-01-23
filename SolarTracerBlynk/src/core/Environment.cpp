#include "Environment.h"

DynamicJsonDocument* Environment::envData = new DynamicJsonDocument(1024);

void Environment::loadEnvData()
{
    // default from config.h
    (*envData)[CONFIG_WIFI_SSID] = WIFI_SSID;
    (*envData)[CONFIG_WIFI_SSID] = WIFI_PASS;
    (*envData)[CONFIG_WIFI_IP_ADDRESS] = WIFI_IP_ADDRESS;
    (*envData)[CONFIG_WIFI_GATEWAY] = WIFI_GATEWAY;
    (*envData)[CONFIG_WIFI_SUBNET] = WIFI_SUBNET;
    (*envData)[CONFIG_WIFI_DNS1] = WIFI_DNS1;
    (*envData)[CONFIG_WIFI_DNS2] = WIFI_DNS2;

#ifdef USE_WIFI_AP_CONFIGURATION
    (*envData)[CONFIG_WM_AP_SSID] = WIFI_AP_CONFIGURATION_HOSTNAME;
    (*envData)[CONFIG_WM_AP_PASSWORD] = WIFI_AP_CONFIGURATION_PASSWORD;
#endif

#ifdef USE_BLYNK
(*envData)[CONFIG_BLYNK_AUTH] = BLYNK_AUTH;
#ifdef USE_BLYNK_2
#else
#ifdef USE_BLYNK_LOCAL_SERVER
    (*envData)[CONFIG_BLYNK_HOSTNAME] = BLYNK_SERVER;
    (*envData)[CONFIG_BLYNK_PORT] = BLYNK_PORT;
#else
    (*envData)[CONFIG_BLYNK_HOSTNAME] = "";
    (*envData)[CONFIG_BLYNK_PORT] = 0;
#endif
#endif
#endif

#ifdef USE_MQTT
    (*envData)[CONFIG_MQTT_HOSTNAME] = MQTT_SERVER;
    (*envData)[CONFIG_MQTT_USERNAME] = MQTT_USERNAME;
    (*envData)[CONFIG_MQTT_PASSWORD] = MQTT_PASSWORD;
    (*envData)[CONFIG_MQTT_CLIENT_ID] = MQTT_CLIENT_ID;
    (*envData)[CONFIG_MQTT_PORT] = MQTT_PORT;
#endif
#ifdef USE_MQTT_HOME_ASSISTANT
    (*envData)[CONFIG_MQTT_HA_DEVICE_NAME] = MQTT_HOME_ASSISTANT_DEVICE_NAME;
#endif

#ifdef USE_OTA_UPDATE
    (*envData)[CONFIG_OTA_HOSTNAME] = OTA_HOSTNAME;
    (*envData)[CONFIG_OTA_PASSWORD] = OTA_PASS;
#endif

#ifdef USE_NTP_SERVER
    (*envData)[CONFIG_NTP_SERVER] = NTP_SERVER_CONNECT_TO;
    (*envData)[CONFIG_NTP_TIMEZONE] = CURRENT_TIMEZONE;
#endif

#ifdef USE_EXTERNAL_HEAVY_LOAD_CURRENT_METER
    (*envData)[CONFIG_EXTERNAL_HEAVY_LOAD_CURRENT_METER_VOLTAGE_ZERO_AMP_VOLT] = EXTERNAL_HEAVY_LOAD_CURRENT_METER_VOLTAGE_ZERO_AMP_VOLT;
#endif


    // END OF LOAD FROM config.h

#if defined USE_WIFI_AP_CONFIGURATION
    if (!LittleFS.begin())
    {
        debugPrintf(true, Text::errorWithCode, ERROR_LITTLEFS_BEGIN_FAILED);
        if (!LittleFS.begin(true))
        {
            debugPrintf(true, Text::errorWithCode, ERROR_LITTLEFS_FORMAT_FAILED);
        }
        return;
    }
    // load from file
    if (!LittleFS.exists(CONFIG_PERSISTENCE))
    {
        // no file found
        debugPrintln("No configuration file found");
        return;
    }

    debugPrintln("Restoring configuration from file");
    // file exists, reading and loading
    File configFile = LittleFS.open(CONFIG_PERSISTENCE, "r");
    if (!configFile)
    {
        debugPrintln("ERROR: cannot open config file");
        return;
    }

    DeserializationError error = deserializeJson(*envData, configFile);
    if (error)
    {
        debugPrintln("ERROR: Cannot deserialize settings from file");
        debugPrintln(error.c_str());
    }
   

    LittleFS.end();
#endif
}

bool Environment::containsStringNotEmpty(const char* key){
    return envData->containsKey(key) && strlen((*envData)[key]) > 0;
}

void Environment::resetEnvData()
{
    LittleFS.begin();
    // load from file
    if (LittleFS.exists(CONFIG_PERSISTENCE))
    {
        LittleFS.remove(CONFIG_PERSISTENCE);
    }
    LittleFS.end();
}

#include "Environment.h"



struct environrmentData Environment::envData;

void Environment::loadEnvData()
{
    // default from config.h
    strcpy(envData.wifiSSID, WIFI_SSID);
    strcpy(envData.wifiPassword, WIFI_PASS);

#ifdef USE_WIFI_AP_CONFIGURATION
    strcpy(envData.wmApSSID, WIFI_AP_CONFIGURATION_HOSTNAME);
    strcpy(envData.wmApPassword, WIFI_AP_CONFIGURATION_PASSWORD);
#endif

#ifdef USE_BLYNK
#ifndef USE_BLYNK_2
#ifdef USE_BLYNK_LOCAL_SERVER
    envData.blynkLocalServer = true;
    strcpy(envData.blynkServerHostname, BLYNK_SERVER);
    envData.blynkServerPort = BLYNK_PORT;
#else
    envData.blynkLocalServer = false;
    envData.blynkServerPort = 0;
#endif
#endif
#if defined USE_BLYNK
    strcpy(envData.blynkAuth, BLYNK_AUTH);
#endif
#endif

#ifdef USE_OTA_UPDATE
    strcpy(envData.otaHostname, OTA_HOSTNAME);
    strcpy(envData.otaPassword, OTA_PASS);
#endif

#ifdef USE_NTP_SERVER
    strcpy(envData.ntpServer, NTP_SERVER_CONNECT_TO);
    strcpy(envData.ntpTimezone, CURRENT_TIMEZONE);
#endif

    // END OF LOAD FROM config.h

#if defined USE_WIFI_AP_CONFIGURATION
    LittleFS.begin();
    // load from file
    if (!LittleFS.exists(CONFIG_PERSISTENCE))
    {
        // no file found
        debugPrintln("No configuration file found");
    }
    else
    {
        debugPrintln("Restoring configuration from file");
        // file exists, reading and loading
        File configFile = LittleFS.open(CONFIG_PERSISTENCE, "r");
        if (!configFile)
        {
            debugPrintln("ERROR: cannot open config file");
        }
        else
        {
            size_t size = configFile.size();
            DynamicJsonDocument doc(size * 3);
            DeserializationError error = deserializeJson(doc, configFile);
            if (error)
            {
                debugPrintln("ERROR: Cannot deserialize settings from file");
                debugPrintln(error.c_str());
            }
            else
            {
                loadStringToEnvIfExist(doc, CONFIG_PERSISTENCE_WIFI_SSID, envData.wifiSSID);
                loadStringToEnvIfExist(doc, CONFIG_PERSISTENCE_WIFI_PASSWORD, envData.wifiPassword);

                loadStringToEnvIfExist(doc, CONFIG_PERSISTENCE_WM_AP_SSID, envData.wmApSSID);
                loadStringToEnvIfExist(doc, CONFIG_PERSISTENCE_WM_AP_PASSWORD, envData.wmApPassword);

#if defined USE_BLYNK
                loadStringToEnvIfExist(doc, CONFIG_PERSISTENCE_BLYNK_AUTH, envData.blynkAuth);
#ifndef USE_BLYNK_2
                if (doc.containsKey(CONFIG_PERSISTENCE_BLYNK_IS_LOCAL))
                    envData.blynkLocalServer = doc[CONFIG_PERSISTENCE_BLYNK_IS_LOCAL];

                loadStringToEnvIfExist(doc, CONFIG_PERSISTENCE_BLYNK_HOSTNAME, envData.blynkServerHostname);

                if (doc.containsKey(CONFIG_PERSISTENCE_BLYNK_PORT))
                    envData.blynkServerPort = doc[CONFIG_PERSISTENCE_BLYNK_PORT];
#endif
#endif

#ifdef USE_OTA_UPDATE
                loadStringToEnvIfExist(doc, CONFIG_PERSISTENCE_OTA_HOSTNAME, envData.otaHostname);
                loadStringToEnvIfExist(doc, CONFIG_PERSISTENCE_OTA_PASSWORD, envData.otaPassword);
#endif

#ifdef USE_NTP_SERVER
                loadStringToEnvIfExist(doc, CONFIG_PERSISTENCE_NTP_SERVER, envData.ntpServer);
                loadStringToEnvIfExist(doc, CONFIG_PERSISTENCE_NTP_TIMEZONE, envData.ntpTimezone);
#endif
            }
        }
    }
    LittleFS.end();
#endif
}


 #ifdef USE_WIFI_AP_CONFIGURATION
void Environment::loadStringToEnvIfExist(DynamicJsonDocument doc, const char *envKey, char *envValue)
{
    if (doc.containsKey(envKey))
        strcpy(envValue, doc[envKey]);
}
#endif

environrmentData* Environment::getData()
{
    return &envData;
}



void Environment::resetEnvData()
{
    LittleFS.begin();
    // load from file
    if (!LittleFS.exists(CONFIG_PERSISTENCE))
    {
        // no file found
        debugPrintln("No configuration file found");
    }
    else
    {
        LittleFS.remove(CONFIG_PERSISTENCE);
    }
    LittleFS.end();
}


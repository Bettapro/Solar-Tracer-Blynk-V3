#include "Environment.h"

struct EnvironrmentData Environment::envData;

void Environment::loadEnvData() {
    envData.serialDebug = true;
    // default from config.h
    strcpy(envData.wifiSSID, WIFI_SSID);
    strcpy(envData.wifiPassword, WIFI_PASS);

    strcpy(envData.wifiIp, WIFI_IP_ADDRESS);
    strcpy(envData.wifiGateway, WIFI_GATEWAY);
    strcpy(envData.wifiSubnet, WIFI_SUBNET);
    strcpy(envData.wifiDns1, WIFI_DNS1);
    strcpy(envData.wifiDns2, WIFI_DNS2);

#ifdef USE_WIFI_AP_CONFIGURATION
    strcpy(envData.wmApSSID, WIFI_AP_CONFIGURATION_HOSTNAME);
    strcpy(envData.wmApPassword, WIFI_AP_CONFIGURATION_PASSWORD);
#endif

#ifdef USE_BLYNK
#ifndef USE_BLYNK_2
#ifdef USE_BLYNK_LOCAL_SERVER
    strcpy(envData.blynkServerHostname, BLYNK_SERVER);
    envData.blynkServerPort = BLYNK_PORT;
#else
    strcpy(envData.blynkServerHostname, "");
    envData.blynkServerPort = 0;
#endif
#endif
#if defined USE_BLYNK
    strcpy(envData.blynkAuth, BLYNK_AUTH);
#endif
#endif
#ifdef USE_MQTT
    strcpy(envData.mqttServerHostname, MQTT_SERVER);
    strcpy(envData.mqttUsername, MQTT_USERNAME);
    strcpy(envData.mqttPassword, MQTT_PASSWORD);
    strcpy(envData.mqttClientId, MQTT_CLIENT_ID);
    envData.mqttServerPort = MQTT_PORT;
#endif
#ifdef USE_MQTT_HOME_ASSISTANT
    strcpy(envData.mqttHADeviceId, MQTT_HOME_ASSISTANT_DEVICE_ID);
    strcpy(envData.mqttHADeviceName, MQTT_HOME_ASSISTANT_DEVICE_NAME);
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
    if (!LittleFS.begin()) {
        LittleFS.format();
    }
    // load from file
    if (!LittleFS.exists(CONFIG_PERSISTENCE)) {
        // no file found
        debugPrintln("No configuration file found");
    } else {
        debugPrintln("Restoring configuration from file");
        // file exists, reading and loading
        File configFile = LittleFS.open(CONFIG_PERSISTENCE, "r");
        if (!configFile) {
            debugPrintln("ERROR: cannot open config file");
        } else {
            size_t size = configFile.size();
            DynamicJsonDocument doc(size * 3);
            DeserializationError error = deserializeJson(doc, configFile);
            if (error) {
                debugPrintln("ERROR: Cannot deserialize settings from file");
                debugPrintln(error.c_str());
            } else {
                if (doc.containsKey(CONFIG_SERIAL_DEBUG)) {
                    envData.serialDebug = doc[CONFIG_SERIAL_DEBUG];
                }

                loadStringToEnvIfExist(doc, CONFIG_WIFI_SSID, envData.wifiSSID);
                loadStringToEnvIfExist(doc, CONFIG_WIFI_PASSWORD, envData.wifiPassword);

                loadStringToEnvIfExist(doc, CONFIG_WIFI_IP_ADDRESS, envData.wifiIp);
                loadStringToEnvIfExist(doc, CONFIG_WIFI_GATEWAY, envData.wifiGateway);
                loadStringToEnvIfExist(doc, CONFIG_WIFI_SUBNET, envData.wifiSubnet);
                loadStringToEnvIfExist(doc, CONFIG_WIFI_DNS1, envData.wifiDns1);
                loadStringToEnvIfExist(doc, CONFIG_WIFI_DNS2, envData.wifiDns2);

                loadStringToEnvIfExist(doc, CONFIG_WM_AP_SSID, envData.wmApSSID);
                loadStringToEnvIfExist(doc, CONFIG_WM_AP_PASSWORD, envData.wmApPassword);

#if defined USE_BLYNK
                loadStringToEnvIfExist(doc, CONFIG_BLYNK_AUTH, envData.blynkAuth);
#ifndef USE_BLYNK_2
                loadStringToEnvIfExist(doc, CONFIG_BLYNK_HOSTNAME, envData.blynkServerHostname);

                if (doc.containsKey(CONFIG_BLYNK_PORT)) {
                    envData.blynkServerPort = doc[CONFIG_BLYNK_PORT];
                }
#endif
#endif
#ifdef USE_MQTT
                loadStringToEnvIfExist(doc, CONFIG_MQTT_HOSTNAME, envData.mqttServerHostname);
                loadStringToEnvIfExist(doc, CONFIG_MQTT_PASSWORD, envData.mqttPassword);
                loadStringToEnvIfExist(doc, CONFIG_MQTT_USERNAME, envData.mqttUsername);
                loadStringToEnvIfExist(doc, CONFIG_MQTT_CLIENT_ID, envData.mqttClientId);
                if (doc.containsKey(CONFIG_MQTT_PORT))
                    envData.mqttServerPort = doc[CONFIG_MQTT_PORT];

#endif
#ifdef USE_MQTT_HOME_ASSISTANT
                loadStringToEnvIfExist(doc, CONFIG_MQTT_HA_DEVICE_ID, envData.mqttHADeviceId);
                loadStringToEnvIfExist(doc, CONFIG_MQTT_HA_DEVICE_NAME, envData.mqttHADeviceName);
#endif

#ifdef USE_OTA_UPDATE
                loadStringToEnvIfExist(doc, CONFIG_OTA_HOSTNAME, envData.otaHostname);
                loadStringToEnvIfExist(doc, CONFIG_OTA_PASSWORD, envData.otaPassword);
#endif

#ifdef USE_NTP_SERVER
                loadStringToEnvIfExist(doc, CONFIG_NTP_SERVER, envData.ntpServer);
                loadStringToEnvIfExist(doc, CONFIG_NTP_TIMEZONE, envData.ntpTimezone);
#endif
            }
        }
    }
    LittleFS.end();
#endif
}

void Environment::loadStringToEnvIfExist(DynamicJsonDocument doc, const char *envKey, char *envValue) {
    if (doc.containsKey(envKey)) strcpy(envValue, doc[envKey]);
}

void Environment::resetEnvData() {
    LittleFS.begin();
    // load from file
    if (LittleFS.exists(CONFIG_PERSISTENCE)) {
        LittleFS.remove(CONFIG_PERSISTENCE);
    }
    LittleFS.end();
}

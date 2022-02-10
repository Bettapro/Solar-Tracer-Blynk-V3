
#include "wifi_manager.h"

#include "../incl/include_all_lib.h"
#include "../core/Environment.h"

bool formatLittleFS()
{
    LittleFS.end();
    LittleFS.begin();
    LittleFS.format();
    return LittleFS.begin();
}

void startWifiConfigurationAP(bool tryConnection)
{
    bool shouldSaveConfig = false;

    // making sure to be connected to wifi from settings to get WiFi.SSID and WiFi.psk
    if (tryConnection)
    {
        WiFi.waitForConnectResult();
    }

    WiFiManager wifiManager;
    wifiManager.setTitle("Solar-tracer-Blynk-V3");
    wifiManager.setBreakAfterConfig(true);
    wifiManager.setSaveParamsCallback([&shouldSaveConfig, &wifiManager]()
                                      {
                                      shouldSaveConfig = true;
                                      wifiManager.stopConfigPortal(); });
    wifiManager.setSaveConfigCallback([&shouldSaveConfig]()
                                      { shouldSaveConfig = true; });
    wifiManager.setParamsPage(true);
    wifiManager.setConfigPortalBlocking(true);
    wifiManager.setDebugOutput(false);

#ifdef USE_BLYNK
    WiFiManagerParameter custom_blynkLocalServerText("<p><b>BLYNK:</b></p>");
    wifiManager.addParameter(&custom_blynkLocalServerText);

    WiFiManagerParameter custom_BlynkAuth(CONFIG_PERSISTENCE_BLYNK_AUTH, "API key", Environment::getData()->blynkAuth, CONFIG_PERSISTENCE_BLYNK_AUTH_LEN);
    wifiManager.addParameter(&custom_BlynkAuth);

#ifndef USE_BLYNK_2

    WiFiManagerParameter customBlynkServerHostname(CONFIG_PERSISTENCE_BLYNK_HOSTNAME, "Hostname", Environment::getData()->blynkServerHostname, CONFIG_PERSISTENCE_BLYNK_HOSTNAME_LEN);
    WiFiManagerParameter custom_BlynkServerPort(CONFIG_PERSISTENCE_BLYNK_PORT, "Port", String(Environment::getData()->blynkServerPort).c_str(), 5, "type=\"number\" min=\"0\"");

    wifiManager.addParameter(&customBlynkServerHostname);
    wifiManager.addParameter(&custom_BlynkServerPort);
#endif
#endif

#ifdef USE_OTA_UPDATE
    WiFiManagerParameter customOtaUpdateText("<p><b>OTA:</b></p>");

    WiFiManagerParameter customOtaHostname(CONFIG_PERSISTENCE_OTA_HOSTNAME, "Hostname", Environment::getData()->otaHostname, CONFIG_PERSISTENCE_OTA_HOSTNAME_LEN);
    WiFiManagerParameter customOtaPassword(CONFIG_PERSISTENCE_OTA_PASSWORD, Text::password, Environment::getData()->otaPassword, CONFIG_PERSISTENCE_OTA_PASSWORD_LEN);

    wifiManager.addParameter(&customOtaUpdateText);
    wifiManager.addParameter(&customOtaHostname);
    wifiManager.addParameter(&customOtaPassword);
#endif

#ifdef USE_NTP_SERVER
    WiFiManagerParameter customNtpText("<p><b>NTP:</b></p>");

    WiFiManagerParameter customNtpServer(CONFIG_PERSISTENCE_NTP_SERVER, "Server", Environment::getData()->ntpServer, CONFIG_PERSISTENCE_NTP_SERVER_LEN);
    WiFiManagerParameter customNtpTimezone(CONFIG_PERSISTENCE_NTP_TIMEZONE, "Timezone", Environment::getData()->ntpTimezone, CONFIG_PERSISTENCE_NTP_TIMEZONE_LEN);

    wifiManager.addParameter(&customNtpText);
    wifiManager.addParameter(&customNtpServer);
    wifiManager.addParameter(&customNtpTimezone);
#endif

    WiFiManagerParameter custom_wmText("<p><b>ACCES POINT:</b></p>");
    WiFiManagerParameter custom_wmSSID(CONFIG_PERSISTENCE_WM_AP_SSID, "SSID", Environment::getData()->wmApSSID, CONFIG_PERSISTENCE_WM_AP_SSID_LEN);
    WiFiManagerParameter custom_wmPassword(CONFIG_PERSISTENCE_WM_AP_PASSWORD, Text::password, Environment::getData()->wmApPassword, CONFIG_PERSISTENCE_WM_AP_PASSWORD_LEN);

    wifiManager.addParameter(&custom_wmText);
    wifiManager.addParameter(&custom_wmSSID);
    wifiManager.addParameter(&custom_wmPassword);

    wifiManager.startConfigPortal(Environment::getData()->wmApSSID, Environment::getData()->wmApPassword);

    if (shouldSaveConfig)
    {
        debugPrintln("Saving wifimanager parameters... ");

        strcpy(Environment::getData()->wmApSSID, custom_wmSSID.getValue());
        strcpy(Environment::getData()->wmApPassword, custom_wmPassword.getValue());

#ifdef USE_BLYNK
#ifndef USE_BLYNK_2
        Environment::getData()->blynkLocalServer = strlen(customBlynkServerHostname.getValue()) > 0;
        strcpy(Environment::getData()->blynkServerHostname, !Environment::getData()->blynkLocalServer ? "" : customBlynkServerHostname.getValue());
        Environment::getData()->blynkServerPort = !Environment::getData()->blynkLocalServer ? 0 : String(custom_BlynkServerPort.getValue()).toInt();
#endif
        strcpy(Environment::getData()->blynkAuth, custom_BlynkAuth.getValue());
#endif

#ifdef USE_OTA_UPDATE
        strcpy(Environment::getData()->otaHostname, customOtaHostname.getValue());
        strcpy(Environment::getData()->otaPassword, customOtaPassword.getValue());
#endif

#ifdef USE_NTP_SERVER
        strcpy(Environment::getData()->ntpServer, customNtpServer.getValue());
        strcpy(Environment::getData()->ntpTimezone, customNtpTimezone.getValue());
#endif

        // if LittleFS is not usable
        if (!LittleFS.begin())
        {
            debugPrintf(true, Text::errorWithCode, 1);
            if (!formatLittleFS())
            {
                debugPrintf(true, Text::errorWithCode, 2);
            }
            return;
        }

        DynamicJsonDocument doc(2048);

        doc[CONFIG_PERSISTENCE_WIFI_SSID] = WiFi.SSID();
        doc[CONFIG_PERSISTENCE_WIFI_PASSWORD] = WiFi.psk();

        doc[CONFIG_PERSISTENCE_WM_AP_SSID] = Environment::getData()->wmApSSID;
        doc[CONFIG_PERSISTENCE_WM_AP_PASSWORD] = Environment::getData()->wmApPassword;
#ifdef USE_BLYNK
        doc[CONFIG_PERSISTENCE_BLYNK_AUTH] = Environment::getData()->blynkAuth;
#ifndef USE_BLYNK_2
        doc[CONFIG_PERSISTENCE_BLYNK_IS_LOCAL] = Environment::getData()->blynkLocalServer;
        doc[CONFIG_PERSISTENCE_BLYNK_HOSTNAME] = Environment::getData()->blynkServerHostname;
        doc[CONFIG_PERSISTENCE_BLYNK_PORT] = Environment::getData()->blynkServerPort;
#endif
#endif

#ifdef USE_OTA_UPDATE
        doc[CONFIG_PERSISTENCE_OTA_HOSTNAME] = Environment::getData()->otaHostname;
        doc[CONFIG_PERSISTENCE_OTA_PASSWORD] = Environment::getData()->otaPassword;
#endif

#ifdef USE_NTP_SERVER
        doc[CONFIG_PERSISTENCE_NTP_SERVER] = Environment::getData()->ntpServer;
        doc[CONFIG_PERSISTENCE_NTP_TIMEZONE] = Environment::getData()->ntpTimezone;
#endif

        File configFile = LittleFS.open(CONFIG_PERSISTENCE, "w");
        if (!configFile)
        {
            LittleFS.end();
        }
        else
        {
            serializeJson(doc, configFile);
            configFile.flush();
            configFile.close();
            LittleFS.end();
            debugPrintln(Text::ok);
        }
    }
}

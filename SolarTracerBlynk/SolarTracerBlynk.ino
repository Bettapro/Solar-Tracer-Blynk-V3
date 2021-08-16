//-------------------------------------------------------------
//  SOLAR-TRACER-BLYNK (V3)
//-------------------------------------------------------------
//
// Developed by @jaminNZx
// With modifications by @tekk
// With modifications by @bettapro
//

#include "src/incl/project_config.h"

extern SimpleTimer* mainTimer;

// -------------------------------------------------------------------------------
// MISC

#ifdef SYNC_ST_TIME
void getTimeFromServer()
{
  struct tm *ti;
  time_t tnow;
  char strftime_buf[64];

  tnow = time(nullptr) + 1;
  strftime(strftime_buf, sizeof(strftime_buf), "%c", localtime(&tnow));
  ti = localtime(&tnow);
  thisController->syncRealtimeClock(ti);
}
#endif

void uploadRealtimeAll()
{
  uploadRealtimeToBlynk();
}
void uploadStatsAll()
{
  uploadStatsToBlynk();
}

// ****************************************************************************
// SETUP and LOOP

void loop()
{
  blynkLoop();
#ifdef USE_OTA_UPDATE
  ArduinoOTA.handle();
#endif
  mainTimer->run();
}

void setup()
{
  setStatusError(STATUS_RUN_BOOTING);
#ifdef USE_STATUS_LED
  ledSetupStart();
#endif

  debugPrintln(" ++ STARTING TRACER-RS485-MODBUS-BLYNK");
  BOARD_DEBUG_SERIAL_STREAM.begin(BOARD_DEBUG_SERIAL_STREAM_BAUDRATE);

#ifdef USE_SERIAL_STREAM
#if defined(BOARD_ST_SERIAL_PIN_MAPPING_RX) & defined(BOARD_ST_SERIAL_PIN_MAPPING_TX)
  BOARD_ST_SERIAL_STREAM.begin(BOARD_ST_SERIAL_STREAM_BAUDRATE, SERIAL_8N1, BOARD_ST_SERIAL_PIN_MAPPING_RX, BOARD_ST_SERIAL_PIN_MAPPING_TX);
#else
  BOARD_ST_SERIAL_STREAM.begin(BOARD_ST_SERIAL_STREAM_BAUDRATE);
#endif
#endif

  controllerSetup();

  debugPrintln(" ++ Setting up WIFI:");
  debugPrintln("Connecting...");

  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    debugPrintln("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  debugPrintln("Connected.");


  blynkSetup();

#ifdef USE_NTP_SERVER
  debugPrintln(" ++ Setting up Local Time:");
  debugPrint("Waiting for NTP time...");
  configTime(0, 0, NTP_SERVER_CONNECT_TO);
  setenv("TZ", CURRENT_TIMEZONE, 3);
  tzset();

  while (time(nullptr) < 100000ul)
  {
    debugPrint(".");
    delay(10);
  }
  debugPrintln(" time synced");

  char strftime_buf[64];

  time_t tnow = time(nullptr) + 1;
  strftime(strftime_buf, sizeof(strftime_buf), "%c", localtime(&tnow));
  struct tm *ti = localtime(&tnow);

  debugPrint("My NOW is: ");
  debugPrint(ti->tm_year + 1900);
  debugPrint("-");
  debugPrint(ti->tm_mon + 1);
  debugPrint("-");
  debugPrint(ti->tm_mday);
  debugPrint(" ");
  debugPrint(ti->tm_hour);
  debugPrint(":");
  debugPrint(ti->tm_min);
  debugPrint(":");
  debugPrintln(ti->tm_sec);

#endif

  debugPrintln();
  debugPrintln("Connected to Blynk.");

#ifdef USE_OTA_UPDATE
  arduinoOtaSetup();

  debugPrint("ArduinoOTA running. ");
  debugPrint("IP address: ");
  debugPrintln(WiFi.localIP().toString());
#endif

  debugPrintln(" ++ Setting up solar tracer:");
#ifdef SYNC_ST_TIME
  debugPrintln("Set NTP time");
  getTimeFromServer();
  delay(500);
#endif
  debugPrintln("Get all values");
  thisController->fetchAllValues();
  delay(1000);
  debugPrintln("Send updates to Blynk");
  uploadRealtimeAll();
  uploadStatsAll();
  delay(1000);

  debugPrintln("Starting timed actions...");

  // periodically refresh tracer values
  mainTimer->setInterval(CONTROLLER_UPDATE_MS_PERIOD, updateSolarController);
  // periodically send STATS all value to blynk
  mainTimer->setInterval(SYNC_STATS_MS_PERIOD, uploadStatsAll);
  // periodically send REALTIME  value to blynk
  mainTimer->setInterval(SYNC_REALTIME_MS_PERIOD, uploadRealtimeAll);

  debugPrintln("SETUP OK!");
  debugPrintln("----------------------------");
  debugPrintln();

  clearStatusError(STATUS_RUN_BOOTING);
#ifdef USE_STATUS_LED
  ledSetupStop();
#endif
}
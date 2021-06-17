//-------------------------------------------------------------
//  SOLAR-TRACER-BLYNK (V3)
//-------------------------------------------------------------
//
// Developed by @jaminNZx
// With modifications by @tekk
// With modifications by @bettapro
//

#include "incl/project_config.h"
#include "incl/variables_contains.h"

// removes the intellisens error for setenv,tzset
// see: https://community.platformio.org/t/identifier-is-undefined-setenv-tzset/16162
_VOID _EXFUN(tzset, (_VOID));
int _EXFUN(setenv, (const char *__string, const char *__value, int __overwrite));

SolarTracer *thisController;

BlynkTimer timer;

// ****************************************************************************
// SETUP and LOOP

void loop()
{
  Blynk.run();
#ifdef USE_OTA_UPDATE
  ArduinoOTA.handle();
#endif
  timer.run();
}

void setup()
{
  BOARD_DEBUG_SERIAL_STREAM.println(" ++ STARTING TRACER-RS485-MODBUS-BLYNK");
  BOARD_DEBUG_SERIAL_STREAM.begin(BOARD_DEBUG_SERIAL_STREAM_BAUDRATE);

// Modbus slave ID 1
#ifdef USE_SERIAL_STREAM
  BOARD_ST_SERIAL_STREAM.begin(BOARD_ST_SERIAL_STREAM_BAUDRATE);
#endif
  thisController = new SOLAR_TRACER_INSTANCE;

  BOARD_DEBUG_SERIAL_STREAM.println(" ++ Setting up WIFI:");
  BOARD_DEBUG_SERIAL_STREAM.println("Connecting...");

  WiFi.mode(WIFI_STA);

#ifdef USE_BLYNK_LOCAL_SERVER
  Blynk.begin(BLYNK_AUTH, WIFI_SSID, WIFI_PASS, BLYNK_SERVER, BLYNK_PORT);
#else
  Blynk.begin(BLYNK_AUTH, WIFI_SSID, WIFI_PASS);
#endif

  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    BOARD_DEBUG_SERIAL_STREAM.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  BOARD_DEBUG_SERIAL_STREAM.println("Connected.");

#ifdef USE_NTP_SERVER
  BOARD_DEBUG_SERIAL_STREAM.println(" ++ Setting up Local Time:");
  BOARD_DEBUG_SERIAL_STREAM.print("Waiting for NTP time...");
  configTime(0, 0, NTP_SERVER);
  setenv("TZ", TIMEZONE, 3);
  tzset();

  while (time(nullptr) < 100000ul)
  {
    BOARD_DEBUG_SERIAL_STREAM.print(".");
    delay(10);
  }
  BOARD_DEBUG_SERIAL_STREAM.println(" time synced");

  char strftime_buf[64];

  time_t tnow = time(nullptr) + 1;
  strftime(strftime_buf, sizeof(strftime_buf), "%c", localtime(&tnow));
  struct tm *ti = localtime(&tnow);

  BOARD_DEBUG_SERIAL_STREAM.print("My NOW is: ");
  BOARD_DEBUG_SERIAL_STREAM.print(ti->tm_year + 1900);
  BOARD_DEBUG_SERIAL_STREAM.print("-");
  BOARD_DEBUG_SERIAL_STREAM.print(ti->tm_mon + 1);
  BOARD_DEBUG_SERIAL_STREAM.print("-");
  BOARD_DEBUG_SERIAL_STREAM.print(ti->tm_mday);
  BOARD_DEBUG_SERIAL_STREAM.print(" ");
  BOARD_DEBUG_SERIAL_STREAM.print(ti->tm_hour);
  BOARD_DEBUG_SERIAL_STREAM.print(":");
  BOARD_DEBUG_SERIAL_STREAM.print(ti->tm_min);
  BOARD_DEBUG_SERIAL_STREAM.print(":");
  BOARD_DEBUG_SERIAL_STREAM.println(ti->tm_sec);

#endif

  BOARD_DEBUG_SERIAL_STREAM.println(" ++ Setting up Blynk:");
  BOARD_DEBUG_SERIAL_STREAM.print("Connecting...");

  while (!Blynk.connect())
  {
    BOARD_DEBUG_SERIAL_STREAM.print(".");
    delay(100);
  }

  BOARD_DEBUG_SERIAL_STREAM.println();
  BOARD_DEBUG_SERIAL_STREAM.println("Connected to Blynk.");

#ifdef USE_OTA_UPDATE
  BOARD_DEBUG_SERIAL_STREAM.println("Starting ArduinoOTA...");

  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword((const char *)OTA_PASS);

  ArduinoOTA.onStart([]()
                     {
                       String type;
                       if (ArduinoOTA.getCommand() == U_FLASH)
                       {
                         type = "sketch";
                       }
                       else
                       { // U_SPIFFS
                         type = "filesystem";
                       }

                       // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                       BOARD_DEBUG_SERIAL_STREAM.println("Start updating " + type);
                     });

  ArduinoOTA.onEnd([]()
                   { BOARD_DEBUG_SERIAL_STREAM.println("\nEnd of update"); });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { BOARD_DEBUG_SERIAL_STREAM.printf("Progress: %u%%\r", (progress / (total / 100))); });

  ArduinoOTA.onError([](ota_error_t error)
                     {
                       BOARD_DEBUG_SERIAL_STREAM.printf("Error[%u]: ", error);
                       if (error == OTA_AUTH_ERROR)
                       {
                         BOARD_DEBUG_SERIAL_STREAM.println("Auth Failed");
                       }
                       else if (error == OTA_BEGIN_ERROR)
                       {
                         BOARD_DEBUG_SERIAL_STREAM.println("Begin Failed");
                       }
                       else if (error == OTA_CONNECT_ERROR)
                       {
                         BOARD_DEBUG_SERIAL_STREAM.println("Connect Failed");
                       }
                       else if (error == OTA_RECEIVE_ERROR)
                       {
                         BOARD_DEBUG_SERIAL_STREAM.println("Receive Failed");
                       }
                       else if (error == OTA_END_ERROR)
                       {
                         BOARD_DEBUG_SERIAL_STREAM.println("End Failed");
                       }
                     });

  ArduinoOTA.begin();

  BOARD_DEBUG_SERIAL_STREAM.print("ArduinoOTA running. ");
  BOARD_DEBUG_SERIAL_STREAM.print("IP address: ");
  BOARD_DEBUG_SERIAL_STREAM.println(WiFi.localIP());
#endif

  BOARD_DEBUG_SERIAL_STREAM.println(" ++ Setting up solar tracer:");
#ifdef SYNC_ST_TIME
  BOARD_DEBUG_SERIAL_STREAM.println("Set NTP time");
  getTimeFromServer();
  delay(500);
#endif
  BOARD_DEBUG_SERIAL_STREAM.println("Get all values");
  thisController->fetchAllValues();
  delay(1000);
  BOARD_DEBUG_SERIAL_STREAM.println("Send updates to Blynk");
  uploadRealtimeToBlynk();
  uploadStatsToBlynk();
  delay(1000);

  BOARD_DEBUG_SERIAL_STREAM.println("Starting timed actions...");

  // periodically refresh tracer values
  timer.setInterval(CONTROLLER_UPDATE_MS_PERIOD, updateSolarController);
  // periodically send STATS all value to blynk
  timer.setInterval(BLINK_SYNC_STATS_MS_PERIOD, uploadStatsToBlynk);
  // periodically send REALTIME  value to blynk
  timer.setInterval(BLINK_SYNC_REALTIME_MS_PERIOD, uploadRealtimeToBlynk);

  BOARD_DEBUG_SERIAL_STREAM.println("SETUP OK!");
  BOARD_DEBUG_SERIAL_STREAM.println("----------------------------");
  BOARD_DEBUG_SERIAL_STREAM.println();
}

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

// --------------------------------------------------------------------------------
// BLYNK CALLBACKS

#ifdef vPIN_LOAD_ENABLED
BLYNK_WRITE(vPIN_LOAD_ENABLED)
{
  uint8_t newState = (uint8_t)param.asInt();

  BOARD_DEBUG_SERIAL_STREAM.print("Setting load state output coil to value: ");
  BOARD_DEBUG_SERIAL_STREAM.println(newState);

  if (thisController->writeBoolValue(SolarTracerVariables::LOAD_MANUAL_ONOFF,
                                     newState > 0))
  {
    BOARD_DEBUG_SERIAL_STREAM.println("Write & Read suceeded.");
  }
  else
  {
    BOARD_DEBUG_SERIAL_STREAM.println("Write & Read failed.");
  }
  thisController->fetchValue(SolarTracerVariables::LOAD_MANUAL_ONOFF);

  BOARD_DEBUG_SERIAL_STREAM.println("Uploading results to Blynk.");

  uploadRealtimeToBlynk();
}
#endif

#ifdef vPIN_CHARGE_DEVICE_ENABLED
BLYNK_WRITE(vPIN_CHARGE_DEVICE_ENABLED)
{
  uint8_t newState = (uint8_t)param.asInt();

  BOARD_DEBUG_SERIAL_STREAM.print("Setting load state output coil to value: ");
  BOARD_DEBUG_SERIAL_STREAM.println(newState);

  if (thisController->writeBoolValue(SolarTracerVariables::CHARGING_DEVICE_ONOFF,
                                     newState > 0))
  {
    BOARD_DEBUG_SERIAL_STREAM.println("Write & Read suceeded.");
  }
  else
  {
    BOARD_DEBUG_SERIAL_STREAM.println("Write & Read failed.");
  }
  thisController->fetchValue(SolarTracerVariables::CHARGING_DEVICE_ONOFF);

  BOARD_DEBUG_SERIAL_STREAM.println("Uploading results to Blynk.");

  uploadRealtimeToBlynk();
}
#endif

// --------------------------------------------------------------------------------
// SOLAR TRACER SETUP

void updateSolarController()
{

  if (thisController->updateRun())
  {
    BOARD_DEBUG_SERIAL_STREAM.println("Update Solar-Tracer SUCCESS!");
  }
  else
  {
    BOARD_DEBUG_SERIAL_STREAM.println("Update Solar-Tracer FAILED!");
  }
}

// upload values realtime
void uploadRealtimeToBlynk()
{
  bool varNotReady = false;
  for (uint8_t index = 0; index < realTimeVirtualBlynkSolarVariablesCount; index++)
  {
    if (thisController->isVariableReadReady(realTimeVirtualBlynkSolarVariables[index].solarVariable))
    {
      switch (SolarTracer::getVariableDatatype(realTimeVirtualBlynkSolarVariables[index].solarVariable))
      {
      case SolarTracerVariablesDataType::FLOAT:
        Blynk.virtualWrite(realTimeVirtualBlynkSolarVariables[index].virtualPin, thisController->getFloatValue(realTimeVirtualBlynkSolarVariables[index].solarVariable));
        break;
      case SolarTracerVariablesDataType::STRING:
        Blynk.virtualWrite(realTimeVirtualBlynkSolarVariables[index].virtualPin, thisController->getStringValue(realTimeVirtualBlynkSolarVariables[index].solarVariable));
        break;
      default:
        break;
      }
    }
    else{
      varNotReady = true;
    }
  }
  if(varNotReady){
    BOARD_DEBUG_SERIAL_STREAM.println("Some RT variables are not ready and not synced!");
  }
}

// upload values stats
void uploadStatsToBlynk()
{
  bool varNotReady = false;
  for (uint8_t index = 0; index < statVirtualBlynkSolarVariablesCount; index++)
  {
    if (thisController->isVariableReadReady(statVirtualBlynkSolarVariables[index].solarVariable))
    {
      switch (SolarTracer::getVariableDatatype(statVirtualBlynkSolarVariables[index].solarVariable))
      {
      case SolarTracerVariablesDataType::FLOAT:
        Blynk.virtualWrite(statVirtualBlynkSolarVariables[index].virtualPin, thisController->getFloatValue(statVirtualBlynkSolarVariables[index].solarVariable));
        break;
      case SolarTracerVariablesDataType::STRING:
        Blynk.virtualWrite(statVirtualBlynkSolarVariables[index].virtualPin, thisController->getStringValue(statVirtualBlynkSolarVariables[index].solarVariable));
        break;
      default:
        break;
      }
    }
    else{
      varNotReady = true;
    }
  }
  if(varNotReady){
    BOARD_DEBUG_SERIAL_STREAM.println("Some ST variables are not ready and not synced!");
  }
}


#include "datetime.h"

#include "../incl/project_core_config.h"
#include "../core/debug.h"
#include "../core/Environment.h" 

#ifdef USE_NTP_SERVER
void setupDatetimeFromNTP()
{
    debugPrint("Waiting for NTP time...");
    configTzTime(Environment::getData()->ntpTimezone, Environment::getData()->ntpServer);

    while (time(nullptr) < 100000ul)
    {
        debugPrint(".");
        delay(500);
    }
    debugPrintln("OK");
}
#endif

struct tm *getMyNowTm()
{
    time_t tnow = time(nullptr) + 1;
    return localtime(&tnow);
}

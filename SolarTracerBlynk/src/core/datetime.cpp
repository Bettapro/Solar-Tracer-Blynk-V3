
#include "datetime.h"

#include "../incl/include_all_core.h"
#include "../core/Environment.h" 

#ifdef USE_NTP_SERVER
void setupDatetimeFromNTP()
{
    debugPrint("Waiting for NTP time");
    configTzTime(Environment::getData()->ntpTimezone, Environment::getData()->ntpServer);

    while (time(nullptr) < 100000ul)
    {
        debugPrint(Text::dot);
        delay(500);
    }
    debugPrintln(Text::ko);
}
#endif

struct tm *getMyNowTm()
{
    time_t tnow = time(nullptr) + 1;
    return localtime(&tnow);
}

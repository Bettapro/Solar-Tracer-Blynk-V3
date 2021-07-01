#pragma once

extern SimpleTimer* mainTimer;

int ledActTimer = -1;
bool ledStatus = true;

void ledSetupStart()
{
    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, HIGH);
}

void ledSetupStop()
{
    digitalWrite(STATUS_LED_PIN, LOW);
}

void ledTimerCallback()
{
    digitalWrite(STATUS_LED_PIN, ledStatus);
    ledStatus = !ledStatus;
}

void changeStatusLedTimerInterval(unsigned long newInterval)
{
    if (newInterval <= 0)
    {
        if (ledActTimer >= 0)
        {
            mainTimer->deleteTimer(ledActTimer);
            digitalWrite(STATUS_LED_PIN, LOW);
            ledActTimer = -1;
        }
        return;
    }

    if (ledActTimer < 0)
    {
        ledActTimer = mainTimer->setInterval(newInterval, ledTimerCallback);
    }
    else
    {
        mainTimer->changeInterval(ledActTimer, newInterval);
    }
}

void notifyStatusLed(uint8_t newStatus)
{
    changeStatusLedTimerInterval(newStatus == 0 ? 2000 : 0);
}

#pragma once

#if defined ESP32
#include "esp32_config.h"
#elif defined ESP8266
#include "esp8266_config.h"
#else
#error This board is not supported.
#endif
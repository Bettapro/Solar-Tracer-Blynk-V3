@echo off
SETLOCAL EnableDelayedExpansion

rmdir "binFiles" /S /Q

set VERSION_number="3.0.6"

set buildRun[0].env=esp32dev
set buildRun[0].fileSuffix=blynk
set buildRun[0].buildFlags=

set buildRun[1].env=esp32dev
set buildRun[1].fileSuffix=mqtt
set buildRun[1].buildFlags=-DUSE_NOT_BLYNK -DUSE_MQTT

set buildRun[2].env=esp32dev
set buildRun[2].fileSuffix=mqttHA
set buildRun[2].buildFlags=-DUSE_NOT_BLYNK -DUSE_MQTT -DUSE_MQTT_HOME_ASSISTANT

set buildRun[3].env=esp8266
set buildRun[3].fileSuffix=blynk
set buildRun[3].buildFlags=

set buildRun[4].env=esp8266
set buildRun[4].fileSuffix=mqtt
set buildRun[4].buildFlags=-DUSE_NOT_BLYNK -DUSE_MQTT

set buildRun[5].env=esp8266
set buildRun[5].fileSuffix=mqttHA
set buildRun[5].buildFlags=-DUSE_NOT_BLYNK -DUSE_MQTT -DUSE_MQTT_HOME_ASSISTANT


set "x=0"

:SymLoop
if defined buildRun[%x%].env (
    set currEnv=%%buildRun[%x%].env%%
    set currFileSuffix=%%buildRun[%x%].fileSuffix%%
    set currBuildFlags=%%buildRun[%x%].buildFlags%%
    call echo "ENV !currEnv! - file !currFileSuffix! - flag !currBuildFlags!"
    
    call set PLATFORMIO_BUILD_FLAGS=!currBuildFlags!
    rem call set PLATFORMIO_DEFAULT_ENVS=!currEnv!
    call %userprofile%\.platformio\penv\Scripts\pio.exe run -e !currEnv!
    call echo F| call xcopy ".pio\build\!currEnv!\firmware.bin" "binFiles\SolarTracerBlynk_%VERSION_NUMBER%_!currEnv!_!currFileSuffix!.bin" /v /f /y

    set /a "x+=1"
    GOTO :SymLoop
)

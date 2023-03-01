@echo off
SETLOCAL EnableDelayedExpansion

rmdir "binFiles" /S /Q

set VERSION_number="3.0.6"

set esp32_build_flag_base=-DUSE_HALL_AP_CONFIGURATION_TRIGGER
set esp8266_build_flag_base=


set buildRun[0].env=esp32dev
set buildRun[0].fileSuffix=run
set buildRun[0].buildFlags=%esp32_build_flag_base% -DUSE_NOT_BLYNK -DUSE_MQTT -DUSE_MQTT_HOME_ASSISTANT -DUSE_EXTERNAL_HEAVY_LOAD_CURRENT_METER -DUSE_HALL_AP_CONFIGURATION_TRIGGER


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

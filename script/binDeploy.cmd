@echo off
SETLOCAL EnableDelayedExpansion

rmdir "binFiles" /S /Q

set VERSION_number="3.0.9"

set esp32_build_flag_base=-DUSE_HALL_AP_CONFIGURATION_TRIGGER
set esp32_build_flag_minimum=-DUSE_NOT_BLYNK -DSTARTUP_BUILD
set esp8266_build_flag_base=

set buildRun[0].env=esp32dev
set buildRun[0].fileSuffix=blynk
set buildRun[0].buildFlags=%esp32_build_flag_base%

set buildRun[1].env=esp32dev
set buildRun[1].fileSuffix=mqtt
set buildRun[1].buildFlags=%esp32_build_flag_base% -DUSE_NOT_BLYNK -DUSE_MQTT 

set buildRun[2].env=esp32dev
set buildRun[2].fileSuffix=mqttHA
set buildRun[2].buildFlags=%esp32_build_flag_base% -DUSE_NOT_BLYNK -DUSE_MQTT -DUSE_MQTT_HOME_ASSISTANT

set buildRun[3].env=esp8266
set buildRun[3].fileSuffix=blynk
set buildRun[3].buildFlags=%esp8266_build_flag_base%

set buildRun[4].env=esp8266
set buildRun[4].fileSuffix=mqtt
set buildRun[4].buildFlags=%esp8266_build_flag_base% -DUSE_NOT_BLYNK -DUSE_MQTT

set buildRun[5].env=esp8266
set buildRun[5].fileSuffix=mqttHA
set buildRun[5].buildFlags=%esp8266_build_flag_base% -DUSE_NOT_BLYNK -DUSE_MQTT -DUSE_MQTT_HOME_ASSISTANT

set buildRun[6].env=esp8266
set buildRun[6].fileSuffix=blynk_swser
set buildRun[6].buildFlags=%esp8266_build_flag_base% -DUSE_SOFTWARE_SERIAL

set buildRun[7].env=esp8266
set buildRun[7].fileSuffix=mqtt_swser
set buildRun[7].buildFlags=%esp8266_build_flag_base% -DUSE_NOT_BLYNK -DUSE_MQTT -DUSE_SOFTWARE_SERIAL

set buildRun[8].env=esp8266
set buildRun[8].fileSuffix=mqttHA_swser
set buildRun[8].buildFlags=%esp8266_build_flag_base% -DUSE_NOT_BLYNK -DUSE_MQTT -DUSE_MQTT_HOME_ASSISTANT -DUSE_SOFTWARE_SERIAL


set esp32devEnv=esp32dev
call set PLATFORMIO_BUILD_FLAGS=%esp32_build_flag_minimum%
call %userprofile%\.platformio\penv\Scripts\pio.exe run -e %esp32devEnv% 
echo F| xcopy ".pio\build\%esp32devEnv%\firmware.bin" "binFiles\%esp32devEnv%\firmware.bin" /v /f /y
echo F| xcopy ".pio\build\%esp32devEnv%\partitions.bin" "binFiles\%esp32devEnv%\partitions.bin" /v /f /y
echo F| xcopy ".pio\build\%esp32devEnv%\bootloader.bin" "binFiles\%esp32devEnv%\bootloader.bin" /v /f /y
echo F| xcopy "%userprofile%\.platformio\packages\framework-arduinoespressif32\tools\partitions\boot_app0.bin" "binFiles\%esp32devEnv%\boot_app0.bin" /v /f /y
python script\esp32_binary_merger\merge_bin_esp.py --output_folder binFiles\ --output_name "SolarTracerBlynk_%VERSION_NUMBER%_%esp32devEnv%_0x0.bin" --bin_path "binFiles\%esp32devEnv%\bootloader.bin" "binFiles\%esp32devEnv%\partitions.bin" "binFiles\%esp32devEnv%\boot_app0.bin" "binFiles\%esp32devEnv%\firmware.bin" --bin_address 0x1000 0x8000 0xe000 0x10000

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

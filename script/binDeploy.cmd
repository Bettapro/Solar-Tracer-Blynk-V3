@echo off

RMDIR "binFiles" /S /Q

set VERSION_number="3.0.2"

set CURRENT_ENV=esp32dev
pio run -e %CURRENT_ENV%
echo F| xcopy ".pio\build\%CURRENT_ENV%\firmware.bin" "binFiles\%CURRENT_ENV%\firmware.bin" /v /f /y
echo F| xcopy ".pio\build\%CURRENT_ENV%\partitions.bin" "binFiles\%CURRENT_ENV%\partitions.bin" /v /f /y
echo F| xcopy "%userprofile%\.platformio\packages\framework-arduinoespressif32\tools\sdk\esp32\bin\bootloader_dio_40m.bin" "binFiles\%CURRENT_ENV%\bootloader.bin" /v /f /y
echo F| xcopy "%userprofile%\.platformio\packages\framework-arduinoespressif32\tools\partitions\boot_app0.bin" "binFiles\%CURRENT_ENV%\boot_app0.bin" /v /f /y
python script\esp32_binary_merger\merge_bin_esp.py --output_folder binFiles\ --output_name "SolarTracerBlynk_%VERSION_NUMBER%_%CURRENT_ENV%_FULL.bin" --bin_path "binFiles\%CURRENT_ENV%\bootloader.bin" "binFiles\%CURRENT_ENV%\partitions.bin" "binFiles\%CURRENT_ENV%\boot_app0.bin" "binFiles\%CURRENT_ENV%\firmware.bin" --bin_address 0x1000 0x8000 0xe000 0x10000
echo F| xcopy "binFiles\%CURRENT_ENV%\firmware.bin" "binFiles\SolarTracerBlynk_%VERSION_NUMBER%_%CURRENT_ENV%.bin" /v /f /y

set CURRENT_ENV=esp8266
pio run -e %CURRENT_ENV%
rem 0x0 .pio\build\esp8266\firmware.bin
echo F| xcopy ".pio\build\%CURRENT_ENV%\firmware.bin" "binFiles\SolarTracerBlynk_%VERSION_NUMBER%_%CURRENT_ENV%.bin" /v /f /y
                                                                                            
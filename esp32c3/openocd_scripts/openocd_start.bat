@echo off
set BOARD=%1

if "%BOARD%"=="" (
    echo Uso: openocd_start.bat [board]
    echo Boards disponibles:
    echo   esp32c3
    echo   esp32c6
    echo   esp32h2
    exit /b
)

if "%BOARD%"=="esp32c3" (
    echo Board seleccionada: esp32c3
    openocd -c "bindto 0.0.0.0" -f openscript_esp32c3.cfg
    exit /b
)

if "%BOARD%"=="esp32c6" (
    echo Board seleccionada: esp32c6
    openocd -c "bindto 0.0.0.0" -f openscript_esp32c6.cfg
    exit /b
)
if "%BOARD%"=="esp32h2" (
    echo Board seleccionada: esp32h2
    openocd -c "bindto 0.0.0.0" -f openscript_esp32h2.cfg
    exit /b
)

echo Board no reconocida: %BOARD%
echo Boards disponibles: esp32c3 ^| esp32c6 ^| esp32h2
exit /b

#!/bin/bash

# Verifica si se pasó un argumento
if [ -z "$1" ]; then
  echo "Uso: ./openocd_start.sh [board]"
  echo "Boards disponibles:"
  echo "  esp32c3"
  echo "  esp32c6"
  echo "  esp32h2"
  exit 1
fi

case "$1" in
  esp32c3)
    echo "Board seleccionada: esp32c3"
    openocd -c "bindto 0.0.0.0" -f ./openscript_esp32c3.cfg
    ;;
  esp32c6)
    echo "Board seleccionada: esp32c6"
    openocd -c "bindto 0.0.0.0"  -f ./openscript_esp32c6.cfg
    ;;
  esp32h2)
    echo "Board seleccionada: esp32h2"
    openocd -c "bindto 0.0.0.0"  -f ./openscript_esp32h2.cfg
    ;;  
  *)
    echo "Board no reconocida: $1"
    echo "Boards disponibles: esp32c3 | esp32c6 | esp32h2"
    exit 1
    ;;
esac

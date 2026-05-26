# Instalación y compilación

Esta guía resume cómo preparar el entorno para compilar y cargar ManipulAR en una placa Arduino compatible.

## Entorno recomendado

- Arduino IDE 2.x o Arduino IDE 1.8.x.
- Placa Arduino Uno, Nano o compatible AVR.
- Cable USB para cargar el firmware.

## Librerías externas

ManipulAR necesita una biblioteca compatible con:

- `LiquidCrystal_I2C`

Se puede instalar desde el Library Manager del Arduino IDE buscando `LiquidCrystal_I2C`.

También usa librerías incluidas con el entorno Arduino AVR:

- `Arduino.h`
- `EEPROM`
- `avr/pgmspace`

## Bibliotecas incluidas en el proyecto

No hace falta instalar `MorseEnDecoder` por separado.

ManipulAR incluye una copia local modificada:

- `MorseEnDecoder.cpp`
- `MorseEnDecoder.h`
- `pitches.h`

Esta copia permite ajustar el tono de audio desde el menú de preferencias.

## Archivos del firmware

Para compilar correctamente, todos estos archivos deben estar en la misma carpeta que `ManipulAR.ino`:

```text
ManipulAR.ino
buttons.cpp
buttons.h
config.h
cw_engine.cpp
cw_engine.h
display.cpp
display.h
keyer.cpp
keyer.h
menu.cpp
menu.h
prefs.cpp
prefs.h
trainer.cpp
trainer.h
MorseEnDecoder.cpp
MorseEnDecoder.h
pitches.h
```

## Compilar con Arduino IDE

1. Abrir Arduino IDE.
2. Instalar `LiquidCrystal_I2C` desde el Library Manager.
3. Abrir `ManipulAR.ino`.
4. Seleccionar la placa, por ejemplo `Arduino Uno` o `Arduino Nano`.
5. Seleccionar el puerto serie correspondiente.
6. Usar `Verify` para compilar.
7. Usar `Upload` para cargar el firmware.

## Configuración principal

Los pines, dirección del LCD y versión del firmware se definen en:

```text
config.h
```

Valores habituales:

- LCD I2C en dirección `0x27`.
- LCD 16x2.
- Arduino Uno/Nano compatible.

Si el LCD no muestra texto, revisar primero la dirección I2C. Algunos módulos usan `0x3F`.

## Primera carga

En el primer arranque, ManipulAR usa valores por defecto. Las preferencias se guardan en EEPROM cuando se modifican desde el menú.

Si se carga una versión nueva sobre una EEPROM ya usada, el firmware valida los valores guardados y corrige los que estén fuera de rango.

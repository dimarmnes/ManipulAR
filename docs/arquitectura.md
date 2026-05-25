# Arquitectura del firmware

ManipulAR está organizado en módulos para facilitar mantenimiento, lectura y futuras ampliaciones.

## Punto de entrada

### `ManipulAR.ino`

Inicializa el hardware, carga preferencias, configura el motor Morse y muestra la pantalla de inicio.

Luego ejecuta el bucle principal, donde se selecciona el modo activo desde el menú:

- Entrenar RX
- Entrenar TX
- Decodificador
- Test PARIS
- Keyer radio
- Preferencias

## Configuración global

### `config.h`

Define:

- versión del proyecto;
- pines de entrada y salida;
- configuración de LCD;
- constantes globales;
- valores relacionados con conectores y hardware.

## Pantalla

### `display.cpp` / `display.h`

Centraliza funciones relacionadas con el LCD:

- inicialización;
- limpieza de pantalla;
- mensajes simples;
- limpieza de líneas.

## Botones

### `buttons.cpp` / `buttons.h`

Lee los tres pulsadores de navegación:

- UP;
- DOWN;
- SELECT.

Devuelve códigos simples para que los demás módulos no dependan directamente de `digitalRead()`.

## Menú y preferencias

### `menu.cpp` / `menu.h`

Gestiona:

- menú principal;
- navegación de preferencias;
- edición de valores;
- editor de lista personal.

### `prefs.cpp` / `prefs.h`

Gestiona:

- valores por defecto;
- lectura y escritura en EEPROM;
- lista de caracteres personal;
- frecuencia de tono;
- validación de preferencias.

## Entrenador

### `trainer.cpp` / `trainer.h`

Contiene los modos principales de práctica:

- recepción;
- transmisión;
- decodificador;
- test PARIS;
- keyer radio como modo accesible desde menú.

Coordina el motor Morse, el keyer, la pantalla, los botones y las preferencias activas.

## Keyer

### `keyer.cpp` / `keyer.h`

Implementa:

- llave recta;
- iámbica A;
- iámbica B;
- temporización de DIT/DAH;
- control de salida hacia radio;
- decodificación interna de lo manipulado.

La salida de radio queda bloqueada por defecto y solo se habilita explícitamente en modo Keyer radio.

## Motor CW automático

### `cw_engine.cpp` / `cw_engine.h`

Actúa como envoltorio sencillo sobre `MorseEncoder`, para enviar caracteres y ajustar velocidad sin exponer detalles internos de la biblioteca en todo el proyecto.

## Biblioteca MorseEnDecoder

### `MorseEnDecoder.cpp` / `MorseEnDecoder.h`

Copia local modificada de la biblioteca original de raron.

ManipulAR la usa para:

- generar CW automático;
- decodificar entrada Morse;
- controlar tonos de salida y sidetone.

La copia local permite ajustar la frecuencia del tono en tiempo de ejecución.

## Separación de salidas

ManipulAR separa la salida interna de entrenamiento de la salida hacia radio:

| Salida | Uso |
| --- | --- |
| D8 / `cwOutPin` | Generador automático interno |
| D7 / `radioKeyPin` | Manipulación de radio solo en Keyer radio |

Esta separación evita transmitir accidentalmente durante prácticas de entrenamiento.

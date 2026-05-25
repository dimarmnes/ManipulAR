#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

#define APP_VERSION "v1.0"

// Pines y conectores
// Jack de llave/paleta TRS: Tip=DIT o llave recta, Ring=DAH, Sleeve=GND.
// Jack de radio: usar radioKeyPin como salida de manipulacion hacia transistor/opto.
// D7/radioKeyPin se habilita solo en Keyer radio; evita transmitir durante practicas.
// cwOutPin es salida del generador automatico; dejar sin conectar a la radio.
// Jack de audio externo: conviene resolverlo con contacto conmutado para cortar el buzzer interno.
const byte morseInPin = 2;
const byte paddleDahPin = 3;

const byte beep_pin   = 11;
const byte BTN_UP     = 4;
const byte BTN_DOWN   = 5;
const byte BTN_SELECT = 6;
const byte radioKeyPin = 7;
const byte cwOutPin    = 8;

// Valor inicial de la lista editable "Personal"; luego se guarda en EEPROM.
#define DEFAULT_PERSONAL_CHARS "ETAN"

// LCD
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

#endif

#include <EEPROM.h>
#include "prefs.h"
#include "config.h"

// Mapa EEPROM:
// - prefs[0..NUM_PREFS-1]: preferencias numericas y marca SAVED_FLG.
// - PERSONAL_CHARS_ADDR..: lista editable Personal.
// - TONE_PITCH_ADDR: frecuencia de sidetone. Va separada para no mover Personal.
byte prefs[NUM_PREFS];

// Lista del set "Personal". Ocupa EEPROM a partir de PERSONAL_CHARS_ADDR.
char personal_chars[PERSONAL_CHARS_LEN + 1];

// Frecuencia de tono en decenas de Hz: 70 => 700 Hz.
// Se guarda asi porque EEPROM trabaja comodo con bytes.
byte tone_pitch = TONE_PITCH_DEFAULT;

static bool personal_char_valid(char c) {
  return (c >= 'A' && c <= 'Z') ||
         (c >= '0' && c <= '9') ||
         c == '.' || c == ',' || c == '/' || c == '?' || c == '=';
}

static void personal_chars_defaults() {
  byte i = 0;
  for (; i < PERSONAL_CHARS_LEN && DEFAULT_PERSONAL_CHARS[i] != '\0'; i++) {
    personal_chars[i] = DEFAULT_PERSONAL_CHARS[i];
  }
  personal_chars[i] = '\0';
}

// Normaliza la lista personal leida desde EEPROM y evita caracteres no soportados.
static void personal_chars_sanitize() {
  bool any = false;

  for (byte i = 0; i < PERSONAL_CHARS_LEN; i++) {
    char c = personal_chars[i];
    if (c == '\0') break;
    if (c >= 'a' && c <= 'z') c -= 32;
    if (!personal_char_valid(c)) {
      personal_chars[i] = '\0';
      break;
    }
    personal_chars[i] = c;
    any = true;
  }

  personal_chars[PERSONAL_CHARS_LEN] = '\0';
  if (!any) personal_chars_defaults();
}

static void personal_chars_load() {
  for (byte i = 0; i < PERSONAL_CHARS_LEN; i++) {
    personal_chars[i] = EEPROM.read(PERSONAL_CHARS_ADDR + i);
  }
  personal_chars[PERSONAL_CHARS_LEN] = '\0';
  personal_chars_sanitize();
}

unsigned int tone_frequency_hz() {
  return (unsigned int)tone_pitch * 10;
}

static void tone_pitch_sanitize() {
  if (tone_pitch < TONE_PITCH_MIN || tone_pitch > TONE_PITCH_MAX) {
    tone_pitch = TONE_PITCH_DEFAULT;
  }
}

static void tone_pitch_load() {
  tone_pitch = EEPROM.read(TONE_PITCH_ADDR);
  tone_pitch_sanitize();
}

static void tone_pitch_save() {
  EEPROM.write(TONE_PITCH_ADDR, tone_pitch);
}

static void personal_chars_save() {
  for (byte i = 0; i < PERSONAL_CHARS_LEN; i++) {
    EEPROM.write(PERSONAL_CHARS_ADDR + i, personal_chars[i]);
    if (personal_chars[i] == '\0') {
      for (byte j = i + 1; j < PERSONAL_CHARS_LEN; j++) {
        EEPROM.write(PERSONAL_CHARS_ADDR + j, '\0');
      }
      break;
    }
  }
}


static void prefs_defaults() {
  prefs[SAVED_FLG] = 0;
  prefs[GROUP_NUM] = 1;
  prefs[GROUP_DLY] = 0;
  prefs[KEY_SPEED] = KEY_SPEED_DEFAULT;
  prefs[CHAR_SET]  = 5;
  prefs[KOCH_NUM]  = 5;
  prefs[KOCH_SKIP] = 0;
  prefs[RESERVED_OUT_MODE] = 0;
  prefs[SHOW_DLY]  = 3;
  prefs[KEYER_MODE] = KEYER_STRAIGHT;
}

// Corrige valores fuera de rango por cambios de version o EEPROM sin inicializar.
// Esto permite cargar firmware nuevo sin depender de borrar EEPROM manualmente.
static void prefs_sanitize() {
  if (prefs[GROUP_NUM] < 1 || prefs[GROUP_NUM] > 15) prefs[GROUP_NUM] = 1;
  if (prefs[GROUP_DLY] > 20) prefs[GROUP_DLY] = 0;
  if (prefs[KEY_SPEED] < KEY_SPEED_MIN || prefs[KEY_SPEED] > KEY_SPEED_MAX) prefs[KEY_SPEED] = KEY_SPEED_DEFAULT;
  if (prefs[CHAR_SET] < 1 || prefs[CHAR_SET] > 6) prefs[CHAR_SET] = 5;
  if (prefs[KOCH_NUM] < 1 || prefs[KOCH_NUM] > 40) prefs[KOCH_NUM] = 5;
  if (prefs[KOCH_SKIP] > 39) prefs[KOCH_SKIP] = 0;
  if (prefs[KOCH_SKIP] > prefs[KOCH_NUM]) prefs[KOCH_SKIP] = 0;
  prefs[RESERVED_OUT_MODE] = 0;
  if (prefs[SHOW_DLY] < 1 || prefs[SHOW_DLY] > 10) prefs[SHOW_DLY] = 3;
  if (prefs[KEYER_MODE] > KEYER_IAMBIC_B) prefs[KEYER_MODE] = KEYER_STRAIGHT;
}

// Carga preferencias. Si no hay marca valida, inicia defaults sin escribir EEPROM hasta guardar.
void prefs_init() {

  if (EEPROM.read(0) == 170) {

    for (int i = 0; i < NUM_PREFS; i++) {
      prefs[i] = EEPROM.read(i);
    }
    prefs_sanitize();
    personal_chars_load();
    tone_pitch_load();

  } else {

    prefs_defaults();
    personal_chars_defaults();
    tone_pitch = TONE_PITCH_DEFAULT;
  }
}

// Guarda preferencias y lista personal. La marca 170 indica configuracion valida.
void prefs_save() {

  for (int i = 0; i < NUM_PREFS; i++) {
    EEPROM.write(i, prefs[i]);
  }

  personal_chars_save();
  tone_pitch_save();
  EEPROM.write(0, 170);
}

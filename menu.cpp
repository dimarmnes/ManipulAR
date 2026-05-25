#include <Arduino.h>
#include "menu.h"
#include "display.h"
#include "buttons.h"
#include "prefs.h"
#include "config.h"

#define PREF_PERSONAL_LIST 255
#define PREF_TONE_PITCH 254

// Caracteres disponibles para editar la lista personal; _ significa fin de lista.
static char personal_editor_chars[] = "_ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,/?=";


//====================
// Menu Principal
//====================
byte get_mode() {
  const char* menu[] = {
    "1-Entrenar RX   ",
    "2-Entrenar TX   ",
    "3-Decodificador ",
    "4-Test PARIS    ",
    "5-Keyer radio   ",
    "6-Preferencias  "
  };
  const byte n_entry = 6; 
  byte entry = 0;
  byte buttons = 0;
  boolean done = false;

  char line1[17];
  sprintf(line1, "ManipulAR %dppm", prefs[KEY_SPEED]);
  displayMessage(line1, "");

  do {
    lcd.setCursor(0, 1);
    lcd.print(menu[entry]);
    delay(150);

    buttons = readButtons();
    if (buttons == 2) {  // UP
      if (entry > 0) entry--;
      else entry = n_entry - 1;  // si está en 1 va a la última
    }
    if (buttons == 1) {  // DOWN
      if (entry < n_entry - 1) entry++;
      else entry = 0;  // si está en la última va a la 1
    }
    if (buttons == 3) {
      while (readButtons());
      done = true;
    }
  } while (!done);

  return entry + 1;
}


static byte personal_char_index(char c) {
  if (c == '\0') return 0;
  if (c >= 'a' && c <= 'z') c -= 32;
  for (byte i = 0; personal_editor_chars[i] != '\0'; i++) {
    if (personal_editor_chars[i] == c) return i;
  }
  return 0;
}

static void reproducir_muestra_tono() {
  tone(beep_pin, tone_frequency_hz(), 80);
  delay(95);
  noTone(beep_pin);
}

static void set_personal_char(byte pos, char c) {
  personal_chars[pos] = c == '_' ? '\0' : c;

  if (personal_chars[pos] == '\0') {
    for (byte i = pos + 1; i <= PERSONAL_CHARS_LEN; i++) personal_chars[i] = '\0';
  } else if (pos == PERSONAL_CHARS_LEN - 1) {
    personal_chars[PERSONAL_CHARS_LEN] = '\0';
  } else if (personal_chars[pos + 1] == '\0') {
    personal_chars[pos + 1] = '\0';
  }
}

// Editor con tres botones: UP/DOWN cambia caracter, SELECT avanza, SELECT largo sale.
static void edit_personal_chars() {
  const unsigned int SELECT_LARGO_MS = 600;
  byte pos = 0;
  bool done = false;
  bool redibujar = true;
  byte char_count = strlen(personal_editor_chars);

  while (readButtons());
  delay(150);

  do {
    if (redibujar) {
      lcd.setCursor(0, 0);
      lcd.print(F("Lista personal "));
      displayClearLine(1);
      lcd.print(pos + 1);
      lcd.print(F(":"));
      char c = personal_chars[pos];
      lcd.print(c == '\0' ? '_' : c);
      lcd.print(F(" "));
      byte usados = pos + 1 >= 10 ? 5 : 4;
      for (byte i = 0; personal_chars[i] != '\0' && usados < LCD_COLS; i++, usados++) {
        lcd.print(personal_chars[i]);
      }
      redibujar = false;
    }

    delay(120);
    byte buttons = readButtons();
    char current = personal_chars[pos];
    byte idx = personal_char_index(current);

    if (buttons == 1) {
      idx = (idx + 1) % char_count;
      set_personal_char(pos, personal_editor_chars[idx]);
      redibujar = true;
    } else if (buttons == 2) {
      idx = idx == 0 ? char_count - 1 : idx - 1;
      set_personal_char(pos, personal_editor_chars[idx]);
      redibujar = true;
    } else if (buttons == 3) {
      unsigned long inicioSelect = millis();
      while (readButtons() == 3);

      if (millis() - inicioSelect >= SELECT_LARGO_MS) {
        done = true;
      } else {
        if (pos < PERSONAL_CHARS_LEN - 1) pos++;
        else pos = 0;
        redibujar = true;
      }
    }
  } while (!done);

  if (personal_chars[0] == '\0') {
    personal_chars[0] = 'E';
    personal_chars[1] = '\0';
  }

  while (readButtons());
}

//====================
// Preferencias
//====================
// Preferencias: UP/DOWN navega, SELECT corto edita, SELECT largo guarda y sale.
void set_prefs() {
  const char* prf_names[] = {
    "Grupo de carac:",
    "Farnsworth ppm:",
    "Velocidad ppm:",
    "Tono audio:    ",
    "Set caracteres: ",
    "Lista personal:",
    "Nivel Koch:     ",
    "Koch saltar:    ",
    "Mostrar tras(s):",
    "Tipo llave:     "
  };

  // PREF_PERSONAL_LIST no es un byte de prefs[]: abre el editor de personal_chars.
  const byte pref_ids[] = {GROUP_NUM, GROUP_DLY, KEY_SPEED, PREF_TONE_PITCH, CHAR_SET, PREF_PERSONAL_LIST, KOCH_NUM, KOCH_SKIP, SHOW_DLY, KEYER_MODE};
  const byte lo_lim[] = {1, 0, 10, TONE_PITCH_MIN, 1, 0, 1, 0, 1, 0};
  const byte hi_lim[] = {15, 20, 30, TONE_PITCH_MAX, 6, 0, 40, 39, 10, 2};
  const byte n_prefs = sizeof(pref_ids) / sizeof(pref_ids[0]);
  const unsigned int SELECT_LARGO_MS = 600;

  byte pref = 0;
  byte pref_id;
  int p_val;
  byte buttons = 0;
  boolean done = false;
  boolean editando = false;
  boolean redibujar = true;

  while (readButtons());
  delay(200);

  lcd.clear();
  Serial.println(F("Preferencias"));

  auto valor_pref = [&](byte id) -> int {
    if (id == PREF_PERSONAL_LIST) return 0;
    if (id == PREF_TONE_PITCH) return tone_pitch;
    return prefs[id];
  };

  auto mostrar = [&]() {
    pref_id = pref_ids[pref];
    p_val = valor_pref(pref_id);

    displayClearLine(0);
    lcd.print(prf_names[pref]);

    displayClearLine(1);
    lcd.print(editando ? '>' : ' ');

    if (pref_id == CHAR_SET) {
      const char* set_names[] = {"", "Letras", "Numeros", "Puntuacion", "Todos", "Koch", "Personal"};
      lcd.print(set_names[p_val]);
    } else if (pref_id == PREF_PERSONAL_LIST) {
      lcd.print(personal_chars);
    } else if (pref_id == PREF_TONE_PITCH) {
      lcd.print(tone_frequency_hz());
      lcd.print(F(" Hz"));
    } else if (pref_id == GROUP_DLY) {
      if (p_val == 0) {
        lcd.print(F("Desactivado"));
      } else {
        lcd.print(p_val);
        lcd.print(F(" ppm"));
      }
    } else if (pref_id == KEYER_MODE) {
      const char* keyer_names[] = {"Recta", "Iambica A", "Iambica B"};
      lcd.print(keyer_names[p_val]);
    } else {
      lcd.print(F("Valor: "));
      lcd.print(p_val);
    }
  };

  do {
    if (redibujar) {
      mostrar();
      redibujar = false;
    }

    delay(120);
    buttons = readButtons();
    pref_id = pref_ids[pref];
    p_val = valor_pref(pref_id);

    if (buttons == 1) {
      if (editando && pref_id != PREF_PERSONAL_LIST) {
        if (pref_id == PREF_TONE_PITCH) {
          if (p_val < hi_lim[pref]) {
            tone_pitch = min((int)hi_lim[pref], p_val + TONE_PITCH_STEP);
            reproducir_muestra_tono();
            redibujar = true;
          }
        } else if (p_val < hi_lim[pref]) {
          prefs[pref_id] = p_val + 1;
          redibujar = true;
        }
      } else {
        if (pref < n_prefs - 1) pref++;
        else pref = 0;
        redibujar = true;
      }
    } else if (buttons == 2) {
      if (editando && pref_id != PREF_PERSONAL_LIST) {
        if (pref_id == PREF_TONE_PITCH) {
          if (p_val > lo_lim[pref]) {
            tone_pitch = max((int)lo_lim[pref], p_val - TONE_PITCH_STEP);
            reproducir_muestra_tono();
            redibujar = true;
          }
        } else if (p_val > lo_lim[pref]) {
          prefs[pref_id] = p_val - 1;
          redibujar = true;
        }
      } else {
        if (pref > 0) pref--;
        else pref = n_prefs - 1;
        redibujar = true;
      }
    } else if (buttons == 3) {
      unsigned long inicioSelect = millis();
      while (readButtons() == 3);

      if (millis() - inicioSelect >= SELECT_LARGO_MS) {
        done = true;
      } else if (pref_id == PREF_PERSONAL_LIST) {
        edit_personal_chars();
        editando = false;
        redibujar = true;
      } else {
        editando = !editando;
        redibujar = true;
      }
    }

  } while (!done);

  displayMessage("Guardando...    ", "");
  prefs_save();
  delay(1000);

  while (readButtons());
}

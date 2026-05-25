#include <LiquidCrystal_I2C.h>
#include "MorseEnDecoder.h"
#include <EEPROM.h>
#include "config.h"
#include "buttons.h"
#include "prefs.h"
#include "display.h"
#include "menu.h"
#include "cw_engine.h"
#include "trainer.h"
#include "keyer.h"

// Objetos Morse globales
MorseSpeaker Mspkr(beep_pin);
MorseDecoder morseInput(morseInPin, MORSE_KEYER, MORSE_ACTIVE_LOW, &Mspkr);
MorseEncoder morseOutput(cwOutPin, &Mspkr);
int Key_speed_adj = -2;

static void aplicar_preferencias_audio() {
  Mspkr.setOutputTonePitch(tone_frequency_hz());
  Mspkr.setSideTonePitch(tone_frequency_hz());
}

//====================
// Setup
//====================
void setup() {
  Serial.begin(9600);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(morseInPin, INPUT_PULLUP);
  keyer_init();

  display_init();
  lcd.setCursor(0, 0);
  lcd.print(F("=ManipulAR "));
  lcd.print(F(APP_VERSION));
  lcd.print(F("="));
  lcd.setCursor(0, 1);
  lcd.print(F("  Iniciando...  "));
  delay(2000);

  prefs_init();
  aplicar_preferencias_audio();
  Mspkr.sideToneOn = false;
  Mspkr.outputToneOn = false;

  // Identificacion al encender
  Mspkr.outputToneOn = true;
  cw_set_speed(20);
  char callsign[] = "MANIPULAR";
  for (int i = 0; i < 9; i++) {
    cw_write(callsign[i]);
    do {
      morseOutput.encode();
    } while (!morseOutput.available());
  }
  Mspkr.outputToneOn = false;
  noTone(beep_pin);

  Serial.println(F("ManipulAR listo"));
}

//====================
// Loop principal
//====================
void loop() {
  byte op_mode = get_mode();

  switch (op_mode) {
    case 1:
      morse_listen();
      break;
    case 2:
      morse_trainer();
      break;
    case 3:
      morse_decode();
      break;
    case 4:
      paris_test();
      break;
    case 5:
      radio_keyer();
      break;
    case 6:
      set_prefs();
      aplicar_preferencias_audio();
      break;
  }
}

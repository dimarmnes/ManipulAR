#include <Arduino.h>
#include "MorseEnDecoder.h"
#include <avr/pgmspace.h>

#include "keyer.h"
#include "config.h"
#include "prefs.h"

extern MorseSpeaker Mspkr;

// Este modulo toma una llave recta o paletas iambicas y genera una senal CW.
// En modo iambico tambien decodifica esa senal interna para Entrenar TX/Decodificador.
// KEY OUT hacia radio queda bloqueado salvo que radioOutputEnabled se active en Keyer radio.
namespace {
  enum KeyerState {
    KEYER_IDLE,
    KEYER_MARK,
    KEYER_SPACE
  };

  enum Element {
    ELEMENT_DIT,
    ELEMENT_DAH
  };

  // Maquina iambica:
  // IDLE espera paletas, MARK mantiene dit/dah, SPACE genera el espacio intra-caracter.
  // Los latches recuerdan una paleta presionada durante MARK para emitir el siguiente elemento.
  KeyerState state = KEYER_IDLE;
  Element currentElement = ELEMENT_DIT;
  Element lastElement = ELEMENT_DAH;
  unsigned long stateStart = 0;
  unsigned int ditMs = 60;
  bool ditLatch = false;
  bool dahLatch = false;
  bool keyDown = false;
  bool radioOutputEnabled = false;
  bool lastDecodeKeyDown = false;
  bool morseSpace = true;
  unsigned long markStart = 0;
  unsigned long spaceStart = 0;
  byte morseTablePointer = 0;
  char decodedChar = '\0';

  // Misma tabla binaria que usa MorseEnDecoder: izquierda=dit, derecha=dah.
  const char morseTable[] PROGMEM =
    " ETIANMSURWDKGOHVF*L*PJBXCYZQ!*54*3***2&*+****16=/***(*7***8*90*"
    "***********?_****\"**.****@***'**-********;!*)*****,****:*******\0";

  bool dit_pressed() {
    return digitalRead(morseInPin) == LOW;
  }

  bool dah_pressed() {
    return digitalRead(paddleDahPin) == LOW;
  }

  void set_key(bool down) {
    keyDown = down;
    digitalWrite(radioKeyPin, radioOutputEnabled && down ? HIGH : LOW);
    Mspkr.sideTone(down);
  }

  // En modo B se memoriza la paleta opuesta durante la marca.
  // Si el operador suelta ambas paletas antes del espacio, B conserva ese elemento extra;
  // A espera a ver que paletas siguen presionadas durante el espacio.
  void latch_opposite_paddle() {
    if (prefs[KEYER_MODE] != KEYER_IAMBIC_B) return;
    if (currentElement == ELEMENT_DIT && dah_pressed()) dahLatch = true;
    if (currentElement == ELEMENT_DAH && dit_pressed()) ditLatch = true;
  }

  void latch_pressed_paddles() {
    if (dit_pressed()) ditLatch = true;
    if (dah_pressed()) dahLatch = true;
  }

  Element next_element() {
    if (ditLatch && dahLatch) {
      return lastElement == ELEMENT_DIT ? ELEMENT_DAH : ELEMENT_DIT;
    }
    if (ditLatch) return ELEMENT_DIT;
    return ELEMENT_DAH;
  }

  void clear_latch(Element element) {
    if (element == ELEMENT_DIT) {
      ditLatch = false;
    } else {
      dahLatch = false;
    }
  }

  void start_element(Element element) {
    currentElement = element;
    clear_latch(element);
    set_key(true);
    state = KEYER_MARK;
    stateStart = millis();
  }

  unsigned int current_duration() {
    return currentElement == ELEMENT_DIT ? ditMs : ditMs * 3;
  }
}

void keyer_init() {
  pinMode(paddleDahPin, INPUT_PULLUP);
  pinMode(morseInPin, INPUT_PULLUP);
  pinMode(radioKeyPin, OUTPUT);
  keyer_stop();
}

void keyer_set_speed(byte speed) {
  if (speed < 1) speed = 1;
  ditMs = 1200 / speed;
}

void keyer_stop() {
  state = KEYER_IDLE;
  ditLatch = false;
  dahLatch = false;
  set_key(false);
}

void keyer_set_radio_output(boolean enabled) {
  radioOutputEnabled = enabled;
  if (!enabled) digitalWrite(radioKeyPin, LOW);
}

void keyer_decode_reset() {
  lastDecodeKeyDown = keyDown;
  markStart = millis();
  spaceStart = millis();
  morseTablePointer = 0;
  decodedChar = '\0';
  morseSpace = true;
}

// Decodifica la senal generada por el keyer iambico sin leer un pin fisico extra.
void keyer_decode_update() {
  unsigned long now = millis();

  if (keyDown != lastDecodeKeyDown) {
    if (keyDown) {
      markStart = now;
      morseSpace = false;
    } else {
      spaceStart = now;
      unsigned long markLen = spaceStart - markStart;
      if (markLen > ditMs / 4 && morseTablePointer < 63) {
        morseTablePointer *= 2;
        if (markLen < ditMs * 2) {
          morseTablePointer += 1;
        } else {
          morseTablePointer += 2;
        }
      }
    }
    lastDecodeKeyDown = keyDown;
  }

  if (!keyDown && morseTablePointer > 0 && decodedChar == '\0' && now - spaceStart >= ditMs * 2) {
    decodedChar = pgm_read_byte_near(morseTable + morseTablePointer);
    morseTablePointer = 0;
  }

  if (!keyDown && morseTablePointer == 0 && decodedChar == '\0' && !morseSpace && now - spaceStart > ditMs * 7) {
    decodedChar = ' ';
    morseSpace = true;
  }
}

boolean keyer_decode_available() {
  return decodedChar != '\0';
}

char keyer_decode_read() {
  char ch = decodedChar;
  decodedChar = '\0';
  return ch;
}

const char* keyer_mode_name(byte mode) {
  switch (mode) {
    case KEYER_IAMBIC_A:
      return "Iambica A";
    case KEYER_IAMBIC_B:
      return "Iambica B";
    case KEYER_STRAIGHT:
    default:
      return "Recta";
  }
}

// Maquina de estados no bloqueante: marca, espacio intra-caracter y reposo.
void keyer_update() {
  if (prefs[KEYER_MODE] == KEYER_STRAIGHT) {
    set_key(digitalRead(morseInPin) == LOW);
    return;
  }

  unsigned long now = millis();

  switch (state) {
    case KEYER_IDLE:
      latch_pressed_paddles();
      if (ditLatch || dahLatch) {
        start_element(next_element());
      }
      break;

    case KEYER_MARK:
      latch_opposite_paddle();
      if (now - stateStart >= current_duration()) {
        set_key(false);
        lastElement = currentElement;
        // En modo B, si ambas paletas siguen apretadas al terminar la marca,
        // se fuerza un elemento extra alternado antes de volver a reposo.
        if (prefs[KEYER_MODE] == KEYER_IAMBIC_B && dit_pressed() && dah_pressed()) {
          if (currentElement == ELEMENT_DIT) dahLatch = true;
          else ditLatch = true;
        }
        state = KEYER_SPACE;
        stateStart = now;
      }
      break;

    case KEYER_SPACE:
      if (now - stateStart >= ditMs) {
        if (ditLatch || dahLatch) {
          start_element(next_element());
        } else if (dit_pressed() || dah_pressed()) {
          latch_pressed_paddles();
          start_element(next_element());
        } else {
          state = KEYER_IDLE;
        }
      }
      break;
  }
}

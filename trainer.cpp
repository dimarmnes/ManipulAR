#include <Arduino.h>
#include "MorseEnDecoder.h"

#include "trainer.h"
#include "config.h"
#include "buttons.h"
#include "prefs.h"
#include "display.h"
#include "cw_engine.h"
#include "keyer.h"

extern MorseSpeaker Mspkr;
extern MorseDecoder morseInput;
extern MorseEncoder morseOutput;
extern int Key_speed_adj;

static void ajustar_velocidad();
static void ajustar_velocidad_entrenador(byte &rx_spd, byte &tx_spd);
static byte velocidad_ajustada(byte speed);
static void aplicar_velocidad_rx_cw(byte speed);
static void aplicar_velocidad_keyer(byte speed);
static void aplicar_velocidades_entrenador(byte rx_spd, byte tx_spd, boolean usar_keyer);
static void mostrar_linea_velocidad(byte speed);
static void cortar_sonido_cw();
static void mostrar_modo_decodificador();
static void esperar_encoder_disponible();
static void reproducir_error();
static void cargar_set_caracteres(char* ch_buf, byte &lo, byte &hi);

static const char koch[] = {'K','M','U','R','E','S','N','A','P','T',
                     'L','W','I','.','J','Z','=','F','O','Y',
                     ',','V','G','5','/','Q','9','2','H','3',
                     '8','B','?','4','7','C','1','D','6','0',
                     'X','\0'};
static const byte KOCH_CHAR_COUNT = sizeof(koch) - 1;

static const char alpha[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G',
                      'H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X',
                      'Y','Z',',','.','/','?','\0'};


// Carga el set elegido en un buffer comun para Entrenar RX y Entrenar TX.
// lo/hi delimitan el rango que random() puede usar dentro del buffer.
// El buffer siempre queda terminado en cero para soportar el set Personal.
static void cargar_set_caracteres(char* ch_buf, byte &lo, byte &hi) {
  switch (prefs[CHAR_SET]) {
    case 1:
      memcpy(ch_buf, alpha, sizeof(alpha));
      lo = 10; hi = 36;
      break;
    case 2:
      memcpy(ch_buf, alpha, sizeof(alpha));
      lo = 0; hi = 10;
      break;
    case 3:
      memcpy(ch_buf, alpha, sizeof(alpha));
      lo = 36; hi = 40;
      break;
    case 4:
      memcpy(ch_buf, alpha, sizeof(alpha));
      lo = 0; hi = 40;
      break;
    case 6: {
      byte len = strlen(personal_chars);
      if (len > 40) len = 40;
      memcpy(ch_buf, personal_chars, len);
      ch_buf[len] = '\0';
      lo = 0; hi = len;
      break;
    }
    case 5:
    default:
      memcpy(ch_buf, koch, sizeof(koch));
      lo = prefs[KOCH_SKIP];
      hi = prefs[KOCH_NUM] + 1;
      if (hi > KOCH_CHAR_COUNT) hi = KOCH_CHAR_COUNT;
      break;
  }

  if (hi <= lo) {
    ch_buf[0] = 'E';
    ch_buf[1] = '\0';
    lo = 0; hi = 1;
  }
}

//====================
// Modo Escucha: envia grupos y luego muestra la respuesta.
//====================
void morse_listen() {
  char ch_buf[42];
  char cw_tx[17];
  byte i;
  byte buttons = 0;
  byte lo, hi;
  boolean pausa = false;

  while (readButtons());
  delay(200);

  randomSeed(micros());

  Mspkr.sideToneOn = false;
  Mspkr.outputToneOn = true;
  cw_set_speed(velocidad_ajustada(prefs[KEY_SPEED]));

  cargar_set_caracteres(ch_buf, lo, hi);

  Serial.println(F("Modo escucha iniciado"));

  do {
    char line1[17];
    sprintf(line1, "Escucha %dppm", prefs[KEY_SPEED]);
    displayMessage(line1, "");

    pausa = false;
    buttons = 0;

    // Generar y enviar grupo
    for (i = 0; i < prefs[GROUP_NUM]; i++) {
      cw_tx[i] = ch_buf[random(lo, hi)];
      if (prefs[GROUP_DLY] > 0 && prefs[GROUP_DLY] < prefs[KEY_SPEED]) {
        int delayExtra = (1200 / prefs[GROUP_DLY] - 1200 / prefs[KEY_SPEED]) * 3;
        delay(delayExtra);
      }
      cw_write(cw_tx[i]);
      do {
        morseOutput.encode();
        buttons = readButtons();
        if (buttons == 1 || buttons == 2) {
          ajustar_velocidad();
          pausa = true;  // señal para reiniciar grupo
          buttons = 0;
          break;
        } else if (buttons) { break; }
      } while (!morseOutput.available());
      if (pausa || buttons) break;
    }

    if (buttons) break;  // SELECT sale al menú
    if (pausa) continue; // reinicia el do-while con nuevo grupo

    // Esperar antes de mostrar
    unsigned long espera = (unsigned long)prefs[SHOW_DLY] * 1000;
    unsigned long inicio = millis();
    while (millis() - inicio < espera) {
      buttons = readButtons();
      if (buttons == 1 || buttons == 2) {
        ajustar_velocidad();
        pausa = true;
        buttons = 0;
        break;
      } else if (buttons) { break; }
    }

    if (buttons) break;
    if (pausa) continue;

    // Mostrar respuesta
    lcd.setCursor(0, 1);
    for (i = 0; i < prefs[GROUP_NUM]; i++) {
      lcd.print(cw_tx[i]);
      Serial.print(cw_tx[i]);
    }
    Serial.println();

    delay(1500);
    if (readButtons()) buttons = 1;

  } while (!buttons);

  while (readButtons());
  Mspkr.outputToneOn = false;
  noTone(beep_pin);
}


//====================
// Entrenador Morse: envia un grupo y espera que el usuario lo copie con llave.
//====================
void morse_trainer() {
  char ch_buf[42];
  char cw_tx[17];
  char cw_rx;
  byte rx_cnt = 0;
  byte i;
  boolean error = false;
  boolean pausa = false;
  byte buttons = 0;
  byte lo, hi;
  byte aciertos = 0;
  byte errores = 0;
  byte tx_speed = prefs[KEY_SPEED];
  boolean usar_keyer = prefs[KEYER_MODE] != KEYER_STRAIGHT;

  while (readButtons());
  delay(200);

  randomSeed(micros());

  Mspkr.sideToneOn = true;
  Mspkr.outputToneOn = true;
  aplicar_velocidades_entrenador(prefs[KEY_SPEED], tx_speed, usar_keyer);
  if (usar_keyer) {
    keyer_set_radio_output(false);
    keyer_decode_reset();
  }

  cargar_set_caracteres(ch_buf, lo, hi);

  Serial.println(F("Entrenador iniciado"));
  displayMessage("Entrenar TX", keyer_mode_name(prefs[KEYER_MODE]));
  delay(1000);

  do {
    lcd.clear();
    rx_cnt = 0;
    boolean new_char = !error && !pausa;  // no genera nuevo si hubo error o pausa
    error = false;
    pausa = false;
    buttons = 0;

    lcd.setCursor(0, 0);
    for (i = 0; i < prefs[GROUP_NUM]; i++) {
      if (new_char) {
        cw_tx[i] = ch_buf[random(lo, hi)];
      }
      if (prefs[GROUP_DLY] > 0 && prefs[GROUP_DLY] < prefs[KEY_SPEED]) {
        int delayExtra = (1200 / prefs[GROUP_DLY] - 1200 / prefs[KEY_SPEED]) * 3;
        delay(delayExtra);
      }
      lcd.print(cw_tx[i]);
      cw_write(cw_tx[i]);
      do {
        morseOutput.encode();
        buttons = readButtons();
        if (buttons == 1 || buttons == 2) {
          ajustar_velocidad_entrenador(prefs[KEY_SPEED], tx_speed);
          aplicar_velocidades_entrenador(prefs[KEY_SPEED], tx_speed, usar_keyer);
          if (usar_keyer) {
            keyer_stop();
            keyer_decode_reset();
          }
          noTone(beep_pin);
          pausa = true;
          buttons = 0;
          break;
        } else if (buttons) { break; }
      } while (!morseOutput.available());
      if (pausa || buttons) break;
      Serial.print(cw_tx[i]);
    }

    if (buttons) break;
    if (pausa) {
      esperar_encoder_disponible();
      noTone(beep_pin);
      continue;
    }

    Serial.print(F(" > "));
    lcd.setCursor(0, 1);
    if (usar_keyer) {
      keyer_stop();
      keyer_decode_reset();
    }

    do {
      if (usar_keyer) {
        keyer_update();
        keyer_decode_update();
      } else {
        morseInput.decode();
      }
      if ((usar_keyer && keyer_decode_available()) || (!usar_keyer && morseInput.available())) {
        cw_rx = usar_keyer ? keyer_decode_read() : morseInput.read();
        if (cw_rx != ' ') {
          lcd.print(cw_rx);
          Serial.print(cw_rx);
          if (cw_rx != cw_tx[rx_cnt]) error = true;
          rx_cnt++;
        }
      }
      buttons = readButtons();
      if (buttons == 1 || buttons == 2) {
          ajustar_velocidad_entrenador(prefs[KEY_SPEED], tx_speed);
          aplicar_velocidades_entrenador(prefs[KEY_SPEED], tx_speed, usar_keyer);
          if (usar_keyer) {
            keyer_stop();
            keyer_decode_reset();
          }
        pausa = true;
        buttons = 0;
      }
    } while (rx_cnt < prefs[GROUP_NUM] && !error && !buttons && !pausa);

    if (buttons) break;
    if (pausa) {
      esperar_encoder_disponible();
      noTone(beep_pin);
      continue;
    }

    if (error) {
      errores++;
      Mspkr.outputToneOn = true;
      reproducir_error();
      Mspkr.outputToneOn = true;
      cw_set_speed(velocidad_ajustada(prefs[KEY_SPEED]));
      Serial.println(F(" ERROR"));
      lcd.setCursor(0, 1);
      lcd.print(F("*** ERROR ***   "));
    } else {
      aciertos++;
      Serial.println(F(" OK"));
    }
    delay(1500);

  } while (!buttons);

  cortar_sonido_cw();

  // Mostrar resultado final
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Resultado:      "));
  lcd.setCursor(0, 1);
  if (aciertos + errores > 0) {
    byte porcentaje = (aciertos * 100) / (aciertos + errores);
    lcd.print(porcentaje);
    lcd.print(F("% aciertos      "));
    Serial.print(F("Resultado: "));
    Serial.print(porcentaje);
    Serial.println(F("%"));
    if (porcentaje >= 90) {
      delay(1500);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(F("Excelente!!!    "));
      lcd.setCursor(0, 1);
      lcd.print(F("Sube de nivel!  "));
      Serial.println(F("Sube de nivel Koch!"));
    }
  } else {
    lcd.print(F("Sin datos       "));
  }
  delay(3000);

  while (readButtons());
  if (usar_keyer) keyer_stop();
  Mspkr.sideToneOn = false;
  Mspkr.outputToneOn = false;
  noTone(beep_pin);
}


//====================
// Decodificador Morse: muestra lo que se manipula con llave recta o iambica.
//====================
void morse_decode() {
  boolean usar_keyer = prefs[KEYER_MODE] != KEYER_STRAIGHT;

  noTone(beep_pin);
  delay(50);

  while (readButtons());
  delay(200);

  Mspkr.sideToneOn = true;
  Mspkr.outputToneOn = false;
  morseInput.setspeed(velocidad_ajustada(prefs[KEY_SPEED]));
  if (usar_keyer) {
    keyer_set_radio_output(false);
    aplicar_velocidad_keyer(prefs[KEY_SPEED]);
    keyer_stop();
    keyer_decode_reset();
  }

  Serial.println(F("Decodificador iniciado"));
  displayMessage("", "");
  mostrar_modo_decodificador();
  lcd.setCursor(0,1);

  byte cw_rx;
  byte buttons;
  byte ch_cnt = 0;

  do {
    if (usar_keyer) {
      keyer_update();
      keyer_decode_update();
    } else {
      morseInput.decode();
    }
    if ((usar_keyer && keyer_decode_available()) || (!usar_keyer && morseInput.available())) {
      cw_rx = usar_keyer ? keyer_decode_read() : morseInput.read();
      if (ch_cnt == 16) {
        displayClearLine(1);
        ch_cnt = 0;
      }
      Serial.print((char)cw_rx);
      lcd.print((char)cw_rx);
      ch_cnt++;
    }

    buttons = readButtons();
    if (buttons == 1 || buttons == 2) {
      ajustar_velocidad();
      if (usar_keyer) {
        aplicar_velocidad_keyer(prefs[KEY_SPEED]);
        keyer_stop();
        keyer_decode_reset();
      }
      // restaurar pantalla decodificador
      mostrar_modo_decodificador();
      displayClearLine(1);
      ch_cnt = 0;
      buttons = 0;
    }

  } while (!(buttons == 3));

  while (readButtons());
  if (usar_keyer) keyer_stop();
  Mspkr.sideToneOn = false;
  noTone(beep_pin);
}

//====================
// Test PARIS
//====================
void paris_test() {
  char cw_tx[] = "PARIS";
  boolean done = false;
  boolean pausa = false;
  byte buttons;

  while (readButtons());
  delay(200);

  Mspkr.sideToneOn = false;
  Mspkr.outputToneOn = true;
  cw_set_speed(velocidad_ajustada(prefs[KEY_SPEED]));

  displayMessage("Test PARIS", "");

  do {
    pausa = false;
    displayClearLine(1);

    for (int i = 0; i < 5; i++) {
      lcd.print(cw_tx[i]);
      cw_write(cw_tx[i]);
      do {
        morseOutput.encode();
        buttons = readButtons();
        if (buttons == 1 || buttons == 2) {
          ajustar_velocidad();
          esperar_encoder_disponible();
          noTone(beep_pin);
          lcd.setCursor(0, 0);
          lcd.print(F("Test PARIS      "));
          pausa = true;
          buttons = 0;
          break;
        } else if (buttons) { done = true; break; }
      } while (!morseOutput.available());
      if (pausa || done) break;
    }

    if (pausa) continue;
    if (done) break;

    delay(1000);
    buttons = readButtons();
    if (buttons == 1 || buttons == 2) {
      ajustar_velocidad();
      lcd.setCursor(0, 0);
      lcd.print(F("Test PARIS      "));
    } else if (buttons) done = true;

  } while (!done);

  while (readButtons());
  Mspkr.outputToneOn = false;
  noTone(beep_pin);
}


//====================
// Keyer radio: manipula KEY OUT sin sidetone local.
//====================
void radio_keyer() {
  byte buttons = 0;

  while (readButtons());
  delay(200);

  Mspkr.sideToneOn = false;
  Mspkr.outputToneOn = false;
  keyer_set_radio_output(true);
  aplicar_velocidad_keyer(prefs[KEY_SPEED]);
  keyer_stop();
  noTone(beep_pin);

  displayMessage("Keyer radio", keyer_mode_name(prefs[KEYER_MODE]));
  Serial.println(F("Keyer radio iniciado"));

  do {
    keyer_update();
    buttons = readButtons();

    if (buttons == 1 || buttons == 2) {
      ajustar_velocidad();
      aplicar_velocidad_keyer(prefs[KEY_SPEED]);
      keyer_stop();
      noTone(beep_pin);
      displayMessage("Keyer radio", keyer_mode_name(prefs[KEYER_MODE]));
      buttons = 0;
    }
  } while (buttons != 3);

  keyer_stop();
  keyer_set_radio_output(false);
  while (readButtons());
  noTone(beep_pin);
}


// La libreria y el keyer reciben la velocidad ya compensada por Key_speed_adj.
// En pantalla y EEPROM se conserva el valor elegido por el usuario.
static byte velocidad_ajustada(byte speed) {
  return speed + Key_speed_adj;
}

static void aplicar_velocidad_rx_cw(byte speed) {
  morseInput.setspeed(velocidad_ajustada(speed));
  cw_set_speed(velocidad_ajustada(speed));
}

static void aplicar_velocidad_keyer(byte speed) {
  keyer_set_speed(velocidad_ajustada(speed));
}

// Entrenar TX puede separar velocidad RX (lo que envia el equipo) y TX
// (lo que decodifica del usuario). En modos iambicos se actualiza tambien el keyer.
static void aplicar_velocidades_entrenador(byte rx_spd, byte tx_spd, boolean usar_keyer) {
  morseInput.setspeed(velocidad_ajustada(tx_spd));
  cw_set_speed(velocidad_ajustada(rx_spd));
  if (usar_keyer) aplicar_velocidad_keyer(tx_spd);
}

static void mostrar_modo_decodificador() {
  displayClearLine(0);
  lcd.print(F("Decod "));
  lcd.print(keyer_mode_name(prefs[KEYER_MODE]));
}

// Consume cualquier elemento pendiente del encoder antes de pausar o salir.
// Evita tonos largos colgados si el usuario interrumpe durante una transmision.
static void esperar_encoder_disponible() {
  while (!morseOutput.available()) {
    morseOutput.encode();
  }
}

static void reproducir_error() {
  for (byte i = 0; i < 3; i++) {
    tone(beep_pin, 200);
    delay(150);
    noTone(beep_pin);
    if (i < 2) delay(100);
  }
}

// Apaga todas las salidas CW conocidas. Se usa al salir de modos de practica.
static void cortar_sonido_cw() {
  digitalWrite(cwOutPin, LOW);
  digitalWrite(radioKeyPin, LOW);
  Mspkr.outputTone(false);
  Mspkr.sideTone(false);
  noTone(beep_pin);
}

static void mostrar_linea_velocidad(byte speed) {
  char line[17];
  sprintf(line, "Vel: %2dppm [SEL]", speed);
  lcd.setCursor(0, 1);
  lcd.print(line);
}

//====================
// Ajuste de velocidad en el aire para modos simples.
//====================
static void ajustar_velocidad() {
  noTone(beep_pin);

  displayMessage("** PAUSA **", "");
  mostrar_linea_velocidad(prefs[KEY_SPEED]);

  Serial.print(F("Pausa - Velocidad: "));
  Serial.println(prefs[KEY_SPEED]);

  // esperamos que suelten el botón que disparó la pausa
  while (readButtons());
  delay(100);

  byte btn;
  do {
    btn = readButtons();
    if (btn == 1 && prefs[KEY_SPEED] < KEY_SPEED_MAX) {
      prefs[KEY_SPEED]++;
      aplicar_velocidad_rx_cw(prefs[KEY_SPEED]);
      mostrar_linea_velocidad(prefs[KEY_SPEED]);
      delay(300);
    } else if (btn == 2 && prefs[KEY_SPEED] > KEY_SPEED_MIN) {
      prefs[KEY_SPEED]--;
      aplicar_velocidad_rx_cw(prefs[KEY_SPEED]);
      mostrar_linea_velocidad(prefs[KEY_SPEED]);
      delay(300);
    }
  } while (btn != 3);
  while (readButtons());
}

//====================
// Ajuste de velocidad del entrenador: RX controla lo enviado, TX controla lo copiado.
//====================
static void ajustar_velocidad_entrenador(byte &rx_spd, byte &tx_spd) {
  noTone(beep_pin);
  boolean ajustando_rx = true;  // empieza ajustando RX
  byte btn;
  unsigned long tiempoSelect = 0;

  while (readButtons());
  delay(100);

  auto mostrar = [&]() {
    lcd.setCursor(0, 0);
    lcd.print(F("** PAUSA **     "));
    lcd.setCursor(0, 1);
    if (ajustando_rx) {
      lcd.print(F("[RX:"));
      lcd.print(rx_spd);
      lcd.print(F("] TX:"));
      lcd.print(tx_spd);
      lcd.print(F("      "));
    } else {
      lcd.print(F(" RX:"));
      lcd.print(rx_spd);
      lcd.print(F(" [TX:"));
      lcd.print(tx_spd);
      lcd.print(F("]     "));
    }
  };

  mostrar();

  do {
    btn = readButtons();

    if (btn == 1) {  // UP
      if (ajustando_rx && rx_spd < KEY_SPEED_MAX) rx_spd++;
      else if (!ajustando_rx && tx_spd < rx_spd) tx_spd++;
      aplicar_velocidades_entrenador(rx_spd, tx_spd, false);
      mostrar();
      delay(300);
    } else if (btn == 2) {  // DOWN
      if (ajustando_rx && rx_spd > KEY_SPEED_MIN) rx_spd--;
      else if (!ajustando_rx && tx_spd > KEY_SPEED_MIN) tx_spd--;
      aplicar_velocidades_entrenador(rx_spd, tx_spd, false);
      mostrar();
      delay(300);
    } else if (btn == 3) {
      tiempoSelect = millis();
      while (readButtons() == 3);  // espera que suelten
      if (millis() - tiempoSelect < 500) {
        ajustando_rx = !ajustando_rx;  // SELECT corto, alterna RX/TX
        mostrar();
      } else {
        break;  // SELECT largo, sale
      }
    }
  } while (true);

  prefs[KEY_SPEED] = rx_spd;
  while (readButtons());
}

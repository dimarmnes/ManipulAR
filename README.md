# ManipulAR

**ManipulAR** es un entrenador de telegrafía CW para Arduino. Su función principal es ayudar a practicar recepción, transmisión y decodificación de código Morse usando un equipo físico, simple y autónomo.

Además del entrenamiento, ManipulAR puede funcionar como **keyer electrónico para llave iámbica**, con salida dedicada hacia una radio mediante una interfaz de keying adecuada.

El proyecto está pensado para radioaficionados, estudiantes de CW y constructores maker que quieran practicar sin depender de una PC, pero manteniendo la posibilidad de usar el mismo equipo como manipulador electrónico en operación real.

## Estado del proyecto

ManipulAR está en una primera versión funcional. El firmware ya puede usarse y probarse, pero la documentación de montaje, esquemas, fotos, capturas y guías de práctica se irán ampliando con el tiempo.

Los comentarios, pruebas y sugerencias son bienvenidos.

## Funciones principales

- Entrenamiento de recepción CW mediante grupos de caracteres.
- Entrenamiento de transmisión: el equipo envía un grupo y el usuario lo repite con la llave.
- Decodificador de lo manipulado por llave recta o paleta iámbica.
- Test PARIS como referencia de velocidad.
- Keyer electrónico para radio, con soporte para llave recta, iámbica A e iámbica B.
- Preferencias guardadas en EEPROM.
- Set de caracteres personalizado editable desde el propio equipo.
- Tono de audio ajustable desde el menú.
- Cambio de velocidad en el aire durante los modos de práctica.
- Separación entre salida interna de entrenamiento y salida `KEY OUT` hacia radio.

## Compilación e instalación

### Arduino IDE

1. Instalar Arduino IDE.
2. Instalar una biblioteca compatible con `LiquidCrystal_I2C`.
3. Abrir `ManipulAR.ino`.
4. Seleccionar una placa compatible, por ejemplo Arduino Uno o Arduino Nano.
5. Compilar y cargar el sketch.

El proyecto incluye una copia local modificada de `MorseEnDecoder`, por lo que no hace falta instalar esa biblioteca por separado.

### Librerías necesarias

- `EEPROM`, incluida con el entorno Arduino AVR.
- `LiquidCrystal_I2C`, instalable desde el Library Manager del Arduino IDE.

## Filosofía del proyecto

ManipulAR busca ser una herramienta práctica de aprendizaje. No intenta reemplazar plataformas como LCWO ni programas de entrenamiento avanzados, sino ofrecer una experiencia física: escuchar, manipular, equivocarse, repetir y progresar usando un dispositivo dedicado.

La prioridad es que el aprendizaje de CW sea llevadero, motivador y cercano a la práctica real de radioafición.

## Créditos e inspiración

ManipulAR está inspirado en el proyecto [`cw-trainer`](https://github.com/mfhughes128/cw-trainer) de Mike Hughes, KC1DMR, basado a su vez en el artículo **“Arduino CW Trainer”** de Tom Lewis, N4TL, publicado en QST en septiembre de 2016.

A partir de esa idea base —un entrenador CW con método Koch, generación de grupos aleatorios y verificación de la respuesta del operador— ManipulAR fue reorganizado, modularizado y ampliado para adaptarse a un hardware propio, una interfaz por menú, soporte de llave recta/iámbica y uso adicional como keyer electrónico para radio.

ManipulAR también incluye una copia local modificada de la biblioteca **MorseEnDecoder**, creada originalmente por raron y publicada bajo GNU GPLv3. Los encabezados originales de copyright y licencia se conservan en los archivos correspondientes.

Ver también:

- [`NOTICE`](NOTICE)
- [`LICENSE`](LICENSE)
- [`docs/licencias.md`](docs/licencias.md)

## Hardware básico

- Arduino Uno/Nano compatible.
- LCD 16x2 I2C.
- Tres pulsadores: Subir, Bajar y Seleccionar.
- Buzzer piezo o salida de audio para sidetone.
- Jack TRS para llave recta o paleta iámbica.
- Jack de salida hacia radio mediante interfaz de keying.

## Mapa de pines

| Pin Arduino | Función |
| --- | --- |
| D2 | DIT / llave recta |
| D3 | DAH |
| D4 | Botón Subir |
| D5 | Botón Bajar |
| D6 | Botón Seleccionar |
| D7 | KEY OUT hacia radio, solo en modo Keyer radio |
| D8 | Salida CW automática interna; no conectar a radio |
| D11 | Buzzer / sidetone interno |
| A4 | SDA LCD I2C |
| A5 | SCL LCD I2C |

## Menú principal

- Entrenar RX
- Entrenar TX
- Decodificador
- Test PARIS
- Keyer radio
- Preferencias

Subir/Bajar navegan por el menú. Seleccionar entra al modo elegido.

## Entrenamiento Koch y Farnsworth

El set `Koch` usa la secuencia progresiva de LCWO.net. El nivel 1 practica los dos primeros caracteres de la secuencia; cada nivel siguiente agrega un carácter nuevo, hasta el nivel 40.

La opción `Koch saltar` permite omitir caracteres iniciales de la secuencia activa. Puede servir para concentrarse en caracteres nuevos o difíciles dentro del nivel actual.

La separación `Farnsworth` permite mantener una velocidad de carácter alta mientras se aumenta el espacio entre caracteres. Si está en `0`, queda desactivada.

Para más contexto práctico, ver [`docs/practica-cw.md`](docs/practica-cw.md).

## Estructura del proyecto

```text
ManipulAR/
├── ManipulAR.ino          # Punto de entrada principal
├── config.h               # Pines, versión y constantes globales
├── display.cpp/.h         # Manejo de pantalla LCD
├── buttons.cpp/.h         # Lectura de pulsadores
├── menu.cpp/.h            # Menú principal y preferencias
├── prefs.cpp/.h           # EEPROM, valores por defecto y preferencias
├── trainer.cpp/.h         # Modos de entrenamiento y test PARIS
├── keyer.cpp/.h           # Llave recta, iámbica A/B y salida KEY OUT
├── cw_engine.cpp/.h       # Envoltorio del motor CW automático
├── MorseEnDecoder.cpp/.h  # Copia local modificada de MorseEnDecoder
├── pitches.h              # Tabla de notas/frecuencias
├── docs/                  # Documentación del proyecto
├── LICENSE                # Licencia GPLv3
└── NOTICE                 # Atribuciones principales
```

## Documentación

- [`docs/manual-usuario.md`](docs/manual-usuario.md): uso general del equipo.
- [`docs/hardware.md`](docs/hardware.md): conexiones, pines y recomendaciones de montaje.
- [`docs/practica-cw.md`](docs/practica-cw.md): explicación de Koch, Farnsworth y criterios de práctica.
- [`docs/faq.md`](docs/faq.md): preguntas frecuentes.
- [`docs/roadmap.md`](docs/roadmap.md): ideas y pendientes para próximas versiones.
- [`docs/arquitectura.md`](docs/arquitectura.md): organización interna del firmware.
- [`docs/licencias.md`](docs/licencias.md): créditos, origen del proyecto y consideraciones de licencia.
- [`CONTRIBUTING.md`](CONTRIBUTING.md): guía breve para reportar problemas o proponer cambios.

## Advertencia sobre conexión a radio

La salida `KEY OUT` está pensada para accionar una radio mediante una interfaz apropiada, preferentemente con optoacoplador. No se recomienda conectar directamente un pin del Arduino al equipo de radio.

Los modos de entrenamiento no activan la salida de radio. La salida `KEY OUT` solo se habilita en el modo **Keyer radio**.

## Versión

La versión actual se define en `config.h`:

```cpp
#define APP_VERSION "v1.0"
```

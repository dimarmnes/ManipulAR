# Licencias, créditos e inspiración

Este documento resume el origen del proyecto ManipulAR, las obras en las que se inspira y las precauciones tomadas para conservar atribuciones y licencias.

## Inspiración principal

ManipulAR está inspirado en el proyecto `cw-trainer` de Mike Hughes, KC1DMR:

- https://github.com/mfhughes128/cw-trainer

Ese proyecto se presenta como un entrenador de código Morse para Arduino y declara estar basado en el artículo **“Arduino CW Trainer”** de Tom Lewis, N4TL, publicado en QST en septiembre de 2016.

La idea pedagógica de base es entrenar CW mediante el método Koch: comenzar con pocos caracteres a una velocidad útil y sumar caracteres a medida que el operador mejora su precisión.

## Relación de ManipulAR con cw-trainer

ManipulAR toma esa idea como punto de partida conceptual: un entrenador autónomo de CW con generación de grupos, práctica auditiva y respuesta del operador mediante llave.

A partir de esa base, ManipulAR fue reorganizado y ampliado con objetivos propios:

- estructura modular por archivos;
- interfaz basada en LCD I2C y tres pulsadores;
- preferencias guardadas en EEPROM;
- set de caracteres personalizado editable desde el equipo;
- ajuste de tono de sidetone;
- soporte explícito para llave recta, iámbica A e iámbica B;
- separación entre entrenamiento interno y salida hacia radio;
- modo adicional de keyer electrónico para operar una radio.

## MorseEnDecoder

ManipulAR incluye una copia local modificada de `MorseEnDecoder`, creada originalmente por raron y publicada bajo GNU GPLv3.

Los archivos incluidos son:

- `MorseEnDecoder.cpp`
- `MorseEnDecoder.h`

La copia local conserva los avisos originales de copyright y licencia. La modificación local usada por ManipulAR permite cambiar la frecuencia del tono en tiempo de ejecución, de modo que el sidetone pueda configurarse desde el menú de preferencias sin depender de modificar una librería global instalada en el entorno Arduino.

## Licencia recomendada para ManipulAR

Dado que ManipulAR distribuye una copia modificada de una biblioteca bajo GNU GPLv3, la forma más simple y transparente de distribuir el proyecto completo es hacerlo bajo **GNU GPLv3**.

Por eso este repositorio incluye:

- `LICENSE`: texto completo de GNU GPLv3;
- `NOTICE`: resumen de atribuciones principales;
- encabezados originales conservados en los archivos derivados o incorporados.

## Buenas prácticas para futuras versiones

Al modificar o publicar nuevas versiones de ManipulAR:

- no eliminar encabezados de copyright o licencia de archivos heredados;
- mantener este documento actualizado;
- mencionar claramente la inspiración en `cw-trainer`, Mike Hughes KC1DMR y Tom Lewis N4TL;
- indicar cualquier modificación local hecha sobre bibliotecas de terceros;
- distribuir siempre el código fuente junto con los binarios o archivos compilados.

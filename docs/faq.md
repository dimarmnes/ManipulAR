# Preguntas frecuentes

## ¿ManipulAR reemplaza a LCWO u otros entrenadores?

No. ManipulAR está pensado como una herramienta física y autónoma para practicar escucha, transmisión y manipulación. Puede complementar plataformas de entrenamiento más completas.

## ¿Puedo conectar `KEY OUT` directo a una radio?

No es recomendable. La salida `KEY OUT` debe usarse mediante una interfaz adecuada, preferentemente con optoacoplador.

## ¿Los modos de práctica transmiten por la radio?

No. La salida hacia radio solo se habilita en el modo `Keyer radio`.

## ¿Qué placa Arduino se recomienda?

El proyecto está pensado para Arduino Uno o Nano compatibles. También puede funcionar en placas similares, siempre que respeten niveles lógicos y pines disponibles.

## ¿Qué biblioteca LCD necesito?

Una biblioteca compatible con `LiquidCrystal_I2C`. La dirección configurada por defecto es `0x27`, definida en `config.h`.

## ¿Por qué el nivel Koch 1 usa dos caracteres?

Porque con un solo carácter no hay discriminación auditiva real. El entrenamiento empieza con dos sonidos para que el oído tenga que reconocer cuál de los dos fue emitido.

En ManipulAR, el set Koch sigue la secuencia de LCWO.net, por lo que el nivel 1 empieza con `K` y `M`.

## ¿Qué hace `Koch saltar`?

Omite caracteres iniciales dentro del nivel Koch activo. Sirve para concentrarse en caracteres nuevos o difíciles.

## ¿Qué pasa si la EEPROM tiene valores inválidos?

Al iniciar, ManipulAR valida las preferencias guardadas. Si encuentra valores fuera de rango, los reemplaza por valores seguros.

## ¿Puedo cambiar los pines?

Sí. Los pines principales están definidos en `config.h`.

## ¿Por qué se incluye `MorseEnDecoder` dentro del proyecto?

Porque ManipulAR usa una copia local modificada para poder ajustar el tono de audio en tiempo de ejecución desde el menú de preferencias.

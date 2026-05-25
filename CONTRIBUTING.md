# Contribuir a ManipulAR

Gracias por probar ManipulAR.

El proyecto está en una primera versión pública, así que el feedback más valioso por ahora es práctico:

- si compila o no en tu entorno;
- qué placa Arduino usaste;
- qué biblioteca `LiquidCrystal_I2C` instalaste;
- problemas de montaje o cableado;
- confusiones en el menú o el manual;
- ideas para mejorar la práctica de CW.

## Reportar problemas

Al abrir un issue, incluir en lo posible:

- placa usada;
- versión del Arduino IDE o herramienta de compilación;
- biblioteca LCD instalada;
- modo de ManipulAR donde aparece el problema;
- pasos para reproducirlo;
- comportamiento esperado y comportamiento observado.

## Proponer cambios

Para cambios de firmware, conviene mantener el estilo actual:

- módulos pequeños por responsabilidad;
- evitar dependencias nuevas si no son necesarias;
- cuidar que los modos de práctica no activen `KEY OUT`;
- mantener actualizada la documentación cuando cambia una función visible.

## Licencia

Las contribuciones se integran bajo la misma licencia del proyecto: GNU GPLv3.

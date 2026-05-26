# Hardware de ManipulAR

Este documento resume las conexiones principales y recomendaciones de montaje.

## Componentes básicos

- Arduino Uno/Nano compatible.
- LCD 16x2 con interfaz I2C.
- Tres pulsadores para navegación.
- Buzzer piezo o salida de audio para sidetone.
- Jack TRS para llave recta o paleta iámbica.
- Jack TS para salida hacia radio.
- Optoacoplador recomendado para aislar la radio.

## Mapa de pines

| Pin Arduino | Función |
| --- | --- |
| D2 | DIT / llave recta |
| D3 | DAH |
| D4 | Botón Subir |
| D5 | Botón Bajar |
| D6 | Botón Seleccionar |
| D7 | KEY OUT hacia radio, solo en modo Keyer radio |
| D8 | Salida CW automática interna, no conectar a radio |
| D11 | Buzzer / sidetone interno |
| A4 | SDA LCD I2C |
| A5 | SCL LCD I2C |

## Jack de llave o paleta

Usar un jack TRS:

| Contacto | Función |
| --- | --- |
| Tip | DIT o llave recta |
| Ring | DAH |
| Sleeve | GND |

Con una llave recta mono/TS, el tip queda como entrada de llave recta y sleeve como GND.

Con una paleta iámbica TRS, el tip corresponde a DIT y el ring a DAH.

## Salida hacia radio

La salida `KEY OUT` está en D7 y solo se activa en modo `Keyer radio`.

Se recomienda usar optoacoplador, por ejemplo PC817, 4N25, 4N35 o equivalente.

### Conexión sugerida del lado Arduino

```text
D7 / radioKeyPin -> resistencia 270 ohm a 390 ohm -> ánodo LED opto
GND Arduino -----------------------------------> cátodo LED opto
```

### Conexión sugerida del lado radio

```text
Tip jack KEY radio ----> colector opto
Sleeve/GND radio ------> emisor opto
```

## Buzzer interno

Conectar el buzzer con una resistencia serie:

```text
D11 / beep_pin -> resistencia 220 ohm -> buzzer -> GND
```

## Salida de audio externa

Si se agrega jack de audio externo, conviene usar un jack con contacto conmutado para desconectar el buzzer interno al insertar un plug.

```text
D11 / beep_pin -> resistencia serie 1k a 4.7k -> Tip del jack audio
GND -------------------------------------------> Sleeve del jack audio
Contacto conmutado del jack -------------------> buzzer interno
```

La salida externa está pensada para parlante amplificado o etapa de audio. No conectar auriculares de baja impedancia directamente al pin del Arduino.

## Recomendaciones de montaje

- Mantener D0/D1 libres para Serial USB.
- Mantener A4/A5 para el LCD I2C.
- Usar masas comunes en las entradas de llave, botones y audio.
- Revisar alimentación si el buzzer provoca reinicios o ruido.
- Probar la salida hacia radio con cuidado y preferentemente con carga fantasma.
- Confirmar que la radio solo se manipule en modo Keyer radio.

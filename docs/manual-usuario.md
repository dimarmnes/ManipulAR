# Manual de usuario de ManipulAR

ManipulAR es principalmente un entrenador de telegrafía CW. También puede funcionar como keyer electrónico para llave iámbica.

## Controles

ManipulAR usa tres botones:

| Botón | Función general |
| --- | --- |
| UP | Subir, aumentar valor o navegar hacia arriba |
| DOWN | Bajar, disminuir valor o navegar hacia abajo |
| SELECT | Entrar, confirmar o salir según el contexto |

En el menú principal, UP/DOWN cambian de opción y SELECT entra al modo seleccionado.

## Modos principales

### Entrenar RX

Modo orientado a recepción. El equipo envía grupos de caracteres en CW para que el usuario practique la escucha.

Es el modo más importante para construir reconocimiento auditivo. La idea es escuchar los caracteres como sonidos completos, no como una suma mental de puntos y rayas.

### Entrenar TX

El equipo envía un grupo y luego el usuario intenta repetirlo usando la llave recta o la paleta iámbica configurada.

Este modo combina escucha, memoria auditiva y transmisión. Sirve para practicar la coordinación entre lo escuchado y lo manipulado.

### Decodificador

Muestra en la pantalla lo que el usuario manipula con la llave.

Puede usarse para revisar el ritmo propio, probar una llave o familiarizarse con la respuesta del keyer.

### Test PARIS

Envía la palabra PARIS como referencia clásica para comparar o verificar velocidad de CW.

### Keyer radio

Permite usar ManipulAR como keyer electrónico conectado a una radio mediante la salida `KEY OUT`.

En este modo:

- la salida activa hacia radio es D7;
- SELECT vuelve al menú principal;
- UP/DOWN ajustan la velocidad del keyer;
- no se genera sidetone local, porque se espera usar el monitor de la propia radio.

## Preferencias

Desde el menú Preferencias se pueden configurar los valores principales del equipo.

Opciones disponibles:

- cantidad de caracteres por grupo;
- separación Farnsworth;
- velocidad;
- set de caracteres;
- nivel Koch;
- salto Koch;
- tiempo antes de mostrar la respuesta;
- tipo de llave;
- tono de audio/sidetone;
- lista personal de caracteres.

### Nivel Koch y Koch saltar

El set `Koch` presenta caracteres de forma progresiva usando la secuencia de LCWO.net.

En ManipulAR, el nivel 1 usa los dos primeros caracteres de la secuencia Koch. Cada nivel siguiente agrega un carácter más, hasta llegar al nivel 40.

`Koch saltar` permite omitir caracteres iniciales dentro del nivel actual. Por ejemplo, si un nivel ya incluye varios caracteres y se quiere reforzar la parte nueva de la secuencia, se puede aumentar este valor para practicar solo los caracteres posteriores.

Si `Koch saltar` queda fuera de rango respecto del nivel elegido, el firmware lo corrige al cargar preferencias.

### Separación Farnsworth

La separación Farnsworth aumenta el espacio entre caracteres sin bajar la velocidad interna de cada carácter.

Esto permite escuchar cada letra o número a una velocidad realista, pero con más tiempo para reconocerlo antes del siguiente carácter.

Si la opción está en `0`, la separación Farnsworth queda desactivada.

Dentro de Preferencias:

- UP/DOWN navegan entre opciones;
- SELECT corto entra o sale de edición;
- en edición, UP/DOWN cambian el valor;
- SELECT largo guarda y sale.

## Tipos de llave

ManipulAR puede trabajar con tres modos de entrada:

| Modo | Uso |
| --- | --- |
| Recta | Llave vertical o contacto simple en D2 |
| Iámbica A | Paleta iámbica con comportamiento modo A |
| Iámbica B | Paleta iámbica con comportamiento modo B |

La selección se realiza desde Preferencias.

## Set Personal

El set `Personal` permite practicar una lista propia de hasta 16 caracteres.

Caracteres disponibles:

- A-Z
- 0-9
- . , / ? =

El carácter `_` marca el final de la lista. Si la lista queda vacía, ManipulAR usa `E` como valor mínimo.

## Uso recomendado para practicar

Una forma simple de usar ManipulAR es:

1. empezar con pocos caracteres;
2. elegir una velocidad cómoda pero realista;
3. practicar recepción antes de preocuparse por transmitir rápido;
4. usar Entrenar TX para reforzar memoria auditiva y coordinación;
5. usar Decodificador solo como apoyo, no como reemplazo de la escucha;
6. pasar a Keyer radio recién cuando la manipulación sea cómoda y estable.

Para más detalles sobre método Koch, separación Farnsworth y criterios de avance, ver [`practica-cw.md`](practica-cw.md).

## Seguridad al usar radio

No conectar directamente un pin del Arduino al jack de key de la radio. Usar una interfaz adecuada, preferentemente con optoacoplador.

Los modos de entrenamiento no activan la salida de radio. La salida hacia radio solo se habilita en el modo Keyer radio.

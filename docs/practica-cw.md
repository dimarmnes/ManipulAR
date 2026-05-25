# Práctica de CW con ManipulAR

Este documento reúne criterios prácticos para usar ManipulAR como entrenador de código Morse.

## Método Koch

El método Koch propone practicar pocos caracteres al comienzo, pero escucharlos a una velocidad útil desde el primer día. La idea es reconocer cada carácter por su sonido completo, no contar puntos y rayas.

Cuando el reconocimiento mejora, se agrega un carácter nuevo. Así el entrenamiento mantiene una dificultad manejable sin bajar demasiado la velocidad de los caracteres.

## Niveles Koch en ManipulAR

ManipulAR usa la secuencia Koch de LCWO.net, con 41 caracteres:

```text
K M U R E S N A P T L W I . J Z = F O Y , V G 5 / Q 9 2 H 3 8 B ? 4 7 C 1 D 6 0 X
```

Los niveles configurables van de 1 a 40.

El nivel 1 habilita los dos primeros caracteres:

```text
K M
```

Cada nivel siguiente agrega un carácter más. Por ejemplo:

| Nivel | Caracteres habilitados |
| --- | --- |
| 1 | K M |
| 2 | K M U |
| 3 | K M U R |
| 10 | K M U R E S N A P T L |
| 40 | Secuencia completa |

## Koch saltar

`Koch saltar` omite caracteres iniciales dentro del nivel activo.

Ejemplo: si el nivel activo habilita:

```text
K M U R E
```

y `Koch saltar` está en `2`, la práctica usa:

```text
U R E
```

Esto puede servir para concentrarse en caracteres nuevos o difíciles, pero no conviene abusar: también es importante mezclar los caracteres recientes con los anteriores.

## Farnsworth

La práctica Farnsworth mantiene rápida la velocidad interna de cada carácter, pero agrega más separación entre caracteres.

Esto ayuda a evitar el hábito de contar puntos y rayas. El oído aprende la forma sonora de cada carácter a una velocidad realista, mientras la separación extra da más tiempo para reconocerlo.

En ManipulAR, la opción `Farnsworth ppm` controla esa separación. Si está en `0`, queda desactivada.

## Criterio de avance

Una regla simple:

- subir de nivel cuando la recepción sea cómoda y estable;
- repetir el nivel si aparecen confusiones frecuentes;
- usar `Koch saltar` solo para reforzar caracteres concretos;
- mantener una velocidad de carácter que suene como CW real, aunque haya separación extra.

No hace falta apurarse. La regularidad suele rendir más que sesiones largas y frustrantes.

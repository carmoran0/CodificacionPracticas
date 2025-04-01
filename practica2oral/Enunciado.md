# Sistema de control de climatización

## Hardware
- Sensor de temperatura: LM35
- LEDs:
  - Verde
  - Rojo
  - Azul
- Opcional: Display de 4 dígitos

## Funcionalidades del sistema

### Menú principal (Terminal serie)
1. Fijar temperatura del termostato
2. Fijar histéresis del termostato
3. Mostrar temperatura actual
4. Mostrar histéresis actual

### Modo de operación
Una vez configurados los parámetros iniciales (temperatura e histéresis):

- **LED Verde**: Se enciende cuando:
  ```
  Tp + H > T > Tp - H
  ```
  (T = Temperatura medida, Tp = Temperatura programada, H = Histéresis)

- **LED Azul**: Se enciende cuando:
  - T > Tp + H
  - Permanece encendido hasta T < Tp

- **LED Rojo**: Se enciende cuando:
  - T < Tp - H
  - Permanece encendido hasta T > Tp

## Mejoras

### Primera mejora
- Almacenamiento en EPROM de parámetros configurados
- Sistema autónomo después de primera configuración

### Segunda mejora
- Display muestra:
  - Temperatura actual permanentemente
  - Cada 5 segundos muestra "P + temperatura programada"

### Mejora para matrícula
- Control remoto desde móvil
- Visualización y modificación de parámetros desde dispositivo móvil





Diseñar una controladora para un sistema de climatización.

Hardware: sensor de temperatura LM35, tres diodos led uno verde, otro rojo y el último azul. Opcional para mejora display de 4 dígitos

El sistema básico mostrará en la terminal serie un menú donde aparecerán las siguientes opciones

1- Fijar la temperatura del termostato

2- Fijar la histéresis del termostato

3- Mostar la temperatura actual

4- Mostrar la histéresis actual

Una vez configurado el sistema, o sea después de haber introducido la temperatura e histéresis por primera vez se comenzará a realizar el control de temperatura.

Si la temperatura que mide el sensor T está situada entre la fijada por el termostato Tp +o- la histéresis H, se encenderá el led verde -> Tp+H>T>Tp-H

Si la temperatura supera Tp+H se encenderá el led azul, (apagará el verde) y no se apagará hasta que la temperatura baje por debajo de Tp, momento en que se encenderá el led verde

Si la temperatura  baja por debajo de Tp-H se encenderá el led rojo (apagará el verde) y no se apagará hasta que la temperatura suba por encima de Tp, momento en que se encenderá el led verde

En cualquier momento se puede reprogramar la temperatura, histéresis o consultar los parámetros del sistema.

Con las condiciones establecidas se optará a una nota de 6.

Primera mejora: las temperaturas programadas se guardan en la memoria EPROM y una vez que se programa la primera vez el sistema es autónomo.

Segunda mejora: la temperatura actual se muestra en el display y una vez cada 5" se muestra una P y la temperatura programada

Mejora para matrícula: hacer que el sistema se controle desde el teléfono móvil, mostrar temperatura, parámetros y poder cambiar los parámetros.
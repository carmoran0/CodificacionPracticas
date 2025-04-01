# Control PID para acelerador de vehículo

## Configuración del sistema
- **Hardware:**
  - Potenciómetro (lectura pedal: 0 = ralentí, 1024 = máximo)
  - Servo para mariposa del carburador
  - Pulsador para modo boxes
  - LED rojo indicador

## Funcionamiento principal
1. **Control PID:**
   ```
   Potenciómetro → PID → Servo
   ```
   - Regula diferencia entre lectura del pedal y posición real del servo
   - Mitiga lecturas erróneas del potenciómetro

2. **Modo boxes:**
   - Pulsador:
     1. 1 pulsación: Limita servo al 25% máximo
     2. 2 pulsaciones: Libera límite
   - LED rojo indica activación del modo

## Menú de control
1. Ajustar parámetro P del PID
2. Ajustar parámetro I del PID
3. Ajustar parámetro D del PID
4. Mostrar ángulo actual del servo


En el siguiente ejercicio vamos a realizar el control del pedal del acelerador de un vehículo de competición.

Para ello situaremos un potenciómetro que será el que realice la lectura del pedal, donde 0 significará que vamos a ralentí y 1024 que vamos a todo gas.

El acelerador controla la mariposa de paso de la mezcla aire - gasolina al motor, usaremos el servo del kit para controlar la mariposa del carburador de nuestro vehículo.

El motor no pueda cambiar de régimen a la misma velocidad que nosotros pisamos el acelerador, y en algunos casos la lectura del potenciómetro puede ser errónea y eso afectaría al motor. Por ese motivo vamos a introducir un controlador PID entre la lectura del potenciómetro y la acción del servo.

Por otra parte, cada vez que el vehículo entra en boxes tenemos que limitar la potencia del motor a un valor máximo, que hará que aunque pisemos el acelerador a tope, el servo no pueda pasar de un 25%  de su valor máximo. Esta acción está controlada con un pulsador, que al pulsar una vez limita la potencia del motor y al pulsar dos veces libera el límite establecido. Un led rojo marcará que estamos en modo boxes.

El programa tendrá un menú con tres opciones que permitirán cambiar cada uno de los 3 parámetros del PID, y una cuarta opción para mostrar el ángulo del servo.
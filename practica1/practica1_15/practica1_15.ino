/*
15. Torres de Hanoi. El juego, en su forma más tradicional, consiste en tres postes
verticales. En uno de los postes se apila un número indeterminado de discos perforados
por su centro (elaborados de madera), que determinará la complejidad de la solución.
Por regla general se consideran siete discos. Los discos se apilan sobre uno de los
postes en tamaño decreciente de abajo arriba. No hay dos discos iguales, y todos ellos
están apilados de mayor a menor radio -desde la base del poste hacia arriba- en uno
de los postes, quedando los otros dos postes vacíos. El juego consiste en pasar todos
los discos desde el poste ocupado (es decir, el que posee la torre) al tercer poste
pudiendo usarse el poste central como ayuda.
El programa comenzará solicitando el número de discos que se van a usar y comenzará
a indicar los movimientos que se producen entre los postes. Los discos se numeran
desde 1 el de radio menor a n el de radio mayor.
*/

#define MAXBUF 20
char buffer[MAXBUF];
boolean myReadString(int timeout);

// Función para resolver las Torres de Hanoi mediante recursión
void hanoi(int n, char origen, char auxiliar, char destino) {
  if (n == 1) {
    // Caso base: mover un disco directamente del origen al destino
    Serial.print("Mover disco 1 desde poste ");
    Serial.print(origen);
    Serial.print(" hasta poste ");
    Serial.println(destino);
    return;
  }
  
  // Mover n-1 discos desde origen a auxiliar usando destino como auxiliar
  hanoi(n-1, origen, destino, auxiliar);
  
  // Mover el disco n desde origen a destino
  Serial.print("Mover disco ");
  Serial.print(n);
  Serial.print(" desde poste ");
  Serial.print(origen);
  Serial.print(" hasta poste ");
  Serial.println(destino);
  
  // Mover n-1 discos desde auxiliar a destino usando origen como auxiliar
  hanoi(n-1, auxiliar, origen, destino);
}

void setup() {
  delay(700);
  Serial.begin(9600);
  delay(700);
  Serial.println("Torres de Hanoi");
  Serial.println("Introduce el numero de discos: ");
}

void loop() {
  if (myReadString(50)) {
    int numDiscos = atoi(buffer);
    
    if (numDiscos > 0) {
      Serial.print("Resolviendo Torres de Hanoi con ");
      Serial.print(numDiscos);
      Serial.println(" discos:");
      
      // Resolver Torres de Hanoi
      // Origen: 'A', Auxiliar: 'B', Destino: 'C'
      hanoi(numDiscos, 'A', 'B', 'C');
      
      // Mostrar el número total de movimientos
      int totalMovimientos = pow(2, numDiscos) - 1;
      Serial.print("Total de movimientos: ");
      Serial.println(totalMovimientos);
      
      // Solicitar nuevo número de discos
      Serial.println("\nIntroduce el numero de discos: ");
    } else {
      Serial.println("Por favor, introduce un numero positivo de discos.");
    }
  }
}

// Función para leer una cadena de la entrada serial con límite de tiempo
boolean myReadString(int timeout) { // ReadString with timeout limit, return true if a string is readed within timeout limits 
  static int i=0; // Número de caracteres leidos
  static long tLimit; // Reloj interno de timeout
  static boolean reading=false; // Marca si hay una lectura en curso
  while((Serial.available()>0) && (i<(MAXBUF-1))){ // Hay caracteres disponibles y no hemos desbordado el buffer
    if(!reading){ // First character
      reading=true; // Estamos en un proceso de lectura
      tLimit = millis(); // Ajustamos el reloj de timeout
      i=0; // Iniciamos el índice del buffer
    }
    buffer[i]=Serial.read(); // read next caracter from buffer
    if((buffer[i]=='\n') || (buffer[i]=='\r')){ // Fin de mensaje
      reading = false; // Fin de lectura
      if(i>0){ // No se trata de una linea vacía
        buffer[i] = 0; // Pon la marca de fin de cadena
        return true; // Lectura exitosa
      }
    }
    i=i+1;
    if(millis()-tLimit > timeout){ // Superamos el timeout de la lectura 86uS por caracter a 115200
      Serial.print("TimeOut"); // Mensaje de error
      reading = false; // Fin de lectura
      return false; // Fallo en la lectura
    }
  }
  return false;
}
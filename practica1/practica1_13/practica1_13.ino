/*
13. implemente una función que se encargue de ordenar los elementos de un vector
numérico. void ordena(int v[]), a la función se le pasa el vector desordenado y lo
devuelve ordenado de mayor a menor. El algoritmo tradicional para la ordenación es el
de la burbuja https://es.wikipedia.org/wiki/Ordenamiento_de_burbuja
*/

#define MAXBUF 20
char buffer[MAXBUF];
boolean myReadString(int timeout);

// Función para ordenar un vector de enteros de mayor a menor usando el algoritmo de burbuja
void ordena(int v[], int longitud) {
  // Implementación del algoritmo de burbuja para ordenación descendente
  for (int i = 0; i < longitud - 1; i++) {
    for (int j = 0; j < longitud - i - 1; j++) {
      // Comparamos elementos adyacentes y los intercambiamos si están en el orden incorrecto
      // Para orden descendente, intercambiamos si el elemento actual es menor que el siguiente
      if (v[j] < v[j + 1]) {
        // Intercambio de elementos
        int temp = v[j];
        v[j] = v[j + 1];
        v[j + 1] = temp;
      }
    }
  }
}

void setup() {
  delay(700);
  Serial.begin(9600);
  delay(700);
  Serial.println("Programa de ordenación de vectores");
  Serial.println("Introduce números separados por espacios:");
}

void loop() {
  int v[MAXBUF]; // Vector para almacenar los números
  
  if (myReadString(50)) {
    char *p;
    int j = 0;

    p = strtok(buffer, " ");
    while ((p != NULL) && (j < MAXBUF)) {
      v[j++] = atoi(p); // atoi = ASCII to Integral 
      p = strtok(NULL, " "); // Buscar el siguiente valor
    }
    
    if (j > 0) { // Si se encontraron números
      Serial.print("Vector original: ");
      for (int i = 0; i < j; i++) {
        Serial.print(v[i]);
        Serial.print(" ");
      }
      Serial.println();
      
      // Ordenar el vector
      ordena(v, j);
      
      Serial.print("Vector ordenado (mayor a menor): ");
      for (int i = 0; i < j; i++) {
        Serial.print(v[i]);
        Serial.print(" ");
      }
      Serial.println();
      
      Serial.println("\nIntroduce más números para ordenar:");
    }
  }
}

// Función para leer una cadena con límite de tiempo
boolean myReadString(int timeout) { // ReadString with timeout limit, return true if a string is read within timeout limits 
  static int i = 0; // Número de caracteres leídos
  static long tLimit; // Reloj interno de timeout
  static boolean reading = false; // Marca si hay una lectura en curso
  
  while ((Serial.available() > 0) && (i < (MAXBUF - 1))) { // Hay caracteres disponibles y no hemos desbordado el buffer
    if (!reading) { // First character
      reading = true; // Estamos en un proceso de lectura
      tLimit = millis(); // Ajustamos el reloj de timeout
      i = 0; // Iniciamos el índice del buffer
    }
    
    buffer[i] = Serial.read(); // read next character from buffer
    if ((buffer[i] == '\n') || (buffer[i] == '\r')) { // Fin de mensaje
      reading = false; // Fin de lectura
      if (i > 0) { // No se trata de una línea vacía
        buffer[i] = 0; // Pon la marca de fin de cadena
        return true; // Lectura exitosa
      }
    }
    
    i = i + 1;
    if (millis() - tLimit > timeout) { // Superamos el timeout de la lectura
      Serial.println("TimeOut"); // Mensaje de error
      reading = false; // Fin de lectura
      return false; // Fallo en la lectura
    }
  }
  
  return false;
}
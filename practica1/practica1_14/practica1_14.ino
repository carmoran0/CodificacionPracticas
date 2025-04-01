/*
14. El algoritmo de la burbuja no es muy óptimo, repita el ejercicio anterior usando el
algoritmo QuickSort https://es.wikipedia.org/wiki/Quicksort
*/

#define MAXBUF 20
char buffer[MAXBUF];
boolean myReadString(int timeout);

// Función para intercambiar dos elementos
void swap(int* a, int* b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

// Función para encontrar la partición
int partition(int arr[], int low, int high) {
  // Seleccionamos el último elemento como pivote
  int pivot = arr[high];
  // Índice del elemento más pequeño
  int i = (low - 1);
  
  for (int j = low; j <= high - 1; j++) {
    // Si el elemento actual es mayor que el pivote (para orden descendente)
    if (arr[j] > pivot) {
      i++;
      swap(&arr[i], &arr[j]);
    }
  }
  swap(&arr[i + 1], &arr[high]);
  return (i + 1);
}

// Función principal de QuickSort
void quickSort(int arr[], int low, int high) {
  if (low < high) {
    // pi es el índice de partición
    int pi = partition(arr, low, high);
    
    // Ordenar elementos antes y después de la partición
    quickSort(arr, low, pi - 1);
    quickSort(arr, pi + 1, high);
  }
}

// Función para ordenar un vector usando QuickSort
void ordena(int v[], int longitud) {
  if (longitud > 1) {
    quickSort(v, 0, longitud - 1);
  }
}

void setup() {
  delay(700);
  Serial.begin(9600);
  delay(700);
  Serial.println("Programa de ordenación de vectores usando QuickSort");
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
      
      // Ordenar el vector usando QuickSort
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
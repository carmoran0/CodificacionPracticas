/*
 * Práctica 2.1 - Simulación de un canal ruidoso
 * 
 * Esta función simula un canal ruidoso para comunicaciones binarias.
 * Recibe un vector binario de entrada, una longitud L en bytes, y una probabilidad de error f.
 * Devuelve un vector de salida donde cada bit tiene una probabilidad f de haber cambiado su valor.
 */

/**
 * Función que simula un canal ruidoso.
 * @param in Vector binario de entrada empaquetado en unsigned char
 * @param out Vector binario de salida empaquetado en unsigned char
 * @param l Longitud del vector en bytes
 * @param f Probabilidad de que un bit cambie de valor (entre 0 y 1)
 */
void noisyChannel(unsigned char *in, unsigned char *out, int l, float f) {
  // Inicializar la semilla para la generación de números aleatorios
  // En una implementación real, esto debería hacerse solo una vez al inicio del programa
  randomSeed(analogRead(0));
  
  // Recorrer cada byte del vector de entrada
  for (int i = 0; i < l; i++) {
    // Inicializar el byte de salida con el valor del byte de entrada
    out[i] = in[i];
    
    // Procesar cada bit del byte actual
    for (int j = 0; j < 8; j++) {
      // Generar un número aleatorio entre 0 y 1
      float r = random(0, 100) / 100.0;
      
      // Si el número aleatorio es menor que la probabilidad de error f,
      // invertir el bit correspondiente en el byte de salida
      if (r < f) {
        // Invertir el bit j-ésimo usando XOR con una máscara
        // La máscara tiene un 1 en la posición j y 0 en las demás posiciones
        out[i] ^= (1 << j);
      }
    }
  }
}

// Función para imprimir un vector de bytes en formato binario
void printBinaryVector(unsigned char *vec, int length) {
  for (int i = 0; i < length; i++) {
    for (int j = 7; j >= 0; j--) {
      // Extraer y mostrar cada bit, empezando por el más significativo
      Serial.print((vec[i] >> j) & 1);
    }
    Serial.print(" ");
  }
  Serial.println();
}

void setup() {
  // Inicializar comunicación serial
  Serial.begin(9600);
  while (!Serial) {
    ; // Esperar a que el puerto serial se conecte
  }
  
  // Ejemplo de uso de la función noisyChannel
  const int length = 3; // Longitud del vector en bytes
  unsigned char input[length] = {0b10101010, 0b11110000, 0b00001111};
  unsigned char output[length];
  float errorProb = 0.2; // Probabilidad de error del 20%
  
  Serial.println("Vector de entrada:");
  printBinaryVector(input, length);
  
  // Aplicar el canal ruidoso
  noisyChannel(input, output, length, errorProb);
  
  Serial.println("Vector de salida (después del canal ruidoso):");
  printBinaryVector(output, length);
  
  // Contar bits que han cambiado
  int changedBits = 0;
  for (int i = 0; i < length; i++) {
    unsigned char diff = input[i] ^ output[i]; // XOR para detectar bits diferentes
    for (int j = 0; j < 8; j++) {
      if ((diff >> j) & 1) {
        changedBits++;
      }
    }
  }
  
  Serial.print("Bits cambiados: ");
  Serial.print(changedBits);
  Serial.print(" de ");
  Serial.print(length * 8);
  Serial.print(" (");
  Serial.print((float)changedBits / (length * 8) * 100);
  Serial.println("%)");
}

void loop() {
  // No se requiere código en el loop para esta práctica
}
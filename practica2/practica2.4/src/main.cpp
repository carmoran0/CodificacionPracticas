/*
 * Práctica 2.4 - Codificador de bloque Hamming (7,4)
 * 
 * Esta función implementa un codificador de bloque Hamming (7,4) que toma bloques de 4 bits
 * y genera palabras código de 7 bits añadiendo 3 bits de paridad.
 * Cada byte de entrada contiene dos bloques de 4 bits que deben codificarse por separado.
 */

/*
 * Función que implementa un codificador de bloque Hamming (7,4).
 * @param in Vector binario de entrada empaquetado en unsigned char
 * @param out Vector binario de salida empaquetado en unsigned char
 * @param l Longitud del vector de entrada en bytes
 */
void hammingCoder(unsigned char *in, unsigned char *out, int l) {
  int outBitIndex = 0; // Índice del bit actual en el vector de salida
  
  // Recorrer cada byte del vector de entrada
  for (int i = 0; i < l; i++) {
    // Procesar cada bloque de 4 bits del byte actual (primero los 4 bits más significativos)
    for (int nibbleIndex = 0; nibbleIndex < 2; nibbleIndex++) {
      // Extraer el bloque de 4 bits
      unsigned char nibble;
      if (nibbleIndex == 0) {
        // Extraer los 4 bits más significativos (posiciones 7-4)
        nibble = (in[i] >> 4) & 0x0F;
      } else {
        // Extraer los 4 bits menos significativos (posiciones 3-0)
        nibble = in[i] & 0x0F;
      }
      
      // Extraer los bits de datos individuales (d1, d2, d3, d4)
      unsigned char d1 = (nibble >> 3) & 1; // Bit más significativo
      unsigned char d2 = (nibble >> 2) & 1;
      unsigned char d3 = (nibble >> 1) & 1;
      unsigned char d4 = nibble & 1;        // Bit menos significativo
      
      // Calcular los bits de paridad según las ecuaciones de Hamming (7,4)
      unsigned char p1 = d1 ^ d2 ^ d4;      // p1 = d1 + d2 + d4 (XOR)
      unsigned char p2 = d1 ^ d3 ^ d4;      // p2 = d1 + d3 + d4 (XOR)
      unsigned char p3 = d2 ^ d3 ^ d4;      // p3 = d2 + d3 + d4 (XOR)
      
      // Formar la palabra código de 7 bits: p1, p2, d1, p3, d2, d3, d4
      unsigned char codeword[7] = {p1, p2, d1, p3, d2, d3, d4};
      
      // Colocar los 7 bits de la palabra código en el vector de salida
      for (int j = 0; j < 7; j++) {
        // Calcular el índice del byte de salida donde se colocará el bit
        int outByteIndex = outBitIndex / 8;
        // Calcular la posición del bit dentro del byte de salida
        int outBitPosition = 7 - (outBitIndex % 8);
        
        // Si estamos en un nuevo byte, inicializarlo a 0
        if (outBitPosition == 7) {
          out[outByteIndex] = 0;
        }
        
        // Colocar el bit en la posición correspondiente del byte de salida
        out[outByteIndex] |= (codeword[j] << outBitPosition);
        
        // Incrementar el índice del bit de salida
        outBitIndex++;
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
  
  // Ejemplo de uso del codificador Hamming (7,4)
  const int length = 2; // Longitud del vector de entrada en bytes
  unsigned char input[length] = {0b10101010, 0b11110000};
  
  // Calcular el tamaño necesario para el vector de salida
  // Cada byte tiene 2 bloques de 4 bits, y cada bloque genera 7 bits
  int outputBits = length * 2 * 7; // Número total de bits en la salida
  int outputLength = (outputBits + 7) / 8; // Redondeo hacia arriba para obtener bytes
  unsigned char output[outputLength];
  
  Serial.println("Vector de entrada:");
  printBinaryVector(input, length);
  
  // Aplicar el codificador Hamming (7,4)
  hammingCoder(input, output, length);
  
  Serial.println("Vector de salida (después del codificador Hamming):");
  printBinaryVector(output, outputLength);
  
  // Mostrar información sobre la codificación
  Serial.println("Codificación Hamming (7,4):");
  Serial.print("Bits de entrada: ");
  Serial.println(length * 8);
  Serial.print("Bits de salida: ");
  Serial.println(outputLength * 8);
  Serial.print("Tasa de código: ");
  Serial.println("4/7");
}

void loop() {
  // No se requiere código en el loop para esta práctica
}
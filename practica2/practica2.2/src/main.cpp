/*
 * Práctica 2.2 - Codificador de repetición
 * 
 * Esta función implementa un codificador de repetición de grado Rn donde n es el número
 * de bits que se repiten en cada bloque. Recibe un mensaje binario empaquetado en bytes,
 * y genera una versión codificada donde cada bit se repite n veces.
 */
#include <Arduino.h>
/*
 * Función que implementa un codificador de repetición.
 * @param in Vector binario de entrada empaquetado en unsigned char
 * @param out Vector binario de salida empaquetado en unsigned char
 * @param l Longitud del vector de entrada en bytes
 * @param n Grado del codificador de repetición (número de veces que se repite cada bit)
 */
void repetitionCoder(unsigned char *in, unsigned char *out, int l, int n) {
  int outBitIndex = 0; // Índice del bit actual en el vector de salida
  
  // Recorrer cada byte del vector de entrada
  for (int i = 0; i < l; i++) {
    // Procesar cada bit del byte actual
    for (int j = 7; j >= 0; j--) {
      // Extraer el bit j-ésimo del byte i-ésimo
      unsigned char bit = (in[i] >> j) & 1;
      
      // Repetir el bit n veces
      for (int k = 0; k < n; k++) {
        // Calcular el índice del byte de salida donde se colocará el bit repetido
        int outByteIndex = outBitIndex / 8;
        // Calcular la posición del bit dentro del byte de salida
        int outBitPosition = 7 - (outBitIndex % 8);
        
        // Si estamos en un nuevo byte, inicializarlo a 0
        if (outBitPosition == 7) {
          out[outByteIndex] = 0;
        }
        
        // Colocar el bit en la posición correspondiente del byte de salida
        out[outByteIndex] |= (bit << outBitPosition);
        
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
  
  // Ejemplo de uso del codificador de repetición
  const int length = 2; // Longitud del vector de entrada en bytes
  unsigned char input[length] = {0b10101010, 0b11110000};
  int n = 6; // Grado del codificador (cada bit se repite 3 veces)
  
  // Calcular el tamaño necesario para el vector de salida
  // Cada bit de entrada genera n bits de salida
  int outputLength = (length * 8 * n + 7) / 8; // Redondeo hacia arriba
  unsigned char output[outputLength];
  
  Serial.println("Vector de entrada:");
  printBinaryVector(input, length);
  
  // Aplicar el codificador de repetición
  repetitionCoder(input, output, length, n);
  
  Serial.println("Vector de salida (después del codificador de repetición):");
  printBinaryVector(output, outputLength);
  
  // Mostrar información sobre la codificación
  Serial.print("Grado de repetición: ");
  Serial.println(n);
  Serial.print("Bits de entrada: ");
  Serial.println(length * 8);
  Serial.print("Bits de salida: ");
  Serial.println(outputLength * 8);
}

void loop() {
}
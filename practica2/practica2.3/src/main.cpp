/*
 * Práctica 2.3 - Decodificador de repetición
 * 
 * Esta función implementa un decodificador de repetición de grado Rn donde n es el número
 * de bits que se repiten en cada bloque. Recibe un mensaje codificado donde cada bit original
 * se ha repetido n veces, y recupera el mensaje original mediante un sistema de votación por mayoría.
 */
#include <Arduino.h>

/**
 * Función que implementa un decodificador de repetición.
 * @param in Vector binario de entrada empaquetado en unsigned char (mensaje codificado)
 * @param out Vector binario de salida empaquetado en unsigned char (mensaje decodificado)
 * @param l Longitud del vector de entrada en bytes
 * @param n Grado del decodificador de repetición (número de veces que se repitió cada bit)
 */
void repetitionDecoder(unsigned char *in, unsigned char *out, int l, int n) {
  int inBitIndex = 0;  // Índice del bit actual en el vector de entrada
  int outBitIndex = 0; // Índice del bit actual en el vector de salida
  
  // Inicializar el vector de salida a ceros
  int outLength = (l * 8) / n; // Número total de bits en el mensaje original
  int outBytes = (outLength + 7) / 8; // Número de bytes necesarios para almacenar el mensaje original
  
  for (int i = 0; i < outBytes; i++) {
    out[i] = 0;
  }
  
  // Procesar cada grupo de n bits repetidos
  while (inBitIndex < l * 8) {
    int countOnes = 0; // Contador de unos en el grupo actual
    
    // Contar cuántos unos hay en el grupo de n bits
    for (int k = 0; k < n && inBitIndex < l * 8; k++) {
      // Calcular el índice del byte de entrada y la posición del bit
      int inByteIndex = inBitIndex / 8;
      int inBitPosition = 7 - (inBitIndex % 8);
      
      // Extraer el bit de la posición correspondiente
      unsigned char bit = (in[inByteIndex] >> inBitPosition) & 1;
      
      // Incrementar el contador si el bit es 1
      if (bit == 1) {
        countOnes++;
      }
      
      // Avanzar al siguiente bit de entrada
      inBitIndex++;
    }
    
    // Determinar el bit original mediante votación por mayoría
    unsigned char decodedBit = (countOnes > n / 2) ? 1 : 0;
    
    // Calcular el índice del byte de salida y la posición del bit
    int outByteIndex = outBitIndex / 8;
    int outBitPosition = 7 - (outBitIndex % 8);
    
    // Colocar el bit decodificado en la posición correspondiente del byte de salida
    out[outByteIndex] |= (decodedBit << outBitPosition);
    
    // Avanzar al siguiente bit de salida
    outBitIndex++;
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
  
  // Ejemplo de uso del decodificador de repetición
  const int n = 3; // Grado del codificador/decodificador (cada bit se repite 3 veces)
  
  // Mensaje original para demostración
  const int originalLength = 2; // Longitud del mensaje original en bytes
  unsigned char original[originalLength] = {0b10101010, 0b11110000};
  
  // Calcular el tamaño necesario para el mensaje codificado
  int codedLength = (originalLength * 8 * n + 7) / 8; // Redondeo hacia arriba
  unsigned char coded[codedLength];
  
  // Codificar el mensaje (simulando el uso del codificador de repetición)
  int outBitIndex = 0;
  for (int i = 0; i < originalLength; i++) {
    for (int j = 7; j >= 0; j--) {
      unsigned char bit = (original[i] >> j) & 1;
      for (int k = 0; k < n; k++) {
        int outByteIndex = outBitIndex / 8;
        int outBitPosition = 7 - (outBitIndex % 8);
        if (outBitPosition == 7) {
          coded[outByteIndex] = 0;
        }
        coded[outByteIndex] |= (bit << outBitPosition);
        outBitIndex++;
      }
    }
  }
  
  // Mensaje decodificado
  unsigned char decoded[originalLength];
  
  Serial.println("Mensaje original:");
  printBinaryVector(original, originalLength);
  
  Serial.println("Mensaje codificado:");
  printBinaryVector(coded, codedLength);
  
  // Aplicar el decodificador de repetición
  repetitionDecoder(coded, decoded, codedLength, n);
  
  Serial.println("Mensaje decodificado:");
  printBinaryVector(decoded, originalLength);
  
  // Mostrar información sobre la decodificación
  Serial.print("Grado de repetición: ");
  Serial.println(n);
  Serial.print("Bits del mensaje codificado: ");
  Serial.println(codedLength * 8);
  Serial.print("Bits del mensaje decodificado: ");
  Serial.println(originalLength * 8);
}

void loop() {
  // No se requiere código en el loop para esta práctica
}
/*
 * Práctica 2.8 - Clase HammingCode
 * 
 * Esta clase implementa un codificador y decodificador Hamming (7,4) que encapsula
 * la funcionalidad de codificación y decodificación Hamming en una clase reutilizable.
 * Permite codificar bloques de 4 bits en palabras código de 7 bits y decodificarlos
 * con capacidad de corrección de errores de un solo bit.
 */
#include <Arduino.h>

/**
 * Clase que implementa un codificador y decodificador Hamming (7,4).
 * Permite codificar mensajes usando el código Hamming (7,4) y decodificarlos
 * con capacidad de corrección de errores de un solo bit.
 */
class HammingCode {
private:
  /**
   * Método auxiliar para imprimir un vector de bytes en formato binario
   * @param vec Vector a imprimir
   * @param length Longitud del vector en bytes
   */
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
  
public:
  /**
   * Constructor de la clase
   */
  HammingCode() {
    // No se requieren parámetros para Hamming (7,4)
  }
  
  /**
   * Método para calcular la longitud del mensaje codificado en bytes
   * @param originalLength Longitud del mensaje original en bytes
   * @return Longitud del mensaje codificado en bytes
   */
  int getEncodedLength(int originalLength) {
    // Cada byte tiene 2 bloques de 4 bits, y cada bloque genera 7 bits
    int codedBits = originalLength * 2 * 7; // Número total de bits en la salida codificada
    return (codedBits + 7) / 8; // Redondeo hacia arriba para obtener bytes
  }
  
  /**
   * Método para codificar un mensaje utilizando Hamming (7,4)
   * @param in Vector binario de entrada empaquetado en unsigned char
   * @param out Vector binario de salida empaquetado en unsigned char
   * @param length Longitud del vector de entrada en bytes
   */
  void encode(unsigned char *in, unsigned char *out, int length) {
    int outBitIndex = 0; // Índice del bit actual en el vector de salida
    
    // Recorrer cada byte del vector de entrada
    for (int i = 0; i < length; i++) {
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
        unsigned char resultado[7] = {p1, p2, d1, p3, d2, d3, d4};
        
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
          out[outByteIndex] |= (resultado[j] << outBitPosition);
          
          // Incrementar el índice del bit de salida
          outBitIndex++;
        }
      }
    }
  }
  
  /**
   * Método para decodificar un mensaje codificado con Hamming (7,4)
   * @param in Vector binario de entrada empaquetado en unsigned char (mensaje codificado)
   * @param out Vector binario de salida empaquetado en unsigned char (mensaje decodificado)
   * @param length Longitud del vector de entrada en bytes
   */
  void decode(unsigned char *in, unsigned char *out, int length) {
    int inBitIndex = 0;   // Índice del bit actual en el vector de entrada
    int outBitIndex = 0;  // Índice del bit actual en el vector de salida
    
    // Inicializar el vector de salida a ceros
    int outBytes = (length * 8 * 4) / 7; // Aproximación del número de bytes necesarios
    outBytes = (outBytes + 7) / 8;  // Redondeo hacia arriba
    
    for (int i = 0; i < outBytes; i++) {
      out[i] = 0;
    }
    
    // Procesar cada palabra código de 7 bits
    while (inBitIndex + 6 < length * 8) { // Asegurarse de que hay al menos 7 bits disponibles
      // Extraer los 7 bits de la palabra código
      unsigned char codeword[7];
      
      for (int j = 0; j < 7; j++) {
        // Calcular el índice del byte de entrada y la posición del bit
        int inByteIndex = inBitIndex / 8;
        int inBitPosition = 7 - (inBitIndex % 8);
        
        // Extraer el bit de la posición correspondiente
        codeword[j] = (in[inByteIndex] >> inBitPosition) & 1;
        
        // Avanzar al siguiente bit de entrada
        inBitIndex++;
      }
      
      // Extraer bits de paridad y datos de la palabra código
      unsigned char p1 = codeword[0]; // Bit de paridad 1
      unsigned char p2 = codeword[1]; // Bit de paridad 2
      unsigned char d1 = codeword[2]; // Bit de dato 1
      unsigned char p3 = codeword[3]; // Bit de paridad 3
      unsigned char d2 = codeword[4]; // Bit de dato 2
      unsigned char d3 = codeword[5]; // Bit de dato 3
      unsigned char d4 = codeword[6]; // Bit de dato 4
      
      // Calcular el síndrome para detectar errores
      unsigned char s1 = p1 ^ d1 ^ d2 ^ d4; // Verificar paridad 1
      unsigned char s2 = p2 ^ d1 ^ d3 ^ d4; // Verificar paridad 2
      unsigned char s3 = p3 ^ d2 ^ d3 ^ d4; // Verificar paridad 3
      
      // Determinar la posición del error (si existe)
      unsigned char errorPos = (s3 << 2) | (s2 << 1) | s1;
      
      // Corregir el error si se detecta (errorPos != 0)
      if (errorPos != 0) {
        // Posiciones de error y su correspondencia en el array codeword
        // 1 -> p1 (índice 0)
        // 2 -> p2 (índice 1)
        // 3 -> d1 (índice 2)
        // 4 -> p3 (índice 3)
        // 5 -> d2 (índice 4)
        // 6 -> d3 (índice 5)
        // 7 -> d4 (índice 6)
        
        // Corregir el bit erróneo
        if (errorPos >= 1 && errorPos <= 7) {
          // Invertir el bit en la posición del error
          codeword[errorPos - 1] = codeword[errorPos - 1] ^ 1;
          
          // Actualizar los bits de datos si fueron corregidos
          if (errorPos == 3) d1 = codeword[2];
          else if (errorPos == 5) d2 = codeword[4];
          else if (errorPos == 6) d3 = codeword[5];
          else if (errorPos == 7) d4 = codeword[6];
        }
      }
      
      // Reconstruir el nibble (4 bits) original
      unsigned char nibble = (d1 << 3) | (d2 << 2) | (d3 << 1) | d4;
      
      // Determinar si este nibble va en la parte alta o baja del byte de salida
      if (outBitIndex % 8 == 0) {
        // Parte alta del byte (bits 7-4)
        out[outBitIndex / 8] |= (nibble << 4);
      } else {
        // Parte baja del byte (bits 3-0)
        out[outBitIndex / 8] |= nibble;
      }
      
      // Avanzar 4 bits en el índice de salida (un nibble)
      outBitIndex += 4;
    }
  }
  
  /**
   * Método para mostrar información sobre un mensaje y su versión codificada
   * @param original Mensaje original
   * @param coded Mensaje codificado
   * @param originalLength Longitud del mensaje original en bytes
   * @param codedLength Longitud del mensaje codificado en bytes
   */
  void printInfo(unsigned char *original, unsigned char *coded, int originalLength, int codedLength) {
    Serial.println("Mensaje original:");
    printBinaryVector(original, originalLength);
    
    Serial.println("Mensaje codificado con Hamming (7,4):");
    printBinaryVector(coded, codedLength);
    
    Serial.print("Bits del mensaje original: ");
    Serial.println(originalLength * 8);
    Serial.print("Bits del mensaje codificado: ");
    Serial.println(codedLength * 8);
    Serial.print("Tasa de código: ");
    Serial.println("4/7");
  }
};

void setup() {
  // Inicializar comunicación serial
  Serial.begin(9600);
  while (!Serial) {
    ; // Esperar a que el puerto serial se conecte
  }
  
  // Crear una instancia de HammingCode
  HammingCode hammingCode;
  
  // Mensaje original para demostración
  const int originalLength = 2; // Longitud del mensaje original en bytes
  unsigned char original[originalLength] = {0b10101010, 0b11110000};
  
  // Calcular el tamaño necesario para el mensaje codificado
  int codedLength = hammingCode.getEncodedLength(originalLength);
  unsigned char coded[codedLength];
  
  // Codificar el mensaje
  hammingCode.encode(original, coded, originalLength);
  
  // Mensaje decodificado
  unsigned char decoded[originalLength];
  
  // Mostrar información sobre el mensaje original y codificado
  hammingCode.printInfo(original, coded, originalLength, codedLength);
  
  // Introducir un error en el mensaje codificado para probar la corrección
  // Invertir el bit en la posición 10 (por ejemplo)
  int errorBitIndex = 10;
  int errorByteIndex = errorBitIndex / 8;
  int errorBitPosition = 7 - (errorBitIndex % 8);
  coded[errorByteIndex] ^= (1 << errorBitPosition);
  
  Serial.println("\nMensaje codificado con un error introducido:");
  printBinaryVector(coded, codedLength);
  
  // Decodificar el mensaje
  hammingCode.decode(coded, decoded, codedLength);
  
  Serial.println("Mensaje decodificado (después de corregir el error):");
  for (int i = 0; i < originalLength; i++) {
    for (int j = 7; j >= 0; j--) {
      Serial.print((decoded[i] >> j) & 1);
    }
    Serial.print(" ");
  }
  Serial.println();
  
  // Verificar si la decodificación fue correcta
  bool correct = true;
  for (int i = 0; i < originalLength; i++) {
    if (original[i] != decoded[i]) {
      correct = false;
      break;
    }
  }
  
  Serial.print("Decodificación correcta: ");
  Serial.println(correct ? "Sí" : "No");
}

void loop() {
  // No se requiere código en el loop para esta práctica
}
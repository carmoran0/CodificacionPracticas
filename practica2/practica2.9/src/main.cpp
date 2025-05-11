/*
 * Práctica 2.9 - Comparación de códigos de repetición y Hamming
 * 
 * Este programa compara el rendimiento de los códigos de repetición y Hamming
 * al transmitir datos a través de un canal ruidoso con probabilidad de error 0.05.
 * Analiza las ventajas e inconvenientes en términos de tasa de transmisión y
 * porcentaje de error para cada sistema de codificación.
 */

#include <Arduino.h>
#include <string.h>

// Probabilidad de error del canal ruidoso
const float ERROR_PROBABILITY = 0.05;

/**
 * Función que simula un canal ruidoso.
 * @param in Vector binario de entrada empaquetado en unsigned char
 * @param out Vector binario de salida empaquetado en unsigned char
 * @param l Longitud del vector en bytes
 * @param f Probabilidad de que un bit cambie de valor (entre 0 y 1)
 */
void noisyChannel(unsigned char *in, unsigned char *out, int l, float f) {
  // Inicializar la semilla para la generación de números aleatorios
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

/**
 * Clase que implementa un codificador y decodificador de repetición.
 * Permite codificar mensajes repitiendo cada bit n veces y decodificarlos
 * mediante un sistema de votación por mayoría.
 */
class RepetitionCode {
private:
  int repetitionDegree; // Grado de repetición (número de veces que se repite cada bit)
  
public:
  /**
   * Constructor de la clase
   * @param n Grado de repetición (número de veces que se repite cada bit)
   */
  RepetitionCode(int n) {
    repetitionDegree = n;
  }
  
  /**
   * Método para establecer el grado de repetición
   * @param n Nuevo grado de repetición
   */
  void setRepetitionDegree(int n) {
    repetitionDegree = n;
  }
  
  /**
   * Método para obtener el grado de repetición actual
   * @return Grado de repetición
   */
  int getRepetitionDegree() {
    return repetitionDegree;
  }
  
  /**
   * Método para calcular la longitud del mensaje codificado en bytes
   * @param originalLength Longitud del mensaje original en bytes
   * @return Longitud del mensaje codificado en bytes
   */
  int getEncodedLength(int originalLength) {
    return (originalLength * 8 * repetitionDegree + 7) / 8; // Redondeo hacia arriba
  }
  
  /**
   * Método para codificar un mensaje utilizando repetición
   * @param in Vector binario de entrada empaquetado en unsigned char
   * @param out Vector binario de salida empaquetado en unsigned char
   * @param length Longitud del vector de entrada en bytes
   */
  void encode(unsigned char *in, unsigned char *out, int length) {
    int outBitIndex = 0; // Índice del bit actual en el vector de salida
    
    // Recorrer cada byte del vector de entrada
    for (int i = 0; i < length; i++) {
      // Procesar cada bit del byte actual
      for (int j = 7; j >= 0; j--) {
        // Extraer el bit j-ésimo del byte i-ésimo
        unsigned char bit = (in[i] >> j) & 1;
        
        // Repetir el bit n veces
        for (int k = 0; k < repetitionDegree; k++) {
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
  
  /**
   * Método para decodificar un mensaje codificado con repetición
   * @param in Vector binario de entrada empaquetado en unsigned char (mensaje codificado)
   * @param out Vector binario de salida empaquetado en unsigned char (mensaje decodificado)
   * @param length Longitud del vector de entrada en bytes
   */
  void decode(unsigned char *in, unsigned char *out, int length) {
    int inBitIndex = 0;  // Índice del bit actual en el vector de entrada
    int outBitIndex = 0; // Índice del bit actual en el vector de salida
    
    // Inicializar el vector de salida a ceros
    int outLength = (length * 8) / repetitionDegree; // Número total de bits en el mensaje original
    int outBytes = (outLength + 7) / 8; // Número de bytes necesarios para almacenar el mensaje original
    
    for (int i = 0; i < outBytes; i++) {
      out[i] = 0;
    }
    
    // Procesar cada grupo de n bits repetidos
    while (inBitIndex < length * 8) {
      int countOnes = 0; // Contador de unos en el grupo actual
      
      // Contar cuántos unos hay en el grupo de n bits
      for (int k = 0; k < repetitionDegree && inBitIndex < length * 8; k++) {
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
      unsigned char decodedBit = (countOnes > repetitionDegree / 2) ? 1 : 0;
      
      // Calcular el índice del byte de salida y la posición del bit
      int outByteIndex = outBitIndex / 8;
      int outBitPosition = 7 - (outBitIndex % 8);
      
      // Colocar el bit decodificado en la posición correspondiente del byte de salida
      out[outByteIndex] |= (decodedBit << outBitPosition);
      
      // Avanzar al siguiente bit de salida
      outBitIndex++;
    }
  }
};

/**
 * Clase que implementa un codificador y decodificador Hamming (7,4).
 * Permite codificar mensajes usando el código Hamming (7,4) y decodificarlos
 * con capacidad de corrección de errores de un solo bit.
 */
class HammingCode {
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
};

/**
 * Función para contar bits diferentes entre dos vectores
 * @param vec1 Primer vector
 * @param vec2 Segundo vector
 * @param length Longitud de los vectores en bytes
 * @return Número de bits diferentes
 */
int countDifferentBits(unsigned char *vec1, unsigned char *vec2, int length) {
  int count = 0;
  for (int i = 0; i < length; i++) {
    unsigned char diff = vec1[i] ^ vec2[i]; // XOR para detectar bits diferentes
    for (int j = 0; j < 8; j++) {
      if ((diff >> j) & 1) {
        count++;
      }
    }
  }
  return count;
}

/**
 * Función para leer un archivo de texto y convertirlo en un vector de bytes
 * @param filename Nombre del archivo
 * @param buffer Buffer donde se almacenarán los datos
 * @param maxLength Tamaño máximo del buffer
 * @return Número de bytes leídos
 */
int readTextFile(const char *filename, unsigned char *buffer, int maxLength) {
  // En Arduino, simulamos la lectura del archivo con un texto predefinido
  const char *text = "The quick brown fox jumps over the lazy dog";
  int length = strlen(text);
  
  // Asegurarse de no exceder el tamaño del buffer
  if (length > maxLength) {
    length = maxLength;
  }
  
  // Copiar el texto al buffer
  memcpy(buffer, text, length);
  
  return length;
}

void setup() {
  // Inicializar comunicación serial
  Serial.begin(9600);
  while (!Serial) {
    ; // Esperar a que el puerto serial se conecte
  }
  
  Serial.println("Comparación de códigos de repetición y Hamming");
  Serial.println("===========================================\n");
  
  // Leer el archivo de texto
  const int MAX_LENGTH = 100;
  unsigned char originalData[MAX_LENGTH];
  int dataLength = readTextFile("TEXTO.txt", originalData, MAX_LENGTH);
  
  Serial.print("Texto original: ");
  for (int i = 0; i < dataLength; i++) {
    Serial.print((char)originalData[i]);
  }
  Serial.println();
  Serial.print("Longitud: ");
  Serial.print(dataLength);
  Serial.print(" bytes (");
  Serial.print(dataLength * 8);
  Serial.println(" bits)");
  Serial.println();
  
  // Crear instancias de los codificadores
  RepetitionCode repCode(3); // Código de repetición con grado 3
  HammingCode hammingCode;   // Código Hamming (7,4)
  
  // Calcular tamaños de los mensajes codificados
  int repCodedLength = repCode.getEncodedLength(dataLength);
  int hammingCodedLength = hammingCode.getEncodedLength(dataLength);
  
  // Crear buffers para los mensajes codificados
  unsigned char repCoded[repCodedLength];
  unsigned char hammingCoded[hammingCodedLength];
  
  // Codificar los mensajes
  repCode.encode(originalData, repCoded, dataLength);
  hammingCode.encode(originalData, hammingCoded, dataLength);
  
  // Crear buffers para los mensajes después del canal ruidoso
  unsigned char repNoisyChannel[repCodedLength];
  unsigned char hammingNoisyChannel[hammingCodedLength];
  
  // Simular el canal ruidoso
  noisyChannel(repCoded, repNoisyChannel, repCodedLength, ERROR_PROBABILITY);
  noisyChannel(hammingCoded, hammingNoisyChannel, hammingCodedLength, ERROR_PROBABILITY);
  
  // Crear buffers para los mensajes decodificados
  unsigned char repDecoded[dataLength];
  unsigned char hammingDecoded[dataLength];
  
  // Decodificar los mensajes
  repCode.decode(repNoisyChannel, repDecoded, repCodedLength);
  hammingCode.decode(hammingNoisyChannel, hammingDecoded, hammingCodedLength);
  
  // Calcular estadísticas para el código de repetición
  int repBitsTransmitted = repCodedLength * 8;
  int repBitsOriginal = dataLength * 8;
  float repRate = (float)repBitsOriginal / repBitsTransmitted;
  int repErrorsChannel = countDifferentBits(repCoded, repNoisyChannel, repCodedLength);
  int repErrorsFinal = countDifferentBits(originalData, repDecoded, dataLength);
  float repErrorPercentChannel = (float)repErrorsChannel / repBitsTransmitted * 100;
  float repErrorPercentFinal = (float)repErrorsFinal / repBitsOriginal * 100;
  
  // Calcular estadísticas para el código Hamming
  int hammingBitsTransmitted = hammingCodedLength * 8;
  int hammingBitsOriginal = dataLength * 8;
  float hammingRate = (float)hammingBitsOriginal / hammingBitsTransmitted;
  int hammingErrorsChannel = countDifferentBits(hammingCoded, hammingNoisyChannel, hammingCodedLength);
  int hammingErrorsFinal = countDifferentBits(originalData, hammingDecoded, dataLength);
  float hammingErrorPercentChannel = (float)hammingErrorsChannel / hammingBitsTransmitted * 100;
  float hammingErrorPercentFinal = (float)hammingErrorsFinal / hammingBitsOriginal * 100;
  
  // Mostrar resultados para el código de repetición
  Serial.println("CÓDIGO DE REPETICIÓN (R3)");
  Serial.println("-------------------------");
  Serial.print("Tasa de código: ");
  Serial.println(repRate);
  Serial.print("Bits transmitidos: ");
  Serial.println(repBitsTransmitted);
  Serial.print("Errores en el canal: ");
  Serial.print(repErrorsChannel);
  Serial.print(" (");
  Serial.print(repErrorPercentChannel);
  Serial.println("%)");
  Serial.print("Errores después de decodificar: ");
  Serial.print(repErrorsFinal);
  Serial.print(" (");
  Serial.print(repErrorPercentFinal);
  Serial.println("%)");
  Serial.println();
  
  // Mostrar resultados para el código Hamming
  Serial.println("CÓDIGO HAMMING (7,4)");
  Serial.println("-------------------");
  Serial.print("Tasa de código: ");
  Serial.println(hammingRate);
  Serial.print("Bits transmitidos: ");
  Serial.println(hammingBitsTransmitted);
  Serial.print("Errores en el canal: ");
  Serial.print(hammingErrorsChannel);
  Serial.print(" (");
  Serial.print(hammingErrorPercentChannel);
  Serial.println("%)");
  Serial.print("Errores después de decodificar: ");
  Serial.print(hammingErrorsFinal);
  Serial.print(" (");
  Serial.print(hammingErrorPercentFinal);
  Serial.println("%)");
  Serial.println();
  
  // Comparación de resultados
  Serial.println("COMPARACIÓN DE RESULTADOS");
  Serial.println("------------------------");
  Serial.println("Ventajas del código de repetición:");
  Serial.println("- Implementación más simple");
  if (repErrorPercentFinal < hammingErrorPercentFinal) {
    Serial.println("- Menor tasa de error final");
  }
  
  Serial.println("\nVentajas del código Hamming:");
  Serial.println("- Mayor eficiencia en la tasa de código");
  if (hammingErrorPercentFinal < repErrorPercentFinal) {
    Serial.println("- Menor tasa de error final");
  }
  
  Serial.println("\nConclusiones:");
  if (hammingRate > repRate) {
    Serial.println("- Hamming es más eficiente en términos de tasa de transmisión");
  } else {
    Serial.println("- Repetición es más eficiente en términos de tasa de transmisión");
  }
  
  if (hammingErrorPercentFinal < repErrorPercentFinal) {
    Serial.println("- Hamming tiene mejor capacidad de corrección de errores");
  } else if (repErrorPercentFinal < hammingErrorPercentFinal) {
    Serial.println("- Repetición tiene mejor capacidad de corrección de errores");
  } else {
    Serial.println("- Ambos códigos tienen similar capacidad de corrección de errores");
  }
}

void loop() {
  // No se requiere código en el loop para esta práctica
}
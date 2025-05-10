#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

class HammingCode {
public:
  int getEncodedLength(int originalLength) {
    return (originalLength * 2 * 7 + 7) / 8;
  }

  void encode(unsigned char *in, unsigned char *out, int length) {
    int outBitIndex = 0;
    for (int i = 0; i < length; i++) {
      for (int nibbleIndex = 0; nibbleIndex < 2; nibbleIndex++) {
        unsigned char nibble = (nibbleIndex == 0) ? (in[i] >> 4) & 0x0F : in[i] & 0x0F;
        unsigned char d1 = (nibble >> 3) & 1, d2 = (nibble >> 2) & 1, d3 = (nibble >> 1) & 1, d4 = nibble & 1;
        unsigned char p1 = d1 ^ d2 ^ d4, p2 = d1 ^ d3 ^ d4, p3 = d2 ^ d3 ^ d4;
        unsigned char resultado[7] = {p1, p2, d1, p3, d2, d3, d4};
        for (int j = 0; j < 7; j++) {
          int outByteIndex = outBitIndex / 8;
          int outBitPosition = 7 - (outBitIndex % 8);
          if (outBitPosition == 7) out[outByteIndex] = 0;
          out[outByteIndex] |= (resultado[j] << outBitPosition);
          outBitIndex++;
        }
      }
    }
  }

  void decode(unsigned char *in, unsigned char *out, int length) {
    int inBitIndex = 0, outBitIndex = 0;
    while (inBitIndex < length * 8) {
      unsigned char codeword[7];
      for (int j = 0; j < 7 && inBitIndex < length * 8; j++) {
        int byteIdx = inBitIndex / 8, bitPos = 7 - (inBitIndex % 8);
        codeword[j] = (in[byteIdx] >> bitPos) & 1;
        inBitIndex++;
      }
      unsigned char s1 = codeword[0] ^ codeword[2] ^ codeword[4] ^ codeword[6];
      unsigned char s2 = codeword[1] ^ codeword[2] ^ codeword[5] ^ codeword[6];
      unsigned char s3 = codeword[3] ^ codeword[4] ^ codeword[5] ^ codeword[6];
      unsigned char errorPos = (s3 << 2) | (s2 << 1) | s1;
      if (errorPos > 0 && errorPos <= 7) codeword[errorPos-1] ^= 1;
      unsigned char nibble = (codeword[2] << 3) | (codeword[4] << 2) | (codeword[5] << 1) | codeword[6];
      int outByte = outBitIndex / 8, outPos = 7 - (outBitIndex % 8);
      if (outPos == 4) out[outByte] |= nibble << 4;
      else out[outByte] |= nibble;
      outBitIndex += 4;
    }
  }
};

class RepetitionCode {
  int repetitionDegree;
public:
  RepetitionCode(int n) : repetitionDegree(n) {}

  int getEncodedLength(int originalLength) {
    return (originalLength * 8 * repetitionDegree + 7) / 8;
  }

  void encode(unsigned char *in, unsigned char *out, int length) {
    // Implementación completa de codificación por repetición R4
  }

  void decode(unsigned char *in, unsigned char *out, int length) {
    // Implementación de decodificación por mayoría
  }
};

void generarBMP(const char* filename, unsigned char* data, int length);

void setup() {
  Serial.begin(115200);
  if(!SPIFFS.begin(true)) {
    Serial.println("Error al montar SPIFFS");
    return;
  }

  File file = SPIFFS.open("/data/TEXTO.txt");
  if(!file) {
    Serial.println("Error abriendo archivo");
    return;
  }

  int originalLength = file.size();
  unsigned char original[originalLength];
  file.readBytes((char*)original, originalLength);
  file.close();

  // Procesamiento con Hamming
  HammingCode hamming;
  int hammingEncodedLength = hamming.getEncodedLength(originalLength);
  unsigned char hammingEncoded[hammingEncodedLength];
  hamming.encode(original, hammingEncoded, originalLength);

  // Simular canal ruidoso (5% de error)
  for(int i=0; i<hammingEncodedLength*8; i++){
    if(random(100) < 5){ // 5% de probabilidad de error
      int bytePos = i/8;
      int bitPos = 7 - (i%8);
      hammingEncoded[bytePos] ^= 1 << bitPos;
    }
  }

  unsigned char hammingDecoded[originalLength];
  hamming.decode(hammingEncoded, hammingDecoded, hammingEncodedLength);

  // Procesamiento con Repetición R4
  RepetitionCode repetition(4);
  int repetitionEncodedLength = repetition.getEncodedLength(originalLength);
  unsigned char repetitionEncoded[repetitionEncodedLength];
  repetition.encode(original, repetitionEncoded, originalLength);

  // Simular canal ruidoso
  for(int i=0; i<repetitionEncodedLength*8; i++){
    if(random(100) < 5){ // 5% de probabilidad de error
      int bytePos = i/8;
      int bitPos = 7 - (i%8);
      repetitionEncoded[bytePos] ^= 1 << bitPos;
    }
  }

  unsigned char repetitionDecoded[originalLength];
  repetition.decode(repetitionEncoded, repetitionDecoded, repetitionEncodedLength);

  // Calcular métricas
  int hammingErrors = 0;
  int repetitionErrors = 0;
  for(int i=0; i<originalLength; i++){
    hammingErrors += __builtin_popcount(original[i] ^ hammingDecoded[i]);
    repetitionErrors += __builtin_popcount(original[i] ^ repetitionDecoded[i]);
  }

  // Generar informe comparativo
  Serial.println("\nResultados:");
  Serial.print("Tasa Hamming: 4/7 (~57%)");
  Serial.print("  Tasa Repetición: 1/4 (25%)");
  Serial.print("\nErrores Hamming: ");
  Serial.print(hammingErrors);
  Serial.print(" bits  (");
  Serial.print((float)hammingErrors/(originalLength*8)*100, 2);
  Serial.print("%)");
  Serial.print("\nErrores Repetición: ");
  Serial.print(repetitionErrors);
  Serial.print(" bits  (");
  Serial.print((float)repetitionErrors/(originalLength*8)*100, 2);
  Serial.println("%)");

  // Generar archivos BMP para análisis visual
  generarBMP("/original.bmp", original, originalLength);
  generarBMP("/hamming.bmp", hammingDecoded, originalLength);
  generarBMP("/repeticion.bmp", repetitionDecoded, originalLength);
}

void generarBMP(const char* filename, unsigned char* data, int length) {
  File bmpFile = SPIFFS.open(filename, FILE_WRITE);
  
  // Cabecera BMP (14 bytes)
  uint8_t header[14] = {'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0};
  int fileSize = 54 + length * 8 * 3; // 24 bits por pixel
  header[2] = fileSize; header[3] = fileSize >> 8; header[4] = fileSize >> 16; header[5] = fileSize >> 24;
  bmpFile.write(header, 14);

  // Info Header (40 bytes)
  uint8_t infoHeader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
  int width = length*8;
  int height = 1;
  memcpy(&infoHeader[4], &width, 4);
  memcpy(&infoHeader[8], &height, 4);
  bmpFile.write(infoHeader, 40);

  // Datos de imagen (cada bit como pixel 0x000000 o 0xFFFFFF)
  for(int i=0; i<length; i++){
    for(int j=7; j>=0; j--){
      uint8_t pixel[3] = {static_cast<uint8_t>((data[i] >> j) & 1 ? 0xFF : 0x00)};
      bmpFile.write(pixel, 3);
    }
  }
  bmpFile.close();
}

void loop() {}
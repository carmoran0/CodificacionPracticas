/* Plantilla proyectos arduino */
#include "SPIFFS.h"
#include <ctype.h>

void processFile();

void setup(){
    Serial.begin(115200);
    if(!SPIFFS.begin(true)){
        Serial.println("Error montando SPIFFS");
        return;
    }
    
    // Listar archivos en SPIFFS
    File root = SPIFFS.open("/");
    Serial.println("Contenido SPIFFS:");
    while(File file = root.openNextFile()){
        Serial.print("  ");
        Serial.println(file.name());
    }
    root.close();
    
    processFile();
}

void loop(){}

void processFile(){
    File input = SPIFFS.open("/data/texto.txt");
    if(!input){
        Serial.println("Error abriendo archivo: /data/texto.txt");
        return;
    }
    Serial.print("Tamaño archivo: ");
    Serial.print(input.size());
    Serial.println(" bytes");

    long counts[26] = {0};
    long total = 0;
    
    while(input.available()){
        char c = input.read();
        if(isalpha(c)){ // Corregir nombre función
    Serial.print("Carácter válido detectado: "); Serial.println(c);
            c = toupper(c);
            counts[c - 'A']++;
            total++;
        }
    }
    
    if(total == 0){
        Serial.println("Archivo contiene " + String(input.size()) + " bytes");
        Serial.println("Caracteres válidos detectados: 0");
        Serial.println("Posible causa: Archivo vacío o formato incorrecto");
        input.close();
        return;
    }
    input.close();


    File output = SPIFFS.open("/data/resultados.txt", FILE_WRITE);
    if(!output){
        Serial.println("Error creando archivo de resultados");
        return;
    }

    for(int i = 0; i < 26; i++){
        float porcentaje = counts[i] * 1.0f / total;
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "%c: %.4f\n", 'A' + i, porcentaje);
        output.print(buffer);
    }
    output.close();
    Serial.println("Análisis completado. Resultados guardados en /data/resultados.txt");
}

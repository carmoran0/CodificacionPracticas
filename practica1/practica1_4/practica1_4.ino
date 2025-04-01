/*
4. Use el Arduino como un generador de una señal sinusoidal F*sen(w*t) donde F es
el dígito F de su NIA en voltios y w = 1/C rad/s frecuencia de la señal sinusoidal. 
Para representar la salida utilice SerialPlotter de Arduino. https://www.luisllamas.es/arduino-serial-plotter/  
*/

#include <math.h>

const float F = 903699.0;
const float C = 1.0; 
const float w = 1.0 / C;

void setup() {
  Serial.begin(115200); 
  delay(20);
}

void loop() {
  static unsigned long tiempopasa = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - tiempopasa >= 1) {
  tiempopasa = currentTime;
    
    float t = currentTime / 1000.0; 
    float signal = F * sin(w * t); 
    
    Serial.println(signal);
  }
}

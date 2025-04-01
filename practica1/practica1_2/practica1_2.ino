#include "driver/ledc.h"

#define BLUELED 44
#define GREENLED 43
#define LEDCHANNELBLUE 0
#define LEDCHANNELGREEN 1
#define FREQUENCY 5000
#define RESOLUTION 8



void setup() {
  // put your setup code here, to run once:
  ledcSetup(LEDCHANNELBLUE, FREQUENCY, RESOLUTION);
  ledcSetup(LEDCHANNELGREEN, FREQUENCY, RESOLUTION);
  ledcAttachPin(BLUELED, LEDCHANNELBLUE);
  ledcAttachPin(GREENLED, LEDCHANNELGREEN);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int dutyCycle=0; dutyCycle<=255; dutyCycle++)
  {
    ledcWrite(LEDCHANNELBLUE, dutyCycle);
    ledcWrite(LEDCHANNELGREEN,255-dutyCycle);
    delay(10);
  }

  
  for (int dutyCycle=255; dutyCycle>=0; dutyCycle--)
  {
    ledcWrite(LEDCHANNELBLUE, dutyCycle);
    ledcWrite(LEDCHANNELGREEN,255-dutyCycle);
    delay(10);
  }
}

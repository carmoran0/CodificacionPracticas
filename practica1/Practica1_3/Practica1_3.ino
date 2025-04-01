/*
La función random devuelve un valor aleatorio
https://programarfacil.com/blog/random-en-arduino-como-usar-numeros-aleatorios/ ,
haga una llamada de random para que le devuelva un valor de 0 a 10 y si el valor
devuelto es mayor que 5 incremente la intensidad del led y si es menor que 5
decremente esa intensidad. Tenga en cuenta que la intensidad aplicada al led con la
función analogWrite o ledc no puede ser ni superior a 255 ni inferior a 0. Ajuste el valor
en que se incrementa / decrementa de la intensidad del led y el tiempo transcurrido
entre dos llamadas a random para conseguir cambios de iluminación continuos y no
demasiado rápidos. Si tiene un led tricolor, aplique el algoritmo a cada una de los tres
leds para obtener una lámpara relajante que vaya modulando su color e intensidad.
Los leds externos deben colocarse con una resistencia en serie de 220 ohm
*/

int R=random(0,256);
int G=random(0,256);
int B=random(0,256);
int randomFlag, ttw, ttd, iR, iG, iB, mult, counter=0;

void setup() {
  // put your setup code here, to run once:

}

void loop() {

  if (millis()-counter>=ttw)
  {
    ttw = random(1,3)*1000/random(1,4);
    ttd = random(2,6);
  
    if (random(1,11) > 5)
      mult=1;
    else
      mult=-1;
  
    iR = random(0,3);
    iG = random(0,3);
    iB = random(0,3);  

    counter = millis();
  }

  R+=iR*mult;
  G+=iG*mult;
  B+=iB*mult;
  
  R=constrain(R,20,255);
  G=constrain(G,20,255);
  B=constrain(B,20,255);

  neopixelWrite(RGB_BUILTIN,R,G,B);
  
  delay(ttd);
}

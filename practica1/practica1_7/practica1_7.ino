/*
7. Diseñe un programa al que introduzca una lista de números y devuelva la suma de
todos ellos, la lista es de longitud variable.
Entrada: 1 34 56 90 47
Salida: 228
*/
/* IO with Serial clases */
#define MAXBUF 20
#define MAXVECT 10
char buffer[MAXBUF];
boolean myReadString(int timeout);
void setup() {
  Serial.begin(9600);
  //Serial.printf("Introduce una lista de numeros a sumar separados por :\n");
  Serial.print("Entrada:\n");
}
void loop(){
  int v[MAXVECT]; 
  if(myReadString(50)){
    char *p;
    int j=0;
    p=strtok(buffer," "); //Divide the string in numbers tokenized by el espacio
    while((p!=NULL)&&(j<MAXVECT)){ //This mark the end of string
      v[j++]=atoi(p); //Convert string to int
      p=strtok(NULL," "); //Try to found a new value
    }
    if(j>0){ //Found numbers to add
      int result = 0;
      for(int i=0; i<j;i++){
        result += v[i];
      }
      //Serial.printf("El resultado de la suma es %d\n", result);
      //Serial.printf("Intenta otro grupo de numeros\n");
      Serial.print("Salida:");
      Serial.println(result);
      Serial.print("Entrada:\n");
    }
  }
} 
//Working on Arduino Uno
boolean myReadString(int timeout) { //ReadString with timeout limit, return true if a string is readed within timeout limits 
  static int i=0; //Número de caracteres leidos
  static long tLimit; //Reloj interno de timeout
  static boolean reading=false; //Marca si hay una lectura en curso
  while((Serial.available()>0) && (i<(MAXBUF-1))){ //Hay caracteres disponibles y no hemos desbordado el buffer
    if(!reading){ //First character
      reading=true; //Estamos en un proceso de lectura
      tLimit = millis(); //Ajustamos el reloj de timeout
      i=0; //Iniciamos el índice del buffer
    }
    buffer[i]=Serial.read(); //read next caracter from buffer
    //Serial.print(buffer[i]);
    if((buffer[i]=='\n') || (buffer[i]=='\r')){ //Fin de mensaje
      reading = false; //Fin de lectura
      if(i>0){ //No se trata de una linea vacía
        buffer[i] = 0; //Pon la marca de fin de cadena
        //Serial.print("Read "); //Depuración
        //Serial.print(i+1);
        //Serial.print(" characters\n");
        return true; //Lectura exitosa
      }
    }
    i=i+1;
    if(millis()-tLimit > timeout){ //Superamos el timeout de la lectura 86uS por caracter a 115200
      Serial.print("TimeOut"); //Mensaje de error
      reading = false; //Fin de lectura
      return false; //Fallo en la lectura
    }
  }
  return false;
}

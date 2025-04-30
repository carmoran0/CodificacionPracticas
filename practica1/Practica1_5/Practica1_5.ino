#define BLUELED 44
#define GREENLED 43


const int E = 3, C = 4;

int serialIndex, i;
char serialBuffer[200];

void setup() 
{
  digitalWrite(GREENLED, HIGH);
  Serial.begin(115200);
  Serial.print("BEGIN");
  delay(20);
}

void loop() 
{
  i=0;
  serialBuffer[200]={};
  Serial.print("AA");
  digitalWrite(GREENLED, HIGH);
  
  while(Serial.available())
  {
    serialBuffer[i]=Serial.read();       
    delay(10);
    
    if (serialBuffer[i] == '\n' || serialBuffer[i] == '\r')
    {
      digitalWrite(GREENLED, LOW);
      serialBuffer[i]='\0';
      break;
    }
    i++;

    if (i>=200)
      i=0;
  }

  for (int j=0; j<200;j++)
  {
    Serial.print(serialBuffer[j]);
  }
  Serial.print('\n');
}

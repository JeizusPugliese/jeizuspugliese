#include<LiquidCrystal.h>

int sensor = A0 ;
int ad1 = 10; //pin para motor
int at1= 9; //pin para motor
const int boton = 13;
int val=LOW;
int old_val=0;
int state=0;
LiquidCrystal lcd(8, 7, 6, 5, 4, 3);

String Texto1 ="";
String Texto2 ="";
String Texto3 ="";
String Texto4 ="";
String luz="";

int sensorluz=0;
int umbral=200;

int vel = 0;

int sensorpir = 2;
int speaker=12;
String alarma ="ON";
int stadopir=0;

void setup()
{
Serial.begin(90000);

pinMode(11,OUTPUT);  
pinMode(boton,INPUT);  
lcd.begin(16, 2);

  
//pinMode para Motor
  
pinMode(ad1, OUTPUT);//configuramos pines de salida del Arduino un par por
pinMode(at1, OUTPUT);//motor
digitalWrite(ad1,LOW);// inicializamos los dos motores, es decir están detenidos
digitalWrite(at1,LOW);//
 
  
pinMode(sensorpir, INPUT);
pinMode(speaker, OUTPUT); 
  
}


void loop()
{
  
  
val= digitalRead(boton);
 
  
if ((val == HIGH) && (old_val == LOW)){
state=1-state;

}
old_val = val; // valor del antiguo estado
if (state==1){
 
 alarma="ON";
  
 if (digitalRead(sensorpir)==HIGH)
  {
 stadopir=1;
  }else{
    stadopir=0;
  }
    
  if(stadopir==1 && alarma=="ON")
    
  {
    
      delay(500);
    tone(speaker,250,500);
    delay(500);
    tone(speaker,500,500);
    delay(500);
    tone(speaker,250,500);
    delay(500);
    tone(speaker,500,500);
    delay(500);
    tone(speaker,250,500);
    delay(500);
    tone(speaker,500,500);
    delay(500);
    tone(speaker,250,500);
    delay(500);
    tone(speaker,500,500);
    delay(3000);
 
  }
     
  
}
else{
 
 alarma="OFF";
}
  
  
sensorluz=analogRead(A1);
  
  
  
 if (sensorluz > umbral){
    digitalWrite(11, HIGH);
   luz="OFF";
  } else{
    digitalWrite(11, LOW);
   luz="ON";
  }  
  
  
    
  
  
  
//VARIABLES PARA MOTOR

int speed = map(vel, 0,100,0,255); 
  
  
//CODIGO PARA SENSOR TEMPERATURA
int lectura = analogRead(sensor);
float voltaje = 5.0 /1024 * lectura ; // Voltaje
  
int temp = voltaje * 100 -50 ;
  
if (temp <10)
{  
vel=0;
}
  
if (temp >=10 && temp<=25)
{
 vel=25; 
}
  
if (temp >25 && temp<=34)
{
 vel=50; 
}
  
if (temp >34 && temp<=40)
{
 vel=75; 
}
  
if (temp >40)
{
 vel=100; 
}
  
  
//CODIGO PARA LCD
  
 
  
  
analogWrite(ad1, speed);

  
}


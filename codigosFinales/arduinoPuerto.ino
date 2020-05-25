#include <Servo.h>

Servo mi_servo;

double Velocidad;

int sensorDer;
int sensorIzq;

int ena=10;
int in1=9;    /*der*/
int in2=8;    /*der*/
int in3=6;    /*izq*/
int in4=7;    /*izq*/
int enb=5;

void avanzar();
void pasilloDetectado();
void girarDer();
void girarIzq();
void retroceder();

int pasilloDeseado();
void volverInicio(int);

void setup() {
  pinMode(ena, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enb, OUTPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  mi_servo.attach(9);
  Velocidad = 50;
  Serial.begin(9600);
}

void loop() {
  int posicion;
  posicion = pasilloDeseado();
  volverInicio(posicion);
}

void avanzar(){
    digitalWrite(in3,LOW);
    digitalWrite(in4,HIGH);
    analogWrite(enb,(Velocidad));
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
    analogWrite(ena,(Velocidad));
}

void girarDer(){
    digitalWrite(in3,LOW);
    digitalWrite(in4,HIGH);
    analogWrite(enb,(Velocidad));
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
    analogWrite(ena,(0));
}

void girarIzq(){
  digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
    analogWrite(ena,(Velocidad));
    digitalWrite(in3,LOW);
    digitalWrite(in4,HIGH);
    analogWrite(enb,(0));
}

void pasilloDetectado(){
    digitalWrite(in3,LOW);
    digitalWrite(in4,HIGH);
    analogWrite(enb,(0));
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
    analogWrite(ena,(0));
    delay(1000);
    mi_servo.write(180);
    delay(1000);
    mi_servo.write(0);
}

void retroceder(){
    digitalWrite(in4,LOW);
    digitalWrite(in3,HIGH);
    analogWrite(enb,(Velocidad));
    digitalWrite(in2,HIGH);
    digitalWrite(in1,LOW);
    analogWrite(ena,(Velocidad));
}

int pasilloDeseado(){
   int pasillo=0, contador;
   contador=0;
  while(!Serial.available());
    contador--;
    pasillo = Serial.read();
   
    while(contador != pasillo){
      sensorDer = digitalRead(11);                   /*cable blanco*/
      sensorIzq = digitalRead(12);                   /*cable verde*/
      if(sensorDer == 0 && sensorIzq == 0){          //no detectan nada empieza a moverse
          avanzar();
      }else if(sensorDer == 1 && sensorIzq == 0 && contador != pasillo){        //detecta derecha gira izquierda
          delay(1000);        //delay para evitar fallos de lectura y que el robot no gire demasiado
          if(sensorDer == 1 && sensorIzq == 0){
            girarDer();
          }
      }else if(sensorDer == 0 && sensorIzq == 1 && contador != pasillo){        //detecta izq gira der
          delay(1000);        //delay para evitar fallos de lectura y que el robot no gire demasiado
          if(sensorDer == 0 && sensorIzq == 1){
            girarIzq();
          }
      }else if(sensorDer == 1 && sensorIzq == 1 && contador != pasillo){        //detecta que ha llegado pasillo, como no es el pasillo que queremos continua
          contador++;
          delay(550);       //delay para evitar que el contador aumente de forma excesiva
          avanzar();
          delay(550);
          if(contador == pasillo){          //si llega al pasillo deseado se para
          pasilloDetectado();
  }
      }
  }
  return contador;
}


void volverInicio(int pasillo){
    int contador, inicio = -1;
    contador = pasillo;

    while(contador != inicio){
      sensorDer = digitalRead(11);                   /*cable blanco*/
      sensorIzq = digitalRead(12);                   /*cable verde*/
      if(sensorDer == 0 && sensorIzq == 0){          //no detectan nada empieza a moverse
          retroceder();
      }else if(sensorDer == 0 && sensorIzq == 1 && contador != inicio){        //detecta derecha gira izquierda
          delay(700);        //delay para evitar fallos de lectura y que el robot no gire demasiado
          if(sensorDer == 0 && sensorIzq == 1){
            girarDer();
          }
      }else if(sensorDer == 1 && sensorIzq == 0 && contador != inicio){        //detecta izq gira der
          delay(700);        //delay para evitar fallos de lectura y que el robot no gire demasiado
          if(sensorDer == 1 && sensorIzq == 0){
            girarIzq();
          }
      }else if(sensorDer == 1 && sensorIzq == 1 && contador != inicio){        //detecta que ha llegado pasillo, como no es el pasillo que queremos continua
          contador--;
          delay(550);       //delay para evitar que el contador aumente de forma excesiva
          avanzar();
          if(contador == inicio){          //si llega al pasillo deseado se para
          pasilloDetectado();
          Serial.write(pasillo);   //manda el contador a C para decir que ha acabado
  }
  }
  }
}

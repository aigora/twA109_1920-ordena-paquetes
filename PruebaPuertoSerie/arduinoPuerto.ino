double Velocidad;

boolean sensorDer;
boolean sensorIzq;

int l298n_ena=10;
int l298n_in1=9;   /*der*/
int l298n_in2=8;    /*der*/
int l298n_in3=6;    /*izq*/
int l298n_in4=7;    /*izq*/
int l298n_enb=5;

void setup() {
  // put your setup code here, to run once:
pinMode(10, OUTPUT);
pinMode(9, OUTPUT);
pinMode(8, OUTPUT);
pinMode(6, OUTPUT);
pinMode(7, OUTPUT);
pinMode(5, OUTPUT);
pinMode(11, INPUT);
pinMode(12, INPUT);
  Velocidad = 110;

}

void loop() {
  int pasillo, contador=0;
  sensorDer = digitalRead(11);                   /*cable blanco*/
  sensorIzq = digitalRead(12);                   /*cable verde*/

  while(!Serial.available());
  pasillo = Serial.read();
  
  do{
  if(((!sensorDer) && (!sensorIzq))){           /*no detectan nada empieza a moverse*/
    digitalWrite(l298n_in3,LOW);
    digitalWrite(l298n_in4,HIGH);
    analogWrite(l298n_enb,(Velocidad));
    digitalWrite(l298n_in1,LOW);
    digitalWrite(l298n_in2,HIGH);
    analogWrite(l298n_ena,(Velocidad));
  }else if((sensorDer && (!sensorIzq))){        /*detecta derecha gira izquierda*/ 
    digitalWrite(l298n_in3,LOW);
    digitalWrite(l298n_in4,HIGH);
    analogWrite(l298n_enb,(Velocidad));
    digitalWrite(l298n_in1,LOW);
    digitalWrite(l298n_in2,HIGH);
    analogWrite(l298n_ena,(0));
  }else if(((!sensorDer) && sensorIzq)){        /*detecta izq gira der*/
    digitalWrite(l298n_in1,LOW);
    digitalWrite(l298n_in2,HIGH);
    analogWrite(l298n_ena,(Velocidad));
    digitalWrite(l298n_in3,LOW);
    digitalWrite(l298n_in4,HIGH);
    analogWrite(l298n_enb,(0));
  }else if((sensorDer && sensorIzq)){        /*detecta ambos ha llegado pasillo*/
    digitalWrite(l298n_in3,LOW);
    digitalWrite(l298n_in4,HIGH);
    analogWrite(l298n_enb,(0));
    digitalWrite(l298n_in1,LOW);
    digitalWrite(l298n_in2,HIGH);
    analogWrite(l298n_ena,(0));
    contador++;
    }
  }while(contador != pasillo);
  
  Serial.write(contador);
  }

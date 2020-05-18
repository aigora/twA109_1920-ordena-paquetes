double Velocidad;

boolean b_sensorDerecho;
boolean b_sensorIzquierdo;

int l298n_ena=10;
int l298n_in1=9;
int l298n_in2=8;
int l298n_in3=6;
int l298n_in4=7;
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
    b_sensorDerecho = digitalRead(11);
    b_sensorIzquierdo = digitalRead(12);
    if ((b_sensorDerecho && (!b_sensorIzquierdo))) 
{ 
      digitalWrite(l298n_in3,LOW);
      digitalWrite(l298n_in4,HIGH);
      analogWrite(l298n_enb,(Velocidad));
      digitalWrite(l298n_in1,LOW);
      digitalWrite(l298n_in2,HIGH);
      analogWrite(l298n_ena,(0));

    }
    if (((!b_sensorDerecho) && b_sensorIzquierdo)) 
{
      digitalWrite(l298n_in1,LOW);
      digitalWrite(l298n_in2,HIGH);
      analogWrite(l298n_ena,(Velocidad));
      digitalWrite(l298n_in3,LOW);
      digitalWrite(l298n_in4,HIGH);
      analogWrite(l298n_enb,(0));

    }
    if (((!b_sensorDerecho) && (!b_sensorIzquierdo))) {
      digitalWrite(l298n_in3,LOW);
      digitalWrite(l298n_in4,HIGH);
      analogWrite(l298n_enb,(0));
      digitalWrite(l298n_in1,LOW);
      digitalWrite(l298n_in2,HIGH);
      analogWrite(l298n_ena,(0));

    }
    if ((b_sensorDerecho && b_sensorIzquierdo)) {
      digitalWrite(l298n_in3,LOW);
      digitalWrite(l298n_in4,HIGH);
      analogWrite(l298n_enb,(Velocidad));
      digitalWrite(l298n_in1,LOW);
      digitalWrite(l298n_in2,HIGH);
      analogWrite(l298n_ena,(Velocidad));

    }


}

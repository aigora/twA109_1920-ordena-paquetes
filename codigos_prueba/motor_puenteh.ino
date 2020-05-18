int motor_IN1 = 7;
int motor_IN2 = 6;

void setup() {
  // inicializar la comunicación serial a 9600 bits por segundo:
  Serial.begin(9600);
  // configuramos los pines como salida
  pinMode(motor_IN1, OUTPUT);
  pinMode(motor_IN2, OUTPUT);
}

void loop() {
  
  gito_horario();
  delay(1000);
  
  giro_antihorario();
  delay(1000);
  
  parar();
  delay(1000);
}

void giro_horario()
{
  digitalWrite (motor_IN1, HIGH);
  digitalWrite (motor_IN2, LOW);
}
void gito_antihorario()
{
  digitalWrite (motor_IN1, LOW);
  digitalWrite (motor_IN2, HIGH);
}

void parar()
{
  digitalWrite (motor_IN1, LOW);
  digitalWrite (motor_IN2, LOW);
}

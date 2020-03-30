#include <Servo.h>

Servo mi_servo;
 
void setup() {
  Serial.begin(9600);
  mi_servo.attach(9);
}
 
void loop() {
  mi_servo.write(0);
  delay(1000);
  mi_servo.write(180);
  // Esperamos 1 segundo
  delay(1000);
}

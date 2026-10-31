#include <Servo.h>

Servo couvercle;
const int PIN_SERVO = 13;
const int PIN_TRIG = A2;
const int PIN_ECHO = A3;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  couvercle.attach(PIN_SERVO);
  couvercle.write(0);
}

void loop() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  long duree = pulseIn(PIN_ECHO, HIGH, 12000);
  int cm = (duree == 0) ? 999 : duree / 58;
  Serial.print(cm);
  Serial.println(" cm");

  if (cm < 25) couvercle.write(90);   // main proche : ouvert
  else couvercle.write(0);            // sinon : ferme
  delay(200);
}

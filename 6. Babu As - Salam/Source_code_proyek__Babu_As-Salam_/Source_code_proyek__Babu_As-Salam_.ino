#include <Servo.h>
Servo servo;

void setup() {
  Serial.begin(9600);
  servo.attach(7);
}

void loop() {
  if(Serial.available()>0){
    char data = Serial.read();
    switch(data){
      case '0':
        servo.write(90);
        delay(3000);
        servo.write(180);
        break;
      default:
        break;
    }
  }
}

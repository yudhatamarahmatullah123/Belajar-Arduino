//Servo Sweep

#include <Servo.h>
Servo myservo; //definisikan variabel myservo sebagai servo
int pos = 0; //variabel untuk memposisikan servo

void setup() {
  myservo.attach(9); //pin servo berada di pin 9 arduino
}

void loop() {
  //mulai dari step 0 derajat sampai step 180 derajat, setiap step bertambah 1
  for (pos = 0; pos <= 180; pos += 1) {
    //tulis nilai variable 'pos' pada servo
    myservo.write(pos);
    delay(15);
  }

  //mulai dari step 180 derajat sampai step 0 derajat, setiap step berkurang 1
  for (pos = 180; pos >= 0; pos -= 1) {
    //tulis nilai variable 'pos' pada servo
    myservo.write(pos);
    delay(15);
  }
}

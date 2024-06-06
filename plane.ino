#include <Servo.h>


Servo myservo;  // create servo object to control a servo


void setup() {
  myservo.attach(5);

  myservo.writeMicroseconds(750);
  Serial.println("low");
  delay(10000);
  for(int i = 1000; i < 2000; i = i + 50){
    delay(250);
    myservo.writeMicroseconds(i);
  }
}


void loop() {
}

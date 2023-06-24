/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-servo-motor
 */

#include <ESP32Servo.h>
#include <BlynkSimpleEsp32.h>

#define SERVO_PIN1 2 // ESP32 pin GIOP26 connected to servo motor
#define SERVO_PIN2 4 
#define SERVO_PIN3 15 
Servo servoMotor1;
Servo servoMotor2;
Servo servoMotor3;
int timerIdOpenServo1,timerIdOpenServo2,timerIdOpenServo3;
int timerIdCloseServo1,timerIdCloseServo2,timerIdCloseServo3;
int pos1=0,pos2=0,pos3=0;
BlynkTimer timer;

void setup() {
  servoMotor1.attach(SERVO_PIN1);
  servoMotor2.attach(SERVO_PIN2);
  servoMotor3.attach(SERVO_PIN3);
  timerIdOpenServo1=timer.setInterval(10L,openServo1);  
  timerIdOpenServo2=timer.setInterval(10L,openServo2);
  timerIdOpenServo3=timer.setInterval(10L,openServo3);
  timerIdCloseServo1=timer.setInterval(10L,closeServo1);
  timer.toggle(timerIdCloseServo1);  
  timerIdCloseServo2=timer.setInterval(10L,closeServo2);
  timer.toggle(timerIdCloseServo2);  
  timerIdCloseServo3=timer.setInterval(10L,closeServo3);
  timer.toggle(timerIdCloseServo3); 
    // attaches the servo on ESP32 pin
}

void openServo1(){
  servoMotor1.write(pos1+1);
  pos1++;
  if(pos1==100){
    timer.toggle(timerIdOpenServo1); 
    timer.toggle(timerIdCloseServo1);   
  }
}
void closeServo1(){
  servoMotor1.write(pos1-1);
  pos1--;
  if(pos1==0){
    timer.toggle(timerIdOpenServo1); 
    timer.toggle(timerIdCloseServo1);   
  }
}
void openServo2(){
  servoMotor2.write(pos2+1);
  pos2++;
  if(pos2==100){
    timer.toggle(timerIdOpenServo2); 
    timer.toggle(timerIdCloseServo2);   
  }
}
void closeServo2(){
  servoMotor2.write(pos2-1);
  pos2--;
  if(pos2==0){
    timer.toggle(timerIdOpenServo2); 
    timer.toggle(timerIdCloseServo2);   
  }
}
void openServo3(){
  servoMotor3.write(pos3+1);
  pos3++;
  if(pos3==100){
    timer.toggle(timerIdOpenServo3); 
    timer.toggle(timerIdCloseServo3);   
  }
}
void closeServo3(){
  servoMotor3.write(pos3-1);
  pos3--;
  if(pos3==0){
    timer.toggle(timerIdOpenServo3); 
    timer.toggle(timerIdCloseServo3);   
  }
}
void loop() {
  timer.run();
  // rotates from 0 degrees to 180 degrees
//   for (int pos = 0; pos <= 100; pos += 1) {
//     // in steps of 1 degree
//     servoMotor1.write(pos);
//     delay(10); // waits 15ms to reach the position
//   }

//   // rotates from 180 degrees to 0 degrees
//   for (int pos = 100; pos >= 0; pos -= 1) {
//     servoMotor1.write(pos);
//     delay(10); // waits 15ms to reach the position
//   }

//  for (int pos = 0; pos <= 100; pos += 1) {
//     // in steps of 1 degree
//     servoMotor2.write(pos);
//     delay(10); // waits 15ms to reach the position
//   }

//   // rotates from 180 degrees to 0 degrees
//   for (int pos = 100; pos >= 0; pos -= 1) {
//     servoMotor2.write(pos);
//     delay(10); // waits 15ms to reach the position
//   }

// for (int pos = 0; pos <= 100; pos += 1) {
//     // in steps of 1 degree
//     servoMotor3.write(pos);
//     delay(10); // waits 15ms to reach the position
//   }

//   // rotates from 180 degrees to 0 degrees
//   for (int pos = 100; pos >= 0; pos -= 1) {
//     servoMotor3.write(pos);
//     delay(10); // waits 15ms to reach the position
//   }

  

  
  
}

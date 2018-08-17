#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <MeAuriga.h>

//motors
MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);

//sensors used,buzzer and LED declaration
MeRGBLed rgbled_0(0, 12);
MeSoundSensor soundsensor_14(14);
MeLightSensor lightsensor_12(12);
MeBuzzer buzzer;
MeUltrasonicSensor ultrasonic_10(10);

//angles
double angle_rad = PI / 180.0;
double angle_deg = 180.0 / PI;
double currentTime = 0;
double lastTime = 0;


//setting up the motors
void isr_process_encoder1(void)
{
  if (digitalRead(Encoder_1.getPortB()) == 0) {
    Encoder_1.pulsePosMinus();
  }
  else {
    Encoder_1.pulsePosPlus();
  }
}

void isr_process_encoder2(void)
{
  if (digitalRead(Encoder_2.getPortB()) == 0) {
    Encoder_2.pulsePosMinus();
  }
  else {
    Encoder_2.pulsePosPlus();
  }
}


//directions of movements 
void move(int direction, int speed)
{
  int leftSpeed = 0;
  int rightSpeed = 0;
  if (direction == 1) {
    leftSpeed = -speed;
    rightSpeed = speed;
  } else if (direction == 2) {
    leftSpeed = speed;
    rightSpeed = -speed;
  } else if (direction == 3) {
    leftSpeed = -speed;
    rightSpeed = -speed;
  } else if (direction == 4) {
    leftSpeed = speed;
    rightSpeed = speed;
  }
  Encoder_1.setTarPWM(leftSpeed);
  Encoder_2.setTarPWM(rightSpeed);
}


//coordinating the encoders
void moveDegrees(int direction, long degrees, int speed_temp)
{
  speed_temp = abs(speed_temp);
  if (direction == 1)
  {
    Encoder_1.move(-degrees, (float)speed_temp);
    Encoder_2.move(degrees, (float)speed_temp);
  }
  else if (direction == 2)
  {
    Encoder_1.move(degrees, (float)speed_temp);
    Encoder_2.move(-degrees, (float)speed_temp);
  }
  else if (direction == 3)
  {
    Encoder_1.move(-degrees, (float)speed_temp);
    Encoder_2.move(-degrees, (float)speed_temp);
  }
  else if (direction == 4)
  {
    Encoder_1.move(degrees, (float)speed_temp);
    Encoder_2.move(degrees, (float)speed_temp);
  }
}

//BASIC MOVEMENTS
void turnRight()
{
  lastTime = millis() / 1000.0;
  while (!((getLastTime()) > (0.6)))
  {
    _loop();
    move(4, 100);
  }

}
void turnLeft()
{
  lastTime = millis() / 1000.0;
  while (!((getLastTime()) > (0.6)))
  {
    _loop();
    move(3, 100);
  }

}

void stopMethod()
{
  lastTime = millis() / 1000.0;

  while (!((getLastTime()) > (0.3)))
  {
    _loop();
    move(1, 0);
  }

}

void moveBackward() {
  while (!((ultrasonic_10.distanceCm()) < (10)))
  {
    _loop();
    move(2, 100);
  }
}

void moveForward()
{

  while (!((ultrasonic_10.distanceCm()) < (45)))
  {
    _loop();
    move(1, 100);
    if (ultrasonic_10.distanceCm() < 10) {
      moveBackward();
    }
  }
}


double theInitialSound=0;
double arrayOfSounds[100];
char val;
bool nav= true;

double arrayOfDistances[3];

void turnRight();
void turnLeft();
void stopMethod();
void moveForward();
double getSound();

void _loop() {
  Encoder_1.loop();
  Encoder_2.loop();


}

int compareSoundsToInitial(){
   int counter=0;
   for (int i = 0; i < 100; i++)
  {
    if ((arrayOfSounds[i]) > (theInitialSound + 2.0))
    {
      counter = counter + 1;
      //Serial.println(counter);
    }
  }
  return counter;
}


void goLeft(){
    turnLeft();
    turnLeft();
    stopMethod();
    if ((arrayOfDistances[0]) < (35)) {
      turnLeft();
    }
    else {
      moveForward();
    }

    navigateHouse();
}

void goStraight(){
   turnLeft();
    stopMethod();
    if ((arrayOfDistances[1]) < (35)) {
      turnRight();
      turnRight();
    }
    else {
      moveForward();
    }
    // stopMethod();
    navigateHouse();
}

void goRight(){
   if ((arrayOfDistances[2]) < (35)) {
      turnRight();
    }
    else {
      moveForward();
    }
    //stopMethod();
    navigateHouse();
  }

void navigateHouse() {
  stopMethod();
  turnLeft();
  stopMethod();
  arrayOfDistances[0] = getDistance();
  turnRight();
  stopMethod();
  arrayOfDistances[1] = getDistance();
  turnRight();
  arrayOfDistances[2] = getDistance();
  stopMethod();
  
  if (arrayOfDistances[0] > arrayOfDistances[1] && arrayOfDistances[0] > arrayOfDistances[2]) {
      goLeft();
  }

  if (arrayOfDistances[1] > arrayOfDistances[0] && arrayOfDistances[1] < arrayOfDistances[2]) {
      goStraight();
  }

  if (arrayOfDistances[2] > arrayOfDistances[0] && arrayOfDistances[2] > arrayOfDistances[1]) {
      goRight();
}
}

void reactDaytime(){
  // buzzer.tone(262, 500);
Serial.begin(9600);
Serial.println("Hello");
navigateHouse();

}
   
  
void reactToSound(){
    //turn LED on at night
    if (((lightsensor_12.read()) == (0))) {
      rgbled_0.setColor(0, 150, 60, 255);
      rgbled_0.show();
      exit(0);
    }
    else {
     reactDaytime();
    }
}

void checkForSound() {
  if ( compareSoundsToInitial() > 25){
    reactToSound();
  }
  else{
    addNewSound();
  }
  
}

//method to calculate an average of sound measures
double getSound()
{
  double sum = 0;
  double sound = 0;
  for (int __i__ = 0; __i__ < 100; ++__i__) {
    sound = soundsensor_14.strength();
    sum = (sum) + (sound);
    sound = 0;
  }
  return sum/100;
}

void addNewSound(){
  
    for (int a = 99; a > 0; a--) {
      arrayOfSounds[a] = arrayOfSounds[a - 1];
    }
    arrayOfSounds[0] = getSound();
    delay(50); //!!!
}


double getDistance() {
  double sum = 0;
  for (int i = 0; i < 50; i++) {
    sum = sum + ultrasonic_10.distanceCm();
  }
  return sum / 50;
}



double getLastTime() {
  return currentTime = millis() / 1000.0 - lastTime;
}


void setup() {
  rgbled_0.setpin(44);
  buzzer.setpin(45);
  rgbled_0.setColor(0, 60, 20, 150);
  rgbled_0.show();
  rgbled_0.setColor(0, 0, 0, 0);
  rgbled_0.show();

  delay(100);

  //start the work of motors
  attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
  Encoder_1.setPulse(9);
  Encoder_1.setRatio(39.267);
  Encoder_1.setPosPid(1.8, 0, 1.2);
  Encoder_1.setSpeedPid(0.18, 0, 0);
  Encoder_2.setPulse(9);
  Encoder_2.setRatio(39.267);
  Encoder_2.setPosPid(1.8, 0, 1.2);
  Encoder_2.setSpeedPid(0.18, 0, 0);

  //Set Pwm 8KHz
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);
  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);

  while (theInitialSound < 163 || theInitialSound >= 165) {
    theInitialSound = getSound();
  //  Serial.println(theInitialSound);
  }
  for (int i = 0; i < 100; i++) {
    arrayOfSounds[i] = getSound();
    delay(70);
  }

}

void loop() {
   
  checkForSound();
}
  
void guard() {
  double initDistance = getDistance();
  delay(100);
  while (ultrasonic_10.distanceCm() > initDistance + 2) {
      buzzer.tone(262, 500);
  }
}

void _delay(float seconds) {
  long endTime = millis() + seconds * 1000;
  while (millis() < endTime)_loop();
}




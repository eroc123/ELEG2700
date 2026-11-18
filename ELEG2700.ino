/*
  ELEG2700 Lab 6_3
  Board: Arduino Nano
  Version: 202508

  Output: Servo1 (D6)
  Output: Servo2 (D9)
  Output: 7-segment (D10,D11)
  Output: motors (D3,D2), (D5,D4)
  Input: IR sensor long range (A5)
  Input: IR sensor left (A4)
  Input: IR sensor middle (A3)
  Input: IR sensor right (A2)
*/
#include "ELEG2700.h"
#include <Servo.h>
#define BLK 0
#define WHT 1


#define CLAMP_OPEN1 130
// #define CLAMP_RELEASE1 30
#define CLAMP_CLOSE1 15

#define CLAMP_OPEN2 10
// #define CLAMP_RELEASE2 30
#define CLAMP_CLOSE2 111

FSMClass FSM1;          //The given FSM is named 'FSM1'.
TM1637DisplayClass LEDDisplay(D10, D11); //7-segment LED Display is connected to D10,D11 for debug
ServoClass Servo1(D6);
ServoClass Servo2(D9);
SensorClass S1(A7);
SensorClass S2(A6);
SensorClass SFL(A2);                      // Front left
SensorClass SFC(A1);                      // Front centre
SensorClass SFR(A0);                      // Front right
SensorClass SBL(A5);                      // Back left
SensorClass SBC(A4);                      // Back centre
SensorClass SBR(A3);
MotorClass MotorR(D3,D2);
MotorClass MotorL(D5,D4);
bool cup = false;
bool first_turn = true;
int reverse = 1;
int speed_1 = 125; // for turning
int speed_2 = 240; // straight line speed
int counter = 0;
int modifier = 1;
// WHT = 1, BLK = 0. Returns a 3-bit pattern: bit2 = left, bit1 = centre, bit0 = right
int frontPattern() {
  return (SFL.getHiLow() << 2) | (SFC.getHiLow() << 1) | SFR.getHiLow();
}
int backPattern() {
  return (SBL.getHiLow() << 2) | (SBC.getHiLow() << 1) | SBR.getHiLow();
}
// int current_pos = LEFT_POS;
//===== Basically, no need to modify setup() and loop() ====
void setup()
{
  Serial.begin(115200);          //optional, for debug
  LEDDisplay.setBrightness(15); //optional
  FSM1.init(S_000);           // must have this line, you can change the first state of the FSM
  
  
  Servo1.init();
  Servo2.init();
  Servo1.setValue(CLAMP_OPEN1);
  Servo2.setValue(CLAMP_OPEN2);
  MotorR.setSpeed(0);
  MotorL.setSpeed(0);
  
}

void loop()
{
    FSM1.run(); //important, don't modify this line              
}
//================= Students add STATES below this line ====================
//----------start of state S_000 -----
void S_000()   
{
  if(FSM1.doTask())
  {
    MotorR.setSpeed(250);
    MotorL.setSpeed(250+5);
    
    LEDDisplay.setValue(0);
    cup = false;
  }
  if (SFL.getHiLow()== BLK || SFC.getHiLow()== BLK || SFR.getHiLow()== BLK) {
    // Servo1.setValue(REAR_POS);
    delay(100);
    Servo2.setValue(CLAMP_OPEN2);
    Servo1.setValue(CLAMP_OPEN1);
    delay(500); // stop at the start/end
    FSM1.transit(S_001); 
    speed_1 = 125; // for turning
    speed_2 = 240;
    cup = false;
  } 

}
void S_001()   
{
  if(FSM1.doTask())
  {
    MotorR.setSpeed(speed_2);
    MotorL.setSpeed(speed_2+5);
    LEDDisplay.setValue(1);
  }
  
  // if (SFR.getHiLow()== BLK && SFC.getHiLow()== BLK && SFL.getHiLow()== BLK)   FSM1.transit(S_008); 
        
  int p = (cup == false) ? frontPattern() : backPattern();

  switch (p) {
    case 0b000: FSM1.transit(S_008); break;   // BBB
    case 0b100: FSM1.transit(S_002); break;   // WBB
    case 0b001: FSM1.transit(S_003); break;   // BBW
    default: break;                           // 101, 111, 010, 110, 011: keep going straight
  }
  if (SBL.getHiLow()== WHT && SBC.getHiLow()== WHT && SBR.getHiLow()== WHT && SFL.getHiLow()== WHT && SFC.getHiLow()== WHT && SFR.getHiLow()== WHT) {
    delay(100);
    if (SBL.getHiLow()== WHT && SBC.getHiLow()== WHT && SBR.getHiLow()== WHT && SFL.getHiLow()== WHT && SFC.getHiLow()== WHT && SFR.getHiLow()== WHT) FSM1.transit(S_000);
  } 

  // if (SFL.getHiLow()== BLK || SFC.getHiLow()== BLK || SFR.getHiLow()== BLK)  FSM1.transit(S_001);      
  if (S1.getHiLow()== BLK && cup == false)  FSM1.transit(S_004);  
  if (S2.getHiLow()== BLK && cup == false)  FSM1.transit(S_004);
}
void S_008()   
{
  if(FSM1.doTask())
  {
    if (counter >= 0) {
      
      FSM1.transit(S_009);
      // current_pos = LEFT_POS;
    }
    // if (counter < 5) {
    //   MotorR.setSpeed(speed_2);
    //   MotorL.setSpeed(0);
    // }
    // if (counter >= 5 && counter < 11) {
    //   MotorR.setSpeed(0);
    //   MotorL.setSpeed(speed_2);
    //   // Servo1.setValue(RIGHT_POS);
    //   // modifier = -1;
    //   // current_pos = RIGHT_POS;
    // }
    if (cup == true) {
      counter += modifier;
    }
    

    LEDDisplay.setValue(8);
    
    
  }
  // delay(500);
  if (cup == false) {
    if (SFL.getHiLow()== WHT || SFR.getHiLow()== WHT)  FSM1.transit(S_001);
  }
  else {
    if (SBL.getHiLow()== WHT || SBR.getHiLow()== WHT)  FSM1.transit(S_001);
  }
  // if (SFL.getHiLow()== BLK && SFC.getHiLow()== WHT && SFR.getHiLow()== BLK)  FSM1.transit(S_001);
  // if (SFL.getHiLow()== BLK && SFC.getHiLow()== BLK && SFR.getHiLow()== BLK)  FSM1.transit(S_008);  

  // FSM1.transit(S_001);
}
void S_009()   // align to the line by pivoting in place
{
  if (FSM1.doTask())
  {
    LEDDisplay.setValue(9);
  }

  // Not carrying the cup: front sees BBB, so use the back sensors.
  // Carrying the cup the car moves the other way, so use the front sensors.

  switch (backPattern()) {
    case 0b101:                               // WBW: aligned
      MotorR.setSpeed(150);
      MotorL.setSpeed(150);
      FSM1.transit(S_001);                    // was S_001
      delay(500);
      break;
    case 0b100: 
      MotorR.setSpeed(-150);
      MotorL.setSpeed(-150);
      delay(1000);

      FSM1.transit(S_001);
      break;

    case 0b001:
      MotorR.setSpeed(-150);
      MotorL.setSpeed(-150);
      delay(1000);
      FSM1.transit(S_001);
      break;

    default: 
      MotorR.setSpeed(-150);
      MotorL.setSpeed(-150);
      delay(1000);
      FSM1.transit(S_001);
  }

  // if (millis() - t0 > 1500) {                 // give up so it can't spin forever
  //   MotorR.setSpeed(0);
  //   MotorL.setSpeed(0);
  //   FSM1.transit(S_001);
  // }
}

void S_002()   
{
  if(FSM1.doTask())
  {
    MotorR.setSpeed(-speed_1);
    MotorL.setSpeed(speed_2);

    LEDDisplay.setValue(2);
  }
  // if (SFL.getHiLow()== BLK && SFC.getHiLow()== BLK && SFR.getHiLow()== BLK)   FSM1.transit(S_008);  
  if (cup == false) {
    if (SFR.getHiLow()== BLK && SFC.getHiLow()== BLK && SFL.getHiLow()== BLK)   FSM1.transit(S_008); 
    // else if (SBL.getHiLow()== WHT && SBC.getHiLow()== BLK && SBR.getHiLow()== WHT) {
    //   MotorR.setSpeed(speed_1);
    //   MotorL.setSpeed(speed_1);
    //   delay(10);
    //   FSM1.transit(S_001);
    // }
    else if (SFL.getHiLow()== WHT && SFC.getHiLow()== BLK && SFR.getHiLow()== WHT)  FSM1.transit(S_001);
    else if (SFL.getHiLow()== BLK && SFC.getHiLow()== WHT && SFR.getHiLow()== BLK)  FSM1.transit(S_001);
    else if (SFL.getHiLow()== BLK && SFC.getHiLow()== WHT && SFR.getHiLow()== WHT)  FSM1.transit(S_003);
    
    if (S1.getHiLow()== BLK && cup == false)  FSM1.transit(S_004);  
    if (S2.getHiLow()== BLK && cup == false)  FSM1.transit(S_004);  
  }
  else {
    if (SBR.getHiLow()== BLK && SBC.getHiLow()== BLK && SBL.getHiLow()== BLK)   FSM1.transit(S_008); 
    // else if (SFL.getHiLow()== WHT && SFC.getHiLow()== BLK && SFR.getHiLow()== WHT) {
    //   MotorR.setSpeed(speed_1);
    //   MotorL.setSpeed(speed_1);
    //   delay(10);
    //   FSM1.transit(S_001);
    // }
    else if (SBL.getHiLow()== WHT && SBC.getHiLow()== BLK && SBR.getHiLow()== WHT)  FSM1.transit(S_001);
    else if (SBL.getHiLow()== BLK && SBC.getHiLow()== WHT && SBR.getHiLow()== BLK)  FSM1.transit(S_001);
    else if (SBL.getHiLow()== BLK && SBC.getHiLow()== WHT && SBR.getHiLow()== WHT)  FSM1.transit(S_003);
  }
  // FSM1.transit(S_001);
}
void S_003()   
{
  if(FSM1.doTask())
  {
    MotorR.setSpeed(speed_2);
    MotorL.setSpeed(-speed_1);

    LEDDisplay.setValue(3);
  }
  // if (SFL.getHiLow()== BLK && SFC.getHiLow()== BLK && SFR.getHiLow()== BLK)   FSM1.transit(S_008);  
  if (cup == false) {
    if (SFR.getHiLow()== BLK && SFC.getHiLow()== BLK && SFL.getHiLow()== BLK)   FSM1.transit(S_008); 
    // else if (SBL.getHiLow()== WHT && SBC.getHiLow()== BLK && SBR.getHiLow()== WHT) {
    //   MotorR.setSpeed(speed_1);
    //   MotorL.setSpeed(speed_1);
    //   delay(10);
    //   FSM1.transit(S_001);
    // }
    else if (SFL.getHiLow()== WHT && SFC.getHiLow()== BLK && SFR.getHiLow()== WHT)  FSM1.transit(S_001); 
    else if (SFL.getHiLow()== BLK && SFC.getHiLow()== WHT && SFR.getHiLow()== BLK)  FSM1.transit(S_001);
    else if (SFL.getHiLow()== WHT && SFC.getHiLow()== WHT && SFR.getHiLow()== BLK)  FSM1.transit(S_002); 

    if (S1.getHiLow()== BLK && cup == false)  FSM1.transit(S_004);  
    if (S2.getHiLow()== BLK && cup == false)  FSM1.transit(S_004);
  }  
  else {
    if (SBR.getHiLow()== BLK && SBC.getHiLow()== BLK && SBL.getHiLow()== BLK)   FSM1.transit(S_008); 
    // else if (SFL.getHiLow()== WHT && SFC.getHiLow()== BLK && SFR.getHiLow()== WHT) {
    //   MotorR.setSpeed(speed_1);
    //   MotorL.setSpeed(speed_1);
    //   delay(10);
    //   FSM1.transit(S_001);
    // }
    else if (SBL.getHiLow()== WHT && SBC.getHiLow()== BLK && SBR.getHiLow()== WHT)  FSM1.transit(S_001); 
    else if (SBL.getHiLow()== BLK && SBC.getHiLow()== WHT && SBR.getHiLow()== BLK)  FSM1.transit(S_001);
    else if (SBL.getHiLow()== WHT && SBC.getHiLow()== WHT && SBR.getHiLow()== BLK)  FSM1.transit(S_002); 
  }
  // FSM1.transit(S_001);
}
void S_004()   
{
  if(FSM1.doTask())
  {
    MotorR.setSpeed(0);
    MotorL.setSpeed(0);
    Servo2.setValue(CLAMP_CLOSE2);
    Servo1.setValue(CLAMP_CLOSE1);
    
    LEDDisplay.setValue(4);
  }
  cup = true;
  delay(2000);
  FSM1.transit(S_005); 
}
void S_005()   
{
  if(FSM1.doTask())
  {

    // Servo1.setValue(REAR_POS);
    LEDDisplay.setValue(5);
    speed_1 *= -1;
    speed_2 *= -1;
    cup = true;
  }
  FSM1.transit(S_001); 
}
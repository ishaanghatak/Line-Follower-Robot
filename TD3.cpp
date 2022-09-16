// ESP Group 46 - Technical Demonstration 3
// Author: Ishaan Ghatak

#include "QEI.h"
#include "C12832.h"
#include "mbed.h"

C12832 lcd(D11,D13,D12,D7,D10);
Timeout myTimeOut;
QEI lEn(PB_14, PB_13, NC, 256);  
QEI rEn(PB_15, PB_12, NC, 256);
DigitalOut enable(PB_2);
DigitalOut leftD(PD_2) , rightD(PC_12);
PwmOut left(PB_7);
PwmOut right(PA_15);
int sampleTime = 1;
volatile float leftPulses = 0;
float leftSpeed = 0;
float preLeftPulses = 0;

volatile float rightPulses = 0;
float rightSpeed = 0;
float preRightPulses = 0;
Ticker myTicker;

void getSpeed(){
   leftPulses  = lEn.getPulses();
        leftSpeed = (leftPulses - preLeftPulses) / 0.001 ;
        preLeftPulses = leftPulses;
    
        rightPulses = rEn.getPulses();
        rightSpeed = (rightPulses - preRightPulses) / sampleTime ;
        preRightPulses = rightPulses;
        

}
int main(){
    while(1){
        
    leftD = rightD = 0;
    enable = 1;
    float baseSpeed = 0.5;
    float period = 0.0001;
    left.period(period);
    right.period(period);
    left.write(baseSpeed);
    right.write(baseSpeed);
    
    lcd.locate(20,20);
    lcd.printf("left:%.2f    right:%.2f",leftSpeed,rightSpeed);
    }

}

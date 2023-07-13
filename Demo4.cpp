// Demonstration 4
// Author: Ishaan Ghatak

#include "QEI.h"
#include "C12832.h"
#include "mbed.h"

AnalogIn sensors[6] = {PA_4,PC_3,PC_2,PB_0,PC_1,PC_0};
DigitalOut leds[6] = {PA_13,PB_8,PC_6,PB_9,PC_8,PC_9};
PwmOut left(PB_7);
PwmOut right(PA_15);
Serial bluetooth(PA_11, PA_12); //define the bluetooth component
//DigitalOut led(D9); // might be used the class in lab
C12832 lcd(D11, D13, D12, D7, D10);
DigitalOut enable(PB_2);
DigitalOut leftD(PD_2) , rightD(PC_12);
//QEI lEn(D9,D14,NC, 624, QEI::X4_ENCODING);  
//QEI rEn(D6,D13,NC, 624, QEI::X4_ENCODING);
//InterruptIn fired(D14);
//Ticker myTicker;

    /*int leftPulses  = 0;    
    int rightPulses = 0;    
    int preLeftPulses = 0;
    int preRightPulses = 0;
    float sampleTime = 0;//wri */
    
    unsigned char rx;

//use encoders to get speed
/* void getSpeed(){
    leftPulses  = lEn.getPulses();
    leftSpeed = (leftPulses - preLeftPulses) / sampleTime ;
    preLeftPulses = leftPulses;

    rightPulses = rEn.getPulses();
    rightSpeed = (rightPulses - preRightPulses) / sampleTime ;
    preRightPulses = rightPulses;
    lcd.locate(20,20);
    lcd.printf("left:%.2f    right:%.2f",leftSpeed,rightSpeed);
} */

//use joystick to turn on/off the buggy
void onOff(){
    enable != enable;
}

//use bluetooth to return
void remoteReturn(){
    if (bluetooth.readable())
        {
            rx = bluetooth.getc(); // get data from bluebooth
            switch (rx) // use switch, case should be modified
            {
            case 00 :
            enable != enable;
            break;
            case 01: // turn around
            enable = 0;
            left.write(1);
            right.write(1);
            wait(0.1);
            leftD = 1;
            rightD = 0;
            enable = 1;
            left.period(0.001);
            right.period(0.001);
            left.write(0.7);
            right.write(0.7);
            wait(1.0);
            enable = 0;
            wait(0.25);
            leftD = rightD = 0;
            enable = 1;
            break;
            }
        }
    }


//use outcomes of sensors to indicate position
float updatePosition(float sensorsAnalog[6]){
    int parameters[6] = {-3 , -2 , -1 , 1 , 2 , 3};
    float position = 0;
    for (int i = 0; i < 6; i++)
    {
        position += sensorsAnalog[i] * parameters[i];
    }
    return position;
}

//pd control for position and calculate velocity
float getSpeedChange(float position , float lastPosition , float Kp , float Kd){
    float deriv = position - lastPosition;
    float speedChange = Kp * position + Kd * deriv;

    return speedChange;
}

//use parameter to transfer speed into duty cycle
/*
void updateLeftSpeed(float Kc , float leftSpeed){
    float leftCycle = Kc * leftSpeed;
    left.write(leftCycle);
}

void updateRightSpeed(float Kc , float rightSpeed){
    float rightCycle = Kc * rightSpeed;
    right.write(rightCycle);
}

*/

int main(){

    bluetooth.baud(9600);//baud rate
    //float maxSpeed = ;
    float speedChange = 0;
    float sensorsAnalog[6] = {0,0,0,0,0,0};
    float position = 0;
    float lastPosition = 0;
    
    //write base speed
    leftD = rightD = 0;
    enable = 1;
    float baseSpeed = 0.85;
    float leftSpeed,rightSpeed = baseSpeed;
    float period = 0.001;
    left.period(period);
    right.period(period);
    left.write(baseSpeed);
    right.write(baseSpeed);
  while (1)
    {
        remoteReturn();

        // printf the speed, if u want to use lce screen then modify printf.
        //myTicker.attach(&getSpeed, sampleTime);

        // scan
        for (int i = 0; i < 6; i++)
        {   
            leds[i] = 1;
            //read data
            sensorsAnalog[i] = sensors[i].read();
        }

        //get position
        position = updatePosition(sensorsAnalog);
        lcd.locate(20,0);
        lcd.printf("The output is %.2f",position);
        //line breaks?
        
        //use pd to get speedchange
        speedChange = getSpeedChange(position,lastPosition,0.08,0.001);
        lastPosition = position;


        //update speed
        leftSpeed = leftSpeed - speedChange;
        rightSpeed = rightSpeed + speedChange;
        
         if (leftSpeed > 0.87) {leftSpeed = 0.87;}
        if (leftSpeed < 0.83) {leftSpeed = 0.83;}
        
        if (rightSpeed > 0.87) {rightSpeed = 0.87;}
        if (rightSpeed < 0.83) {rightSpeed = 0.83;}
        
        
        //write duty cycles into pwm
        left.write(leftSpeed);
        right.write(rightSpeed);
        
       
        lcd.locate(0,20);
        lcd.printf("The speed %.2f  %.2f  %.2f",leftSpeed,rightSpeed,speedChange);
    } 
}

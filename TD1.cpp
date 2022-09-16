// ESP Group 46 - TD1

// Directives to include the necessary header files.
#include "C12832.h" 
#include "mbed.h" 

// Definitions used for ease of setting modes and directions.
#define FORWARDS 0    //Directions
#define BACKWARDS 1
#define UNI 0         //Modes
#define BI 1

//Definitions used for pins as they are constant values.
#define L_PWM PB_15   // PWM1/3
#define L_MODE PB_14
#define L_DIRECTION PB_13
#define R_PWM PC_8    // PWM3/3
#define R_MODE PC_6
#define R_DIRECTION PC_5
#define L_ENCODER PC_14
#define R_ENCODER PC_10

// Other relevant definitions used for calculations
#define WHEEL_RAD 0.04f   // Wheel radius = 4 cm (0.04 m)
#define DELTA_T 400000    // Delta t, for pulse measurement in microseconds
#define PULSE_REV 256        // Pulse per revolution (for encoders)
#define SAMPLE_F 2           // Sample frequency
#define SWITCH_F 10000.0f    // 10 kHz PWM frequency
#define PI 3.141592f

C12832 lcd(D11, D13, D12, D7, D10);
DigitalOut EN(PB_2);

class Pwm {
private:
    PwmOut _pwm;
    int _frequency;
    int _period; // Period to = 1/_frequency

public:
    Pwm(PinName pin, int frequency) : _pwm(pin), _frequency(frequency) {
        _period = (1000000 / frequency);  // Period in us
        _pwm.period_us(_period);      // Set PWM output period
    }

    void setDutyCycle(float duty_cycle){
        _pwm.write(duty_cycle);  // Set the duty cycle (val between 0.0 - 1.0)
    }
};

class Encoder{
private:
    InterruptIn channelA;   // InterruptIn channel, receives pulse signals from encoders
    Ticker sampler;         // Ticker, samples revolution speed
    Timeout pulseDt;       // Timeout, checks the delta t for pulse signal measurement
    int pulse_count;        // int, counts oulses
    int _pulse_per_s;      // int, pulse per second
    float _speed, _speed_norm;                // Wheel speed in m/s and normalised (0.0 - 1.0)
    float _sample_f, _sample_p;    // Sample frequency and period

    void incrementCount() {
        ++pulse_count;      // Increment the pulse counter
    }
    void samplePulse() {
        pulse_count = 0;    // Reset pulse counter
        pulseDt.attach_us(callback(this, &Encoder::calcSpeed), DELTA_T);  // Starts timeout
    }
    void calcSpeed() {
        _pulse_per_s = (pulse_count * 1000000 / (int) DELTA_T);
        _speed = ( ((float) _pulse_per_s / PULSE_REV) * (2.0f * PI * WHEEL_RAD) );
    }

public:
    Encoder(PinName chA, float sf) : channelA(chA), _sample_f(sf){
        channelA.rise(callback(this, &Encoder::incrementCount));  // Increment each time channelA pulses
        _sample_p = (1.00f / _sample_f);
        sampler.attach(callback(this, &Encoder::samplePulse), _sample_p);   // Starts ticker for sampling speed
    }
    float getSpeed(void) const  {
        return _speed;
    }
    void startCounter(void) {
        sampler.detach();   // Stop the ticker for speed sampling
        pulse_count = 0;    // Reset
    }
    int getCounter(void) const {
        return pulse_count;
    }
    void stopCounter(void){
        sampler.attach(callback(this, &Encoder::samplePulse), _sample_p);   // Start the ticker for speed sampling
    }
};

class Motor {
private:
    DigitalOut _mode;       
    DigitalOut _direction; 
    Pwm _motor;                 // PWM signal for motor

    void _setMode(int mode) {
        _mode = mode;
    }

public:
    Motor(PinName mode, PinName dir, PinName pwm, int freq) : _mode(mode), _direction(dir), _motor(pwm, freq) {
        _setMode(UNI);     // Set the mode to unipolar
        setDirection(FORWARDS);  // Set direction to forwards
        setSpeed(0.0f);         // Stops motor
    }
    void setDirection(int direction){
        _direction = direction;
    }
    void setSpeed(float speed){
        speed = 1.0f - speed;        // Sets the speed between 0 and 100% (maximum)
        _motor.setDutyCycle(speed);
    }
};

void test_motor(){
    Motor motorLeft(L_MODE, L_DIRECTION, L_PWM, SWITCH_F);
    Motor motorRight(R_MODE, R_DIRECTION, R_PWM, SWITCH_F);
    Encoder wheelLeft(L_ENCODER, SAMPLE_F);
    Encoder wheelRight(R_ENCODER, SAMPLE_F);
    
    motorLeft.setDirection(FORWARDS);
    motorRight.setDirection(FORWARDS);

    lcd.cls();
    lcd.locate(0, 0);
    lcd.printf("Motor Left:  %5.2lf m/s", wheelLeft.getSpeed());
    lcd.locate(0, 10);
    lcd.printf("Motor Right: %5.2lf m/s", wheelRight.getSpeed());

    for(int i = 0; i<2; i++) {
        for(int i = 0; i < 100; i++) {  // For loop to test the speed from 0 to max value corresponding to duty cycle
            float speed = ((float) i / 100.0f);    // Correspond value of i to the duty cycle
            motorLeft.setSpeed(speed);
            motorRight.setSpeed(speed);
            wait(0.1);
            lcd.locate(74, 0);
            lcd.printf("%5.2lf", wheelLeft.getSpeed());
            lcd.locate(74, 10);
            lcd.printf("%5.2lf", wheelRight.getSpeed());
        }

        motorLeft.setDirection(BACKWARDS); // Rotates the buggy on the spot.
        motorRight.setDirection(FORWARDS);
    }
    motorLeft.setSpeed(0.0);     // Set speed of left motor
    motorRight.setSpeed(0.0);     // Set speed of right motor

    while(1) {  // Allow tests of encoders without running motors
        wait(0.1);
        lcd.locate(74, 0); // Display only readings as they change
        lcd.printf("%5.2lf", wheelLeft.getSpeed());  // Display speed as 5 characters, i.e. xx.xx
        lcd.locate(74, 10);
        lcd.printf("%5.2lf", wheelRight.getSpeed());
    }
}

void draw_square(){
    Motor motorLeft(L_MODE, L_DIRECTION, L_PWM, SWITCH_F);
    Motor motorRight(R_MODE, R_DIRECTION, R_PWM, SWITCH_F);

    float R_straight = 0.3;
    float L_straight = R_straight * 1;
    float time_straight = 1.245;
    float R_turn = 0.3;
    float L_turn = R_turn * 1;
    float time_turn_90 = 0.45;
    float wait_time = 0.3;
    

    for(int i = 1; i < 9; i++) {
        motorLeft.setDirection(FORWARDS);
        motorRight.setDirection(FORWARDS);
        motorLeft.setSpeed(L_straight);
        motorRight.setSpeed(R_straight);
        
        wait(time_straight);
        
        motorLeft.setSpeed(0);
        motorRight.setSpeed(0);
        wait(wait_time);
        
        if (i < 4) {
            // Make a left turn
            motorLeft.setDirection(BACKWARDS);
            motorRight.setSpeed(R_turn);  
            motorLeft.setSpeed(L_turn);
            wait(time_turn_90); // Wait for 90 degrees rotation
            motorRight.setSpeed(0);
            motorLeft.setSpeed(0);
            wait(wait_time);
        } else if (i == 4) {
            motorRight.setDirection(BACKWARDS);
            motorRight.setSpeed(R_turn);
            motorLeft.setSpeed(L_turn);
            wait(time_turn_90 * 2.2);
            motorLeft.setSpeed(0);   
            motorRight.setSpeed(0);
            wait(wait_time);
        } else if (i == 8) {
            // Stop at the end.
            motorLeft.setSpeed(0);
            motorRight.setSpeed(0);
        } else {
            motorRight.setDirection(BACKWARDS);
            motorLeft.setSpeed(L_turn);
            motorRight.setSpeed(R_turn);
            wait(time_turn_90); // Wait for 90 degrees rotation
            motorLeft.setSpeed(0);
            motorRight.setSpeed(0);
            wait(wait_time);
        }}
}

int main(){
    EN = 1; // Enable signal for the H-Bridge
    //Select one of the two functions below by (un)commenting:
        draw_square();
        //test_motor();
    // EN = 0; // Disable EN after performing function if necessary
    while(1) {}
}

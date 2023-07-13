// Demonstration 2
// Author: Ishaan Ghatak

#include "mbed.h"
#include "C12832.h"

class Analogue_sensor {
  private:
  AnalogIn sensor;
  DigitalOut emitter;
  float value;
  Timeout turn_off;
  
  public:
  Analogue_sensor(PinName p1, PinName p2): sensor(p1), emitter(p2){
      emitter = 0 ;
      value = 0.0;}

    void emitter_on(void) {   
        emitter = 1;
        //turn_off.attach(callback(this, &Analogue_sensor::reading_value), 0.05);
    }

    void emitter_off(){
        emitter = 0;
    }
    
    void reading_value(){
        value = sensor.read();
        emitter = 0;
        emitter_on();
    }

    float reading(){
        return sensor.read();
    }
    

};




int main() {
    C12832 lcd(D11, D13, D12, D7, D10); // object named lcd 
  
    // Initialising sensors
    Analogue_sensor sensor_1 (PA_4, PA_13); // sensor, emitter
    Analogue_sensor sensor_2 (PC_3, PB_8);
    Analogue_sensor sensor_3 (PC_2, PC_6);
    Analogue_sensor sensor_4 (PB_0, PB_9);
    Analogue_sensor sensor_5 (PC_1, PC_8);
    Analogue_sensor sensor_6 (PC_0, PC_9);
    
    //float * sensor_scaling_factors = calibration();
    sensor_1.emitter_on();
    sensor_2.emitter_on();
    sensor_3.emitter_on();
    sensor_4.emitter_on();
    sensor_5.emitter_on();
    sensor_6.emitter_on();
        
    float S1, S2, S3, S4, S5, S6;
    while(1) {
        
        S1 = sensor_1.reading();
        S2 = sensor_2.reading();
        S3 = sensor_3.reading();
        S4 = sensor_4.reading();
        S5 = sensor_5.reading();
        S6 = sensor_6.reading();
        float average = (S1 + S2 + S3 + S4 + S5 + S6)/6;
        
    }
}

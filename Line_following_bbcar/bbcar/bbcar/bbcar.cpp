#include "bbcar.h"

BBCar::BBCar( PwmOut &pin_servo0, PwmOut &pin_servo1, Ticker &servo_ticker ):servo0(pin_servo0), servo1(pin_servo1){
    servo0.set_speed(0);
    servo1.set_speed(0);
    servo_ticker.attach(callback(this, &BBCar::controlWheel), 20ms);
}

void BBCar::controlWheel(){
    servo0.control();
    servo1.control();
}

void BBCar::stop(){
    servo0.set_factor(1);
    servo1.set_factor(1);
    servo0.set_speed(0);
    servo1.set_speed(0);
}

void BBCar::goStraight( double speed ){
    servo0.set_factor(1);
    servo1.set_factor(1);
    servo0.set_speed(speed);
    servo1.set_speed(-speed);
}

void BBCar::setCalibTable( int len0, double pwm_table0[], double speed_table0[], int len1, double pwm_table1[], double speed_table1[] ){
    servo0.set_calib_table(len0, pwm_table0, speed_table0);
    servo1.set_calib_table(len1, pwm_table1, speed_table1);
}
void BBCar::goStraightCalib ( double speed ){
    servo0.set_factor(1);
    servo1.set_factor(1);
    servo0.set_speed_by_cm(speed);
    servo1.set_speed_by_cm(-speed);
}
void BBCar::gofollowline(double speed, double factor0, double factor1){
    servo0.set_factor(factor0);
    servo1.set_factor(factor1);
    servo0.set_speed(speed * factor0);
    servo1.set_speed(-speed * factor1);
}
/*	speed : speed value of servo
    factor: control the speed value with 0~1
            control left/right turn with +/-
*/
void BBCar::turn( double speed, double factor ){
    if(factor > 0){
        servo0.set_factor(factor);
        servo1.set_factor(1);
    }
    else if(factor < 0){
        servo0.set_factor(1);
        servo1.set_factor(-factor);
    }
    servo0.set_speed(speed);
    servo1.set_speed(-speed);
}
extern int stepcounter0;
extern int stepcounter1;
void BBCar::park(int distance_1, int distance_2, int direction, double speed) {
    double current_distance0 = 0;
    double current_distance1 = 0;
    double feedback_0 = 0;
    double feedback_1 = 0;
    distance_2 = distance_2 + 6;
    distance_1 = distance_1 - 10;
    // initialize encoder counter
    stepcounter0 = 0;
    stepcounter1 = 0;
    // start move    
    servo0.set_factor(1);
    servo1.set_factor(1);
    // we want to moving backward
    servo0.set_speed_by_cm(-speed);
    servo1.set_speed_by_cm(speed);
    // move control(straight line)
    while(current_distance0 < distance_2 &&
             current_distance1 < distance_2) {
        /*if (current_distance0 - distance_2 < 3) {
            servo0.set_speed_by_cm(-speed * 0.7);
            }
            if (current_distance1 - distance_2 < 3) {
            servo1.set_speed_by_cm(speed * 0.7);
            }*/
        current_distance0 = stepcounter0 * 6.5 * 3.1415 / 32;
        current_distance1 = stepcounter1 * 6.5 * 3.1415 / 32;
        /*if (stepcounter0 < stepcounter1) {
            feedback_0 -= 0.05;
            feedback_1 += 0.05;
        }
        if (stepcounter0 > stepcounter1) {
            feedback_0 += 0.05;
            feedback_1 -= 0.05;
        }
        servo0.set_speed_by_cm(-speed - feedback_0);
        servo1.set_speed_by_cm(speed + feedback_1);*/
        ThisThread::sleep_for(50ms);
        //printf("D5 = %d, D6 = %d", stepcounter0, stepcounter1);
    }
    // stop car
    servo0.set_speed(0);
    servo1.set_speed(0);
    ThisThread::sleep_for(1000ms);
    // west situation turn right backward
    stepcounter0 = 0;
    stepcounter1 = 0;
    current_distance0 = stepcounter0 * 6.5 * 3.1415 / 32;
    current_distance1 = stepcounter1 * 6.5 * 3.1415 / 32;
    if (direction == 0) {
        servo0.set_speed_by_cm(-speed);
        servo1.set_speed_by_cm(0);
    }
    if (direction == 1) {
        servo0.set_speed_by_cm(0);
        servo1.set_speed_by_cm(speed);
    }
    while(current_distance0 < (16.49 - 4) &&
            current_distance1 < (16.49 - 4)) {
        /*if (current_distance0 - distance_2 < 3) {
            servo0.set_speed_by_cm(-speed * 0.7);
        }
        if (current_distance1 - distance_2 < 3) {
            servo1.set_speed_by_cm(speed * 0.7);
        }*/
        current_distance0 = stepcounter0 * 6.5 * 3.1415 / 32;
        current_distance1 = stepcounter1 * 6.5 * 3.1415 / 32;
        ThisThread::sleep_for(50ms);
    }
    // stop car
    servo0.set_speed(0);
    servo1.set_speed(0);
    ThisThread::sleep_for(1000ms);
    // initialize encoder counter
    stepcounter0 = 0;
    stepcounter1 = 0;
    // start move    
    servo0.set_factor(1);
    servo1.set_factor(1);
    // we want to moving backward
    servo0.set_speed_by_cm(-speed);
    servo1.set_speed_by_cm(speed);
    // move control(straight line)
    current_distance0 = 0;
    current_distance1 = 0;
    feedback_0 = 0;
    feedback_1 = 0;
    while(current_distance0 < distance_1 &&
            current_distance1 < distance_1) {
      /*if (current_distance0 - distance_1 < 3) {
        servo0.set_speed_by_cm(-speed * 0.7);
      }
      if (current_distance1 - distance_1 < 3) {
        servo1.set_speed_by_cm(speed * 0.7);
      }*/
      current_distance0 = stepcounter0 * 6.5 * 3.1415 / 32;
      current_distance1 = stepcounter1 * 6.5 * 3.1415 / 32;
      /*if (stepcounter0 < stepcounter1) {
            feedback_0 += 0.05;
            feedback_1 -= 0.05;
        }
        if (stepcounter0 > stepcounter1) {
            feedback_0 -= 0.05;
            feedback_1 += 0.05;
        }
        servo0.set_speed_by_cm(-speed - feedback_0);
        servo1.set_speed_by_cm(speed + feedback_1);*/
      ThisThread::sleep_for(50ms);
    }
    // stop car
    servo0.set_speed(0);
    servo1.set_speed(0);

}



float BBCar::clamp( float value, float max, float min ){
    if (value > max) return max;
    else if (value < min) return min;
    else return value;
}

int BBCar::turn2speed( float turn ){
    return 25+abs(25*turn);
}



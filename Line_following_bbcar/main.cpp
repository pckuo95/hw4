#include"mbed.h"
#include "bbcar.h"
BufferedSerial pc(USBTX,USBRX); //tx,rx
BufferedSerial uart(A1,A0); //tx,rx

Ticker servo_ticker;
// D6 is slower
// D5 is faster
PwmOut pin5(D5), pin6(D6);


// BufferedSerial xbee(D1, D0);
DigitalIn encoder0(D11);
DigitalIn encoder1(D12);
DigitalOut led3(LED3);
DigitalOut led2(LED2);

BBCar car(pin5, pin6, servo_ticker);
// pin5
volatile int stepcounter0 = 0;
Ticker encoder_ticker;
volatile int last0 = 0;
// pin6
volatile int stepcounter1 = 0;
volatile int last1 = 0;
volatile int counttimer = 0;
// pin5
void encoder_read() {
   int value0 = encoder0;
   counttimer++;
    if (!last0 && value0) {
       led3 = !led3;
       stepcounter0++;
    }
   last0 = value0;
   int value1 = encoder1;
   if (!last1 && value1) {
       led2 = !led2;
       stepcounter1++;
   }
   last1 = value1;
   
}
int main(){


// set BBCar
   // please contruct you own calibration table with each servo
   double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
   double speed_table0[] = {-21.126, -22.322, -21.684, -21.844, -10.357, 0.000, 11.111, 21.924, 21.525, 22.083, 21.445};
   double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
   double speed_table1[] = {-12.038, -11.719, -11.958, -10.503, -4.222, 0.000, 5.020, 9.706, 10.125, 11.161, 10.683};

   // first and fourth argument : length of table
   car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);
   // set decoder

    encoder_ticker.attach(&encoder_read, .02);

// Read line detect from uart
    uart.set_baud(9600);
    char test[1];
    test[0] = 'R';
    pc.write(test, sizeof(test));
    
    char buf[100];
    int enable = 1;
    int count = 0;
    double current_distance0 = 0;
    double current_distance1 = 0;
    double target_dist;
    int j = 0;
    
    while(1){
        int i;
        
        
        memset(buf, 0, 100);
        i = 0;
        
        int readDone = 0;
        char read[1];
        
        if (enable && j < 15) {
            read[0] = 's';
            pc.write(read, sizeof(read));
        } else {
            read[0] = 'n';
        }        
        uart.write(read, sizeof(read));
        j++;
        printf("j = %d\n", j);
        counttimer = 0;
        while (readDone == 0 && i < 100 && enable && counttimer < 50) {
            if(uart.readable()) {
                char recv[1];
                uart.read(recv, sizeof(recv));
                pc.write(recv, sizeof(recv));
                buf[i] = recv[0];
                // pc.write("%c", buf[i]);
                if(recv[0] == '\n') {
                    readDone = 1;
                    j = 0;
                    enable = 0;
                    //printf("Read Done! = %d\n", i);
                    break;
                }
                i++;
            }
            
        }
        /*
        for (i = 0; buf[i] != '\n'; i++) {
            printf("%c", buf[i]);
        }    
        */
        //
        int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
        
        for(i = 0; buf[i] != ',';i++) {
            x1 = 10 * x1;
            if (buf[i] == 0) break;
            x1 += buf[i] - '0'; 
        }
        i++;    // clear space
        for(; buf[i] != ',';i++) {
            x2 = 10 * x2;
            if (buf[i] == 0) break;
            x2 += buf[i] - '0';  
        }
        i++;    // clear space
        for(; buf[i] != ',';i++) {
            y1 = 10 * y1;
            if (buf[i] == 0) break;
            y1 += buf[i] - '0';  
        }
        i++;    // clear space
        for(;buf[i] != '\n';i++) {
            y2 = 10 * y2;
            if (buf[i] == 0) break;
            y2 += buf[i] - '0';  
        }
        printf("%d,%d,%d,%d\n",x1,x2,y1,y2);
        // control ----------
        // if x1 > x2 turn right
        int diff = x1 - x2;

        if (readDone == 1) {
            count++;
            //  '\'
            if (x1 > 80 || x2 > 80) {
                current_distance0 = 0;
                current_distance1 = 0;
                // initialize encoder counter
                stepcounter0 = 0;
                stepcounter1 = 0;
                target_dist = 10;
                if (x1 < 30 || x2 < 30) {
                    car.gofollowline(30, 0.4, 0.85);
                } else {
                    car.gofollowline(30, 0.5, 0.85);
                }
                printf("go left\n");
                while(current_distance0 < target_dist &&
                    current_distance1 < target_dist) {
                current_distance0 = stepcounter0 * 6.5 * 3.1415 / 32;
                current_distance1 = stepcounter1 * 6.5 * 3.1415 / 32;
                ThisThread::sleep_for(100ms);
            }
            } else {
                // '/'
                current_distance0 = 0;
                current_distance1 = 0;
                // initialize encoder counter
                stepcounter0 = 0;
                stepcounter1 = 0;
                target_dist = 10;
                if (x1 > 130 || x2 > 130) {
                    car.gofollowline(30, 0.8, 0.65);
                } else {
                    car.gofollowline(30, 0.8, 0.75);
                }
                printf("go right\n");
                while(current_distance0 < target_dist &&
                    current_distance1 < target_dist) {
                current_distance0 = stepcounter0 * 6.5 * 3.1415 / 32;
                current_distance1 = stepcounter1 * 6.5 * 3.1415 / 32;
                ThisThread::sleep_for(100ms);
                }
            }
            car.stop();
            ThisThread::sleep_for(500ms);
            if (count > 10) {
                enable = 0;
            } else {
                enable = 1;
            }
        } else {
            car.stop();
            ThisThread::sleep_for(500ms);   
        }
        // control --------
    }

}

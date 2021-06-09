#include"mbed.h"
#include "bbcar.h"



BufferedSerial pc(USBTX,USBRX); //tx,rx
BufferedSerial uart(A1,A0); //tx,rx

Ticker servo_ticker;
// D6 is slower
// D5 is faster
PwmOut pin5(D5), pin6(D6);
DigitalInOut pin10(D10);



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

// pin5
void encoder_read() {
   int value0 = encoder0;
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

    parallax_ping  ping1(pin10);
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
   // pc.write("initial!");


// Read line detect from uart
    uart.set_baud(9600);
    char test[1];
    test[0] = 'R';
    pc.write(test, sizeof(test));
    
    char buf[100];
    int backward = 0;
    int enable = 1;
    while(1){
        int i;
        
        
        memset(buf, 0, 100);
        i = 0;
        int j = 0;
        int readDone = 0;
        char read[1];
        
        if (enable) {
            read[0] = 's';
            pc.write(read, sizeof(read));
        } else {
            read[0] = 'n';
        }        
        uart.write(read, sizeof(read));
        
        while (readDone == 0 && i < 100 && enable) {
            if(uart.readable()) {
                char recv[1];
                uart.read(recv, sizeof(recv));
                pc.write(recv, sizeof(recv));
                buf[i] = recv[0];
                // pc.write("%c", buf[i]);
                if(recv[0] == '\n') {
                    readDone = 1;
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
        i = 0;
        int Tx = 0, Ry = 0;
        int Tx_minus = 0;
        if (buf[0] == '-') {
            Tx_minus = 1;
            i++;
        }
        for(; buf[i] != ',';i++) {
            Tx = 10 * Tx;
            if (buf[i] == 0) break;
            Tx += buf[i] - '0'; 
        }
        if (Tx_minus == 1) Tx = -Tx;

        i++;    // clear space
        for(; buf[i] != '\n';i++) {
            Ry = 10 * Ry;
            if (buf[i] == 0) break;
            Ry += buf[i] - '0';  
        }
                
        if (360 > Ry && Ry > 180) {
            Ry = Ry - 360;
        }
        
        if (Ry < 5 && Ry > -5 && Tx < 80 && Tx > -80) {
            enable = 0;
        } else {
            enable = 1;
        }

        printf("caluDone,%d,%d\n", Ry, Tx);
        
        // ---------------------- 
        
            double current_distance0 = 0;
            double current_distance1 = 0;
            // initialize encoder counter
            stepcounter0 = 0;
            stepcounter1 = 0;
            double target_dist;
        if (enable && (Ry > 6 || Ry < -6)) {
            if (Ry > 0) {
                target_dist = ((90 - Ry) * 11.0 * 3.1415 / 180.0) - 2;
                car.gofollowline(30, 0, 0.9);
                printf("turnL,%f\n",target_dist);
                while(current_distance0 < target_dist &&
                        current_distance1 < target_dist) {
                    current_distance0 = stepcounter0 * 6.5 * 3.1415 / 32;
                    current_distance1 = stepcounter1 * 6.5 * 3.1415 / 32;
                    ThisThread::sleep_for(100ms);
                }
            }
            if (Ry < 0) {
                target_dist = ((90 + Ry) * 11 * 3.1415 / 180) - 2;
                car.gofollowline(30, 0.65, 0);
                printf("turnR,%f\n",target_dist);
                while(current_distance0 < target_dist &&
                        current_distance1 < target_dist) {
                    current_distance0 = stepcounter0 * 6.5 * 3.1415 / 32;
                    current_distance1 = stepcounter1 * 6.5 * 3.1415 / 32;
                    ThisThread::sleep_for(100ms);
                }
            }
            car.stop();
            ThisThread::sleep_for(1s);

            // ---------------------- 
            current_distance0 = 0;
            current_distance1 = 0;
            // initialize encoder counter
            stepcounter0 = 0;
            stepcounter1 = 0;
            if (Tx > 0) {
                target_dist = (Tx / 10)-2;
            } else {
                target_dist = (-Tx / 10)-2;
            }
            car.gofollowline(30, 0.65, 0.9);
            printf("goF,%f\n",target_dist);
            while(current_distance0 < target_dist &&
                    current_distance1 < target_dist) {
                current_distance0 = stepcounter0 * 6.5 * 3.1415 / 32;
                current_distance1 = stepcounter1 * 6.5 * 3.1415 / 32;
                ThisThread::sleep_for(100ms);
            }
            car.stop();
            ThisThread::sleep_for(1s);
            //--- turn90
            // ---------------------- 
            current_distance0 = 0;
            current_distance1 = 0;
            // initialize encoder counter
            stepcounter0 = 0;
            stepcounter1 = 0;
            target_dist = 16.5 - 2;
            if (Ry > 0) {
                car.gofollowline(30, 0.65, 0);
                printf("ThenturnR,%f\n",target_dist);
            } else if (Ry < 0){
                car.gofollowline(30, 0, 0.9);
                printf("ThenturnL,%f\n",target_dist);
            }
            
            while(current_distance0 < target_dist &&
                    current_distance1 < target_dist) {
                current_distance0 = stepcounter0 * 6.5 * 3.1415 / 32;
                current_distance1 = stepcounter1 * 6.5 * 3.1415 / 32;
                ThisThread::sleep_for(50ms);
            }
            car.stop();
            ThisThread::sleep_for(1s);
        } else if (enable) {
            current_distance0 = 0;
            current_distance1 = 0;
            // initialize encoder counter
            stepcounter0 = 0;
            stepcounter1 = 0;
            target_dist = 3;
            if (Ry > 0) {
                car.gofollowline(30, 0, 0.8);
                printf("ThenturnR,%f\n",target_dist);
            } else if (Ry < 0){
                car.gofollowline(30, 0.7, 0);
                printf("ThenturnL,%f\n",target_dist);
            }
            while(current_distance0 < target_dist &&
                    current_distance1 < target_dist) {
                current_distance0 = stepcounter0 * 6.5 * 3.1415 / 32;
                current_distance1 = stepcounter1 * 6.5 * 3.1415 / 32;
                ThisThread::sleep_for(50ms);
            }
            car.stop();
            ThisThread::sleep_for(1s);
            // then reverse
            current_distance0 = 0;
            current_distance1 = 0;
            // initialize encoder counter
            stepcounter0 = 0;
            stepcounter1 = 0;
            target_dist = 3;
            j = j + 1;
            if (Ry > 0) {
                car.gofollowline(30, 0.7, 0);
                printf("ThenturnL,%f\n",target_dist);
            } else if (Ry < 0){
                car.gofollowline(30, 0, 0.8);
                printf("ThenturnR,%f\n",target_dist);
            }
            while(current_distance0 < target_dist &&
                    current_distance1 < target_dist) {
                current_distance0 = stepcounter0 * 6.5 * 3.1415 / 32;
                current_distance1 = stepcounter1 * 6.5 * 3.1415 / 32;
                ThisThread::sleep_for(50ms);
            }
            car.stop();
            ThisThread::sleep_for(1s);
            printf("PING=%f\n",(float)ping1);
            if (j == 3) break;
        }
        printf("PING=%f\n",(float)ping1);
        
        
        // -----------------------
        

        /*
          // ------------ control
        if (Ry > 0) {
            if (backward == 0) {
                if (Tx > 0) {
                    backward = 1;
                    // go after turn
                    printf("step:A!\n");
                    // car.gofollowline(7 * (Tx / 300) , 0, 1);
                    car.gofollowline(6, 0, 1);
                    ThisThread::sleep_for(1s);
                    car.stop();
                    ThisThread::sleep_for(1s);
                    // car.gofollowline(7 * (Ry / 20) , 1, 1);
                    car.gofollowline(6, 1, 1);
                    ThisThread::sleep_for(2s);
                } else {
                    backward = 1;
                    // turn after go 
                    printf("step:B!\n");
                    //car.gofollowline(7 * (Ry / 20) , 1, 1);
                    car.gofollowline(6, 1, 1);
                    ThisThread::sleep_for(2s);
                    car.stop();
                    ThisThread::sleep_for(1s);
                    car.gofollowline(6, 1, 0);
                    // car.gofollowline(5 * (-Tx / 300), 1, 0);
                    ThisThread::sleep_for(1s);
                }
            } else {
                if (Tx > 0) {
                    backward = 0;
                    // just go back
                    printf("step:C!\n");
                    // car.gofollowline(-7* (Ry / 20), 1, 1);
                    car.gofollowline(-6, 1, 1);
                    ThisThread::sleep_for(2s);
                } else {
                    backward = 0;
                    // turn after go 
                    printf("step:D!\n");
                    // car.gofollowline(-5 * (-Tx / 300), 1, 0);
                    car.gofollowline(-6, 1, 0);
                    ThisThread::sleep_for(1s);
                    car.stop();
                    ThisThread::sleep_for(1s);
                    car.gofollowline(-6, 1, 1);
                    // car.gofollowline(-7* (Ry / 20), 1, 1);
                    ThisThread::sleep_for(2s);
                }
            }
        } else {
            if (backward == 0) {
                if (Tx > 0) {
                    backward = 1;
                    // turn after go
                    printf("step:E!\n");
                    //car.gofollowline(7 * (-Ry / 20) , 1, 1);
                    car.gofollowline(6, 1, 1);
                    printf("E:go!\n");
                    ThisThread::sleep_for(2s);
                    car.stop();
                    ThisThread::sleep_for(1s);
                    // car.gofollowline(5 * (Tx / 300) , 0, 1);
                    car.gofollowline(6, 0, 1);
                    printf("E:turn!\n");
                    ThisThread::sleep_for(1s);
                } else {
                    backward = 1;
                    printf("step:F!\n");
                    // go after turn 
                    // car.gofollowline(5 * (-Tx / 300), 1, 0);
                    car.gofollowline(6, 1, 0);
                    printf("F:turn!\n");
                    ThisThread::sleep_for(1s);
                    car.stop();
                    ThisThread::sleep_for(1s);
                    car.gofollowline(6, 1, 1);
                    printf("F:go!\n");
                    //car.gofollowline(7 * (-Ry / 20) , 1, 1);
                    ThisThread::sleep_for(2s);
                }
            } else {
                if (Tx > 0) {
                    backward = 0;
                    // go back after turn
                    printf("step:G!\n");
                    // car.gofollowline(-5 * (Tx / 300), 1, 0);
                    car.gofollowline(-6, 1, 0);
                    printf("G:turn!\n");
                    ThisThread::sleep_for(1s);
                    car.stop();
                    ThisThread::sleep_for(1s);
                    //car.gofollowline(-7* (-Ry / 20), 1, 1);
                    car.gofollowline(-6, 1, 1);
                    printf("G:go!\n");
                    ThisThread::sleep_for(2s);
                } else {
                    backward = 0;
                    // just go back 
                    printf("step:H!\n");
                    car.gofollowline(-7, 1, 1);
                    printf("H:go!\n");
                    //car.gofollowline(-7* (-Ry / 20), 1, 1);
                    ThisThread::sleep_for(2s);
                }
            }
        }
            
        // add some disturb
        if (Ry > 10 && Tx < 10  && Tx > -10) {
            car.gofollowline(5, 0, 1);
            ThisThread::sleep_for(1s);
            car.stop();
        }
        if (Ry < 10 && Tx < 10  && Tx > -10) {
            car.gofollowline(5, 1, 0);
            ThisThread::sleep_for(1s);
            car.stop();
        }
        car.stop();
        ThisThread::sleep_for(1s);
        enable = 1;


        // -----
*/
        






        
      
        // ThisThread::sleep_for(100ms);

    }

}

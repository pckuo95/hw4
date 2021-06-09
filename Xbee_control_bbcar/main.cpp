#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

Ticker servo_ticker;
// D6 is slower
// D5 is faster
PwmOut pin5(D5), pin6(D6);
BufferedSerial xbee(D1, D0);
DigitalIn encoder0(D11);
DigitalIn encoder1(D12);
DigitalOut led3(LED3);
BBCar car(pin5, pin6, servo_ticker);

// pin5
volatile int stepcounter0 = 0;
Ticker encoder_ticker0;
volatile int last0 = 0;
// pin6
volatile int stepcounter1 = 0;
Ticker encoder_ticker1;
volatile int last1 = 0;

// pin5
void encoder_read0() {
   int value0 = encoder0;
   if (!last0 && value0) stepcounter0++;
   last0 = value0;
}
// pin6
void encoder_read1() {
   int value1 = encoder1;
   if (!last1 && value1) stepcounter1++;
   last1 = value1;
   led3 = !led3;
}


int main() {
// set BBCar
   // please contruct you own calibration table with each servo
   double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
   double speed_table0[] = {-21.126, -22.322, -21.684, -21.844, -10.357, 0.000, 11.111, 21.924, 21.525, 22.083, 21.445};
   double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
   double speed_table1[] = {-12.038, -11.719, -11.958, -10.503, -4.222, 0.000, 5.020, 9.706, 10.125, 11.161, 10.683};

   /*double pwm_table0[] = {-150, -120,-110, -100, -90, -80, -70, -60, -50, -40, -30, -20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 150};
   double speed_table0[] = {-21.126, -20.329, -20.967, -21.924, -23.199, -23.916, -24.475, -23.598, -24.794, -21.445, -15.705, -9.885, -4.146 , 0.000, 2.471, 8.530, 14.509, 20.249, 24.634, 24.554, 24.714, 23.837, 24.156, 23.119, 22.083, 22.083, 21.445};
   double pwm_table1[] = {-150, -120,-110, -100, -90, -80, -70, -60, -50, -40, -30, -20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 150};
   double speed_table1[] = {-15.038, -16.263, -15.944, -15.705, -15.147, -14.430, -13.393, -11.878, -9.806, -7.892, -6.059, -3.906, -1.913, 0.000, 1.196, 3.508, 5.421, 7.414, 9.567, 11.320, 12.835, 14.031, 14.908, 15.546, 15.944, 16.263, 16};
   */

   // first and fourth argument : length of table
   car.setCalibTable(27, pwm_table0, speed_table0, 27, pwm_table1, speed_table1);
   // set decoder

   encoder_ticker0.attach(&encoder_read0, .02);
   encoder_ticker1.attach(&encoder_read1, .02);
   // set Xbee
   char buf[256], outbuf[256];
   FILE *devin = fdopen(&xbee, "r");
   FILE *devout = fdopen(&xbee, "w");
   while (1) {
      // clear buffer
      memset(buf, 0, 256);
      for( int i = 0; ; i++ ) {
         char recv = fgetc(devin);
         if(recv == '\n') {
            printf("\r\n");
            break;
         }
         buf[i] = fputc(recv, devout);
      }

      
   RPC::call(buf, outbuf);
   }
}

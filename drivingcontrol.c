#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <bcm2835.h>

// ************************************************************************************
// Test program to control steering servo and throttle ESC (electronic speed control)
// via hardware pwm of raspberry pi.
// Steering servo has to be connected to GPIO BCM12, which is physical pin 32 + ground.
// ESC has to be connected to GPIO BMC13, which is physical pin 33 + ground.
// !!!!!!!! ATTENTION TO NOT BREAK THE SYSTEM !!!!!!
// esc will be powered by the LiPo battery of the vehicle, so NEVER; NEVER; NEVER EVER
// connect the red cable to the RPI. It will only need the PWM signal (white cable) +
// ground (black cable).
// For the servo you also should make sure to have an seperate power supply or battery
// for the 5 volts to move the servo. For testing purposes you can use the rpi, but you will
// need a good power supply, because otherwise the RPI will go in under voltage and reboot.
//
// While the program is running you can
// control the steering with:
//    k key to the left
//    l key to the right
// control the throttle with:
//    a key to increase speed
//    y key to decrease speed
//    b key to brake
//
// Author: luckynrslevin
// Date: 09.01.2020
// Version: 0.2
// License: MIT
//
// ************************************************************************************

char getch();
int setSteering(int data);
int setThrottle(int data);
int braking(int data);

int main(int argc, char **argv)
{
   // Load bcm2835 driver to get access to hardware PWM
   if (!bcm2835_init())
      return 1;

    // Basic configuration of PWM to control servo and ESC (electronic speed control)
    // see also https://raspberrypi.stackexchange.com/questions/53854/driving-pwm-output-frequency
    // For 50 Hz servo motro steering you have to set
    // clock to 192 and range to 2000
    // PWM Frequency in Hz = 19,200,000 Hz / pwmClock / pwmRange
    // 50 Hz = 19.200.000 Hz / 192 / 2000
    // minimum and maximum value for servo end position is 100 (= 1ms), 200 (=2ms)
    // center position is 150 (=1.5 ms)
    // Make sure to disable "balanced" markspace mode, see:
    //   https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
    //   => Page 139, chapter 9.4 Modes of Operation
    //   MSEN=1 is required to have classic PWM without balanced algorithm.
    // bcm2835 PWM api doc:
    //   http://www.airspayce.com/mikem/bcm2835/group__pwm.html
    bcm2835_pwm_set_clock(192);

    // PWM channel 0 for steering servo
    // (physical pin: 32, GPIO: BCM12, Alt0)
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_32, BCM2835_GPIO_FSEL_ALT0);
    bcm2835_pwm_set_mode(0, 1, 1);
    bcm2835_pwm_set_range(0, 2000);

    // PWM channel 1 for ESC
    // (physical pin: 33, GPIO: BCM13, Alt0)
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_33, BCM2835_GPIO_FSEL_ALT0);
    bcm2835_pwm_set_mode(1, 1, 1);
    bcm2835_pwm_set_range(1, 2000);

    // Set steering servor to neutral position (1.5 ms)
    int steering_data = 150;
    bcm2835_pwm_set_data(0, steering_data);
    // 1.5 ms also seems to be neutral position for esc,
    // maybe esc identifys initial signal during startup
    // not 100 % sure, but this seems to work for now
    int throttle_data = 150;
    bcm2835_pwm_set_data(1, throttle_data);

    while (1)
    {
       char pushedkey;
       pushedkey = getch();
       switch(pushedkey)
       {
         case 107: steering_data = steering_data + 1;
                   steering_data = setSteering(steering_data);
                   break;
         case 108: steering_data = steering_data - 1;
                   steering_data = setSteering(steering_data);
                   break;
         case 97: throttle_data = throttle_data + 1;
                   throttle_data = setThrottle(throttle_data);
                   break;
         case 121: throttle_data = throttle_data - 1;
                   throttle_data = setThrottle(throttle_data);
                   break;
         case 98:  printf("braking 1");
                   throttle_data = braking(throttle_data);
                   break;
         case 113: bcm2835_close();
                   return 0;
       }
    }
}

// set PWM steering value
int setSteering(int i) {
  // limit PWM min/max values to avoid breaking the servo (120/180).
  if (i < 120) {
    i = 120;
  } else if (i > 180) {
    i = 180;
  } else {
    // keep value passed to function
  }
  printf("New steering value: %d\n", i);
  bcm2835_pwm_set_data(0, i);
  return i;
}

// set esc throttle value
int setThrottle(int i) {
  // limit throttle to limit max. speed of vehicle
  if (i < 120) { // min maybe not required
    i = 120;
  } else if (i > 160) {
    i = 160;
  } else {
    // keep value passed to function
  }
  printf("New throttle value: %d\n", i);
  bcm2835_pwm_set_data(1, i);
  return i;
}

// braking
int braking(int i) {
  printf("braking 2");
  // limit PWM min/max values to avoid breaking the servo (120/180).
  if (i > 150) {
    printf("braking 3");
    i = 100; // set to 1 ms for braking
  } else {
    printf("braking 4");
    i = 150; // reset to 1,5 ms
  }
  printf("Braking, set value to: %d\n", i);
  bcm2835_pwm_set_data(1, i);
  return i;
}

// get character from keyboard without need of return key.
char getch() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}

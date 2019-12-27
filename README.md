# rover_driving_functions

Test program to control steering servo and throttle ESC (electronic speed control)
via hardware pwm of raspberry pi.
Steering servo has to be connected to GPIO BCM12, which is physical pin 32 + ground.
ESC has to be connected to GPIO BMC13, which is physical pin 33 + ground.

!!!!!!!! ATTENTION TO NOT BREAK THE SYSTEM !!!!!!!!

ESC will be powered by the LiPo battery of the vehicle, so NEVER connect the red cable to the RPI. It will only need the PWM signal + ground. For the servo you also should make sure to have an seperate power supply or battery to move the servo. For testing purposes you can use the rpi, but you will need a good power supply, because otherwise the RPI will go in under voltage and reboot.

While the program is running you can

control the steering with:
    
    k key to the left
    l key to the right
    
control the throttle with:
    
    a key to increase speed
    y key to decrease speed

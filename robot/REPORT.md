# Solution Report
This report summarizes my implementation of the 5 assignments within the robot lab of CSE2425 Embedded Systems course.

## Assignment 1: Variable Speed
> Control the motors using PWM signals and timers. The program should start the robot and make it run, after 3-4 sec at condtant speed, increase the duty cycle to make it run faaster and after 3-4 more seconds slow down the robot gradually and make it stop

> Passing Criteria: 1.5pts if using timer registers.

For this first assignment, I started of by figuring out what the passingcriteria meant by timer registers. So after setting the motors to run at some duty cycle,I need to wait some amount of seconds, and I need to do that in a busy wait loop thats dictated by these timer registers, which I found to be in the pico SDK ```hardware/structs/timer.h```. Based on that, I made the file [timers.c](./snapshots/assignment1_pwm/src/timers.c), which accesses the ```timer_hw->timerawl``` register to count microseconds and expose the ```busy_wait_ms_hw(uint32_t ms)``` function.  
After that, I moved onto actually implementing the PWM functionality for the motors. I put all of that in [motors_pwm.c](./snapshots/assignment1_pwm/src/motors_pwm.c). I first started my experiments by simply driving the pins high with simple SIO, to figure out which combinations are for forward and reverse for each motor and defined those at the top of the file. Then I set up my PWM config. I decided to use a PWM frequency of 20KHz, which given the system clock of 125MHz and choosing a PWM clock divider of 1.0, gives us a TOP value of 6249.  
After that, I made some helper functions for configuring the IO and getting the PWm slices and channels:
- ```gpio_set_func_pwm(uint gpio)``` - takes a GPIO pin number and sets its control register to PWM function
- ```pwm_slice_for_gpio(uint gpio)``` - returns the PWM slice number for a GPIO pin number, which given the 8 slices with 2 channels each is equal to  ```(gpio % 16) / 2```
- ```pwm_chan_is_b(uint gpio)``` - returns true if the GPIO pin number corresponds to a B channel. which is equivalent to checking if pin number is odd.
- ```pwm_init_slice(uint slice)``` - takes a slice number and initializes it for PWM. Sets the control, div, top, cc registers.
I also made some more helper functions which take a duty_percent between \[0,100\], and transforms that to the actual PWM CC level it would mean. Also made a helper to apply a level to the CC register of a corresponding GPIO pin number, and with that I expose 4 public functions that can be used, along with some nice structs for selecting direction and motor:
- ```motors_pwm_init()```
- ```motors_pwm_stop(motor_select_t which)```
- ```motors_pwm_drive(motor_select_t which, motor_dir_t dir, uint8_t duty_percent)```
- ```motors_pwm_drive_lr(motor_dir_t left_dir, uint8_t left_duty, motor_dir_t right_dir, uint8_t right_duty)```

With all these functions implemented, making [main.c](./snapshots/assignment1_pwm/src/main.c) is trivial. I initialize the motors, droive them at a set speed, wait 3 seconds, increase speed, wait another 3 seconds, then stop gradually.

## Assignment 2: Obstacle detection and stopping
> Use the front ultrasonic ranging sensor to detect an obstacle in front of the robot. Your program should make the robot run forward, detect if there is an obstacle ahead, stop when it comes close to the obstacle.

> Passing Criteria: 2.5pts if using PIO

For this assignment, I will go into less detail about the code than I did for the previous one, otherwise this report will become way too long.


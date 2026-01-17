/**
 * Student name: Vlad Paun
 * Student number: 6152937
 */

#include "motors_pwm.h"
#include "timers.h"

#include <stdbool.h>
#include <stdint.h>


int main(void) {
  motors_pwm_init();

  while (true) {
    motors_pwm_drive(MOTOR_BOTH, MOTOR_DIR_FORWARD,75);
    busy_wait_ms_hw(3000);

    motors_pwm_drive(MOTOR_BOTH, MOTOR_DIR_FORWARD, 100);
    busy_wait_ms_hw(3000);

    for (int d = 95; d >= 0; d -= 5) {
      motors_pwm_drive(MOTOR_BOTH, MOTOR_DIR_FORWARD, (uint8_t)d);
      busy_wait_ms_hw(100);
    }

    busy_wait_ms_hw(1000);
  }
}

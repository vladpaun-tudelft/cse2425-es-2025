#include "HCSR04.h"
#include "motors_pwm.h"
#include "timers.h"

static void on_go(void) {
  motors_pwm_drive(MOTOR_BOTH, MOTOR_DIR_FORWARD, 100);
}

static void on_stop(void) {
  motors_pwm_stop(MOTOR_BOTH);
}

int main(void) {
  motors_pwm_init();

  hcsr04_config_t cfg = {
      .trig_pin = 9u,
      .echo_pin = 8u,
      .stop_cm = 15u,
      .go_cm = 16u,
      .period_ms = 60u,
  };

  HCSR04_init(&cfg, on_stop, on_go);

  while (true) {
    busy_wait_ms_hw(1000);
  }
}

#include "B83609.h"
#include "line_follow.h"
#include "motors_pwm.h"
#include "timers.h"

#include <pico/stdio.h>
#include <stdbool.h>

#define TARGET_DISTANCE_M 2.5f
#define WHEEL_DIAMETER_MM 65.0f
#define ENCODER_TICKS_PER_MOTOR_REV 20.0f
#define STOP_EARLY_MM 30.0f

static uint32_t distance_to_ticks(float meters) {
  const float pi = 3.1415926f;
  float wheel_circ_mm = pi * WHEEL_DIAMETER_MM;
  float mm_per_tick = wheel_circ_mm / ENCODER_TICKS_PER_MOTOR_REV;
  float target_mm = meters * 1000.0f;
  float ticks = target_mm / mm_per_tick;
  if (ticks < 0.0f) {
    ticks = 0.0f;
  }
  return (uint32_t)(ticks + 0.5f);
}

int main(void) {
  stdio_init_all();
  busy_wait_ms_hw(2000);

  line_follow_init();

  B83609_init();
  B83609_reset_counts();

  uint32_t target_ticks = distance_to_ticks(TARGET_DISTANCE_M);
  float wheel_circ_mm = 3.1415926f * WHEEL_DIAMETER_MM;
  float ticks_per_mm = ENCODER_TICKS_PER_MOTOR_REV / wheel_circ_mm;
  uint32_t stop_early_ticks = (uint32_t)(STOP_EARLY_MM * ticks_per_mm + 0.5f);
  uint32_t stop_ticks = target_ticks - stop_early_ticks;

  while (true) {
    line_follow_step(NULL);
    uint32_t left_ticks = B83609_get_left_count();

    if (left_ticks >= stop_ticks) {
      motors_pwm_stop(MOTOR_BOTH);
      break;
    }

    busy_wait_ms_hw(10);
  }

  while (true) {
    busy_wait_ms_hw(100);
  }
}

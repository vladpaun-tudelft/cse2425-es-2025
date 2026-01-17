/**
 * Student name: Vlad Paun
 * Student number: 6152937
 */

#include "obstacle_avoidance.h"

#include "TCRT5000.h"
#include "motors_pwm.h"
#include "pid.h"
#include "timers.h"

#include <math.h>

static float read_front_cm(const hcsr04_config_t *front_cfg) {
  float cm = HCSR04_get_distance_cm(front_cfg);
  if (cm < 0.0f) {
    cm = 0.0f;
  }
  return cm;
}

static float read_side_cm(const hcsr04_config_t *side_cfg) {
  float cm = HCSR04_get_distance_cm(side_cfg);
  if (cm < 0.0f) {
    cm = 0.0f;
  }
  return cm;
}

static inline int8_t clamp_i8(int8_t v, int8_t lo, int8_t hi) {
  if (v < lo)
    return lo;
  if (v > hi)
    return hi;
  return v;
}

static int8_t apply_min_speed(int16_t speed) {
  if (speed == 0) {
    return 0;
  }
  int16_t abs_speed = speed < 0 ? -speed : speed;
  if (abs_speed < 65) {
    abs_speed = 65;
  }
  if (abs_speed > 100) {
    abs_speed = 100;
  }
  return (speed < 0) ? (int8_t)(-abs_speed) : (int8_t)abs_speed;
}

static void turn_right_to_match_front(const hcsr04_config_t *front_cfg,
                                      const hcsr04_config_t *side_cfg) {
  const float tol_cm = 0.5f;
  float front_cm = read_front_cm(front_cfg);
  float side_cm = read_side_cm(side_cfg);

  while (fabsf(side_cm - front_cm) > tol_cm) {
    motors_pwm_drive_lr_signed(80, -80);
    busy_wait_ms_hw(10);
    side_cm = read_side_cm(side_cfg);
  }
  motors_pwm_stop(MOTOR_BOTH);
}

static void forward_hold_side(const hcsr04_config_t *side_cfg,
                              float target_cm) {
  const float base_speed = 80.0f;
  const float loop_dt_s = 0.12f;
  const float inner_gain = 6.0f;
  pid_t pid;
  PID_init(&pid, 30.0f, 0.0f, 0.0f, -20.0f, 20.0f);
  PID_reset(&pid);

  while (TCRT5000_read_right_raw() < 1200u) {
    float side_cm = read_side_cm(side_cfg);
    float error = (target_cm - side_cm) / (target_cm + side_cm + 1.0f);
    float corr = PID_update(&pid, error, loop_dt_s);
    float inner = corr * inner_gain;

    float left_cmd = base_speed + inner;
    float right_cmd = base_speed - corr;

    int8_t left_speed = apply_min_speed((int16_t)lroundf(left_cmd));
    int8_t right_speed = apply_min_speed((int16_t)lroundf(right_cmd));
    motors_pwm_drive_lr_signed(left_speed, right_speed);
    busy_wait_ms_hw(20);
  }
}

void obstacle_avoidance_run(const hcsr04_config_t *front_cfg,
                            const hcsr04_config_t *side_cfg) {
  turn_right_to_match_front(front_cfg, side_cfg);

  float side_ref = read_side_cm(side_cfg) + 5.0;
  forward_hold_side(side_cfg, side_ref);
  motors_pwm_turn(MOTOR_TURN_RIGHT, 6u);
  motors_pwm_stop(MOTOR_BOTH);
}

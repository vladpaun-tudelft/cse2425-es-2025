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
#include <stdint.h>

static void turn_right_to_match_front(const hcsr04_config_t *front_cfg,
                                      const hcsr04_config_t *side_cfg) {
  const float tol_cm = 0.5f;
  float front_cm = HCSR04_get_distance_cm(front_cfg);
  float side_cm = HCSR04_get_distance_cm(side_cfg);

  while (fabsf(side_cm - front_cm) > tol_cm) {
    motors_pwm_drive_lr_signed(75, -75);
    busy_wait_ms_hw(10);
    side_cm = HCSR04_get_distance_cm(side_cfg);
  }
  motors_pwm_stop(MOTOR_BOTH);
}

static inline int clamp(float v, int lo, int hi) {
  if ((int)v < lo)return lo;
  if ((int)v > hi)return hi;
  return (int)v;
}

static void forward_hold_side(const hcsr04_config_t *side_cfg,
                              float target_cm) {
  const float base_speed = 70.0f;
  const float loop_dt_s = 0.2f;
  const float inner_gain = 6.0f;
  pid_t pid;
  PID_init(&pid, 30.0f, 0.0f, 0.0f, -20.0f, 20.0f);
  PID_reset(&pid);

  while (TCRT5000_read_right_raw() < 1000u) {
    float side_cm = HCSR04_get_distance_cm(side_cfg);
    float error = (target_cm - side_cm) / (target_cm + side_cm + 1.0f);
    float corr = PID_update(&pid, error, loop_dt_s);
    float inner = corr * inner_gain;

    float left_cmd = base_speed + inner;
    float right_cmd = base_speed - corr;

    motors_pwm_drive_lr_signed(clamp(left_cmd, -100, 100),
                               clamp(right_cmd, -100, 100));
    busy_wait_ms_hw(10);
  }
}

void obstacle_avoidance_run(const hcsr04_config_t *front_cfg,
                            const hcsr04_config_t *side_cfg) {
  turn_right_to_match_front(front_cfg, side_cfg);

  float side_ref = HCSR04_get_distance_cm(side_cfg) + 5.0;
  forward_hold_side(side_cfg, side_ref);
  motors_pwm_turn(MOTOR_TURN_RIGHT, 6u);
  motors_pwm_stop(MOTOR_BOTH);
}

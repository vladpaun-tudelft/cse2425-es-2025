/**
 * Student name: Vlad Paun
 * Student number: 6152937
 */

#include "line_follow.h"

#include "motors_pwm.h"
#include "pid.h"
#include "TCRT5000.h"

#include <math.h>
#include <stdint.h>

#define LOOP_DT_S 0.01f

#define BASE_SPEED 80
#define MAX_SPEED 100
#define PID_OUT_MARGIN 0

#define ERROR_DEADZONE 0.0f

#define PID_KP 200.0f
#define PID_KI 0.0f
#define PID_KD 2.0f
#define PID_OUT_MAX (MAX_SPEED - BASE_SPEED - PID_OUT_MARGIN)
#define PID_OUT_MIN (-(PID_OUT_MAX))

#define VLAD_FACTOR_MUL 1.0f
#define VLAD_FACTOR_LIN 0.0f
#define INNER_CORR_GAIN ((((float)(MAX_SPEED + BASE_SPEED)) / (float)PID_OUT_MAX) * (VLAD_FACTOR_MUL) + (VLAD_FACTOR_LIN))

static pid_t s_pid;

static inline int clamp(float v, int lo, int hi) {
  if ((int)v < lo)return lo;
  if ((int)v > hi)return hi;
  return (int)v;
}

void line_follow_init(void) {
  motors_pwm_init(65);
  TCRT5000_init();
  PID_init(&s_pid, PID_KP, PID_KI, PID_KD, PID_OUT_MIN, PID_OUT_MAX);
}

void line_follow_step(line_follow_debug_t *debug) {
  uint16_t left_raw = TCRT5000_read_left_raw();
  uint16_t right_raw = TCRT5000_read_right_raw();

  float sum = (float)left_raw + (float)right_raw + 1.0f;
  float error = ((float)left_raw - (float)right_raw) / sum;
  float abs_error = fabsf(error);
  if (abs_error < ERROR_DEADZONE) {
    error = 0.0f;
    abs_error = 0.0f;
  }

  float correction = PID_update(&s_pid, error, LOOP_DT_S);
  float inner_correction = correction * INNER_CORR_GAIN;

  float left_cmd = (float)BASE_SPEED;
  float right_cmd = (float)BASE_SPEED;
  if (correction >= 0.0f) {
    left_cmd -= inner_correction;
    right_cmd += correction;
  } else {
    left_cmd -= correction;
    right_cmd += inner_correction;
  }


  motors_pwm_drive_lr_signed(clamp(left_cmd, -MAX_SPEED, MAX_SPEED),
                             clamp(right_cmd, -MAX_SPEED, MAX_SPEED));

}

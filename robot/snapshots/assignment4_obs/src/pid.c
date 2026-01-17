/**
 * Student name: Vlad Paun
 * Student number: 6152937
 */

#include "pid.h"

#include <math.h>

void PID_init(pid_t *pid, float kp, float ki, float kd, float out_min,
              float out_max) {
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;
  pid->integral = 0.0f;
  pid->prev_error = 0.0f;
  pid->out_min = out_min;
  pid->out_max = out_max;
  pid->has_prev = false;

  if (ki != 0.0f) {
    float i_limit = fabsf(out_max / ki);
    pid->integral_min = -i_limit;
    pid->integral_max = i_limit;
  } else {
    pid->integral_min = 0.0f;
    pid->integral_max = 0.0f;
  }
}

void PID_reset(pid_t *pid) {
  pid->integral = 0.0f;
  pid->prev_error = 0.0f;
  pid->has_prev = false;
}

float PID_update(pid_t *pid, float error, float dt_s) {
  if (dt_s <= 0.0f)
    return 0.0f;

  float derivative = 0.0f;
  if (pid->has_prev) {
    derivative = (error - pid->prev_error) / dt_s;
  }

  if (pid->ki != 0.0f) {
    pid->integral += error * dt_s;
    if (pid->integral > pid->integral_max)
      pid->integral = pid->integral_max;
    if (pid->integral < pid->integral_min)
      pid->integral = pid->integral_min;
  }

  float output = (pid->kp * error) + (pid->ki * pid->integral) +
                 (pid->kd * derivative);

  if (output > pid->out_max)
    output = pid->out_max;
  if (output < pid->out_min)
    output = pid->out_min;

  pid->prev_error = error;
  pid->has_prev = true;

  return output;
}

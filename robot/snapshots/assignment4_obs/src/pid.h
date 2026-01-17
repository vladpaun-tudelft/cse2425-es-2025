/**
 * Student name: Vlad Paun
 * Student number: 6152937
 */

#pragma once

#include <stdbool.h>

typedef struct {
  float kp;
  float ki;
  float kd;
  float integral;
  float prev_error;
  float out_min;
  float out_max;
  float integral_min;
  float integral_max;
  bool has_prev;
} pid_t;

void PID_init(pid_t *pid, float kp, float ki, float kd, float out_min,
              float out_max);
void PID_reset(pid_t *pid);
float PID_update(pid_t *pid, float error, float dt_s);

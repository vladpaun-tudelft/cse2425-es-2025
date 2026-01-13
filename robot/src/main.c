#include "motors_pwm.h"
#include "pid.h"
#include "TCRT5000.h"
#include "timers.h"

#include <math.h>
#include <pico/stdio.h>
#include <stdio.h>

#define LOOP_MS 10u
#define LOOP_DT_S 0.01f

#define BASE_SPEED 80
#define MIN_DUTY 65
#define MAX_SPEED 100
#define PID_OUT_MARGIN 0

#define PID_KP 200.0f
#define PID_KI 0.0f
#define PID_KD 0.0f
#define PID_OUT_MAX (MAX_SPEED - BASE_SPEED - PID_OUT_MARGIN)
#define PID_OUT_MIN (-(PID_OUT_MAX))
#define INNER_CORR_GAIN ((float)(MAX_SPEED + BASE_SPEED) / (float)PID_OUT_MAX)

static inline int16_t clamp_i16(int16_t v, int16_t lo, int16_t hi) {
  if (v < lo)
    return lo;
  if (v > hi)
    return hi;
  return v;
}

static int8_t apply_min_duty(int16_t speed) {
  if (speed == 0)
    return 0;

  int16_t abs_speed = speed < 0 ? -speed : speed;
  if (abs_speed < MIN_DUTY)
    abs_speed = MIN_DUTY;
  if (abs_speed > MAX_SPEED)
    abs_speed = MAX_SPEED;

  return (speed < 0) ? (int8_t)(-abs_speed) : (int8_t)abs_speed;
}

int main(void) {
  motors_pwm_init();
  TCRT5000_init();
  stdio_init_all();
  busy_wait_ms_hw(2000);

  pid_t pid;
  PID_init(&pid, PID_KP, PID_KI, PID_KD, PID_OUT_MIN, PID_OUT_MAX);

  uint32_t loop_count = 0;

  while (true) {
    uint16_t left_raw = TCRT5000_read_left_raw();
    uint16_t right_raw = TCRT5000_read_right_raw();

    float denom = (float)left_raw + (float)right_raw + 1.0f;
    float error = ((float)left_raw - (float)right_raw) / denom;

    float correction = PID_update(&pid, error, LOOP_DT_S);
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

    int16_t left_speed = (int16_t)lroundf(left_cmd);
    int16_t right_speed = (int16_t)lroundf(right_cmd);

    left_speed = clamp_i16(left_speed, -MAX_SPEED, MAX_SPEED);
    right_speed = clamp_i16(right_speed, -MAX_SPEED, MAX_SPEED);

    motors_pwm_drive_lr_signed(apply_min_duty(left_speed),
                               apply_min_duty(right_speed));

    if ((loop_count % 20u) == 0u) {
      printf("L=%u R=%u err=%.3f corr=%.2f ls=%d rs=%d\n",
             (unsigned)left_raw, (unsigned)right_raw, (double)error,
             (double)correction, (int)left_speed, (int)right_speed);
    }
    loop_count++;

    busy_wait_ms_hw(LOOP_MS);
  }
}

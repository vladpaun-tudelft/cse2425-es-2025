/**
 * Student name: Vlad Paun
 * Student number: 6152937
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  MOTOR_LEFT = 0,
  MOTOR_RIGHT = 1,
  MOTOR_BOTH = 2,
} motor_select_t;

typedef enum {
  MOTOR_DIR_FORWARD = 0,
  MOTOR_DIR_REVERSE = 1,
} motor_dir_t;

void motors_pwm_init(void);
void motors_pwm_stop(motor_select_t which);
void motors_pwm_drive(motor_select_t which, motor_dir_t dir, uint8_t duty_percent);
void motors_pwm_drive_lr(motor_dir_t left_dir, uint8_t left_duty, motor_dir_t right_dir, uint8_t right_duty);

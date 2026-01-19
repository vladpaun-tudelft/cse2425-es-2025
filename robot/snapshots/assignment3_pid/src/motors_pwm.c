/**
 * Student name: Vlad Paun
 * Student number: 6152937
 */

#include "motors_pwm.h"

#include <hardware/regs/io_bank0.h>
#include <hardware/regs/pwm.h>
#include <hardware/structs/io_bank0.h>
#include <hardware/structs/pwm.h>

// Motor pins
// Right motor: GPIO19 + GPIO18  (PWM1B + PWM1A)
// Left motor:  GPIO21 + GPIO20  (PWM2B + PWM2A)
#define RIGHT_IN1 19
#define RIGHT_IN2 18
#define LEFT_IN1 21
#define LEFT_IN2 20

#define RIGHT_IN_FWD RIGHT_IN2
#define RIGHT_IN_REV RIGHT_IN1
#define LEFT_IN_FWD LEFT_IN1
#define LEFT_IN_REV LEFT_IN2

// PWM config
// f_pwm = clk_sys / (div * (TOP + 1))
// I want f_pwm = 20 kHz, so clk_sys = 125 MHz, div=1 => TOP=6249
#define PWM_TOP 6249u

static uint8_t s_min_duty = 0u;

static inline void gpio_set_func_pwm(uint gpio) {
  io_bank0_hw->io[gpio].ctrl = GPIO_FUNC_PWM << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
}

static inline uint pwm_slice_for_gpio(uint gpio) {
  // slice = (gpio % 16) / 2
  return (gpio & 0x0Fu) >> 1;
}

static inline bool pwm_chan_is_b(uint gpio) { return (gpio & 1u) != 0u; }

static void pwm_init_slice(uint slice) {
  pwm_hw->slice[slice].csr = 0;
  // divider thing = 1.0
  pwm_hw->slice[slice].div =
      (1u << PWM_CH0_DIV_INT_LSB) | (0u << PWM_CH0_DIV_FRAC_LSB);
  pwm_hw->slice[slice].top = PWM_TOP;
  pwm_hw->slice[slice].cc = 0;
  pwm_hw->slice[slice].csr = (1u << PWM_CH0_CSR_EN_LSB);
}

static inline uint8_t clamp(uint8_t v, uint8_t lo, uint8_t hi) {
  if (v < lo)
    return lo;
  if (v > hi)
    return hi;
  return v;
}

static inline uint16_t duty_to_level(uint8_t duty_percent) {
  duty_percent = clamp(duty_percent, 0, 100);
  if (duty_percent >= 100)
    return (uint16_t)(PWM_TOP + 1u);
  return (uint16_t)(((uint32_t)(PWM_TOP + 1u) * (uint32_t)duty_percent) / 100u);
}

static void pwm_set_gpio_level(uint gpio, uint16_t level) {
  uint slice = pwm_slice_for_gpio(gpio);
  uint32_t cc = pwm_hw->slice[slice].cc;

  if (pwm_chan_is_b(gpio)) {
    cc = (cc & 0x0000FFFFu) | ((uint32_t)level << 16);
  } else {
    cc = (cc & 0xFFFF0000u) | ((uint32_t)level);
  }

  pwm_hw->slice[slice].cc = cc;
}

static inline uint8_t signed_speed_to_duty(int8_t speed, motor_dir_t *dir) {
  int16_t s = (int16_t)speed;
  if (s >= 0) {
    *dir = MOTOR_DIR_FORWARD;
    return clamp((uint8_t)s, 0u, 100u);
  }

  *dir = MOTOR_DIR_REVERSE;
  return clamp((uint8_t)(-s), 0u, 100u);
}


static void motor_apply_pins(uint in_fwd, uint in_rev, motor_dir_t dir,
                             uint8_t duty_percent) {
  if (duty_percent != 0u && duty_percent < s_min_duty) {
    duty_percent = s_min_duty;
  }
  uint16_t level = duty_to_level(duty_percent);

  if (duty_percent == 0) {
    pwm_set_gpio_level(in_fwd, 0);
    pwm_set_gpio_level(in_rev, 0);
    return;
  }

  if (dir == MOTOR_DIR_FORWARD) {
    pwm_set_gpio_level(in_fwd, level);
    pwm_set_gpio_level(in_rev, 0);
  } else {
    pwm_set_gpio_level(in_fwd, 0);
    pwm_set_gpio_level(in_rev, level);
  }
}

void motors_pwm_init(uint8_t min_duty_percent) {
  s_min_duty = clamp(min_duty_percent, 0u, 100u);
  gpio_set_func_pwm(RIGHT_IN1);
  gpio_set_func_pwm(RIGHT_IN2);
  gpio_set_func_pwm(LEFT_IN1);
  gpio_set_func_pwm(LEFT_IN2);

  pwm_init_slice(1);
  pwm_init_slice(2);

  motors_pwm_stop(MOTOR_BOTH);
}

void motors_pwm_stop(motor_select_t which) {
  if (which == MOTOR_LEFT || which == MOTOR_BOTH) {
    pwm_set_gpio_level(LEFT_IN1, 0);
    pwm_set_gpio_level(LEFT_IN2, 0);
  }
  if (which == MOTOR_RIGHT || which == MOTOR_BOTH) {
    pwm_set_gpio_level(RIGHT_IN1, 0);
    pwm_set_gpio_level(RIGHT_IN2, 0);
  }
}

void motors_pwm_drive(motor_select_t which, motor_dir_t dir,
                      uint8_t duty_percent) {
  duty_percent = clamp(duty_percent, 0, 100);

  if (which == MOTOR_LEFT || which == MOTOR_BOTH) {
    motor_apply_pins(LEFT_IN_FWD, LEFT_IN_REV, dir, duty_percent);
  }
  if (which == MOTOR_RIGHT || which == MOTOR_BOTH) {
    motor_apply_pins(RIGHT_IN_FWD, RIGHT_IN_REV, dir, duty_percent);
  }
}

void motors_pwm_drive_lr(motor_dir_t left_dir, uint8_t left_duty,
                         motor_dir_t right_dir, uint8_t right_duty) {
  motor_apply_pins(LEFT_IN_FWD, LEFT_IN_REV, left_dir, left_duty);
  motor_apply_pins(RIGHT_IN_FWD, RIGHT_IN_REV, right_dir, right_duty);
}

void motors_pwm_drive_signed(motor_select_t which, int8_t speed) {
  motor_dir_t dir;
  if (which == MOTOR_LEFT || which == MOTOR_BOTH) {
    uint8_t duty = signed_speed_to_duty(speed, &dir);
    motor_apply_pins(LEFT_IN_FWD, LEFT_IN_REV, dir, duty);
  }
  if (which == MOTOR_RIGHT || which == MOTOR_BOTH) {
    uint8_t duty = signed_speed_to_duty(speed, &dir);
    motor_apply_pins(RIGHT_IN_FWD, RIGHT_IN_REV, dir, duty);
  }
}

void motors_pwm_drive_lr_signed(int8_t left_speed, int8_t right_speed) {
  motor_dir_t left_dir;
  motor_dir_t right_dir;
  uint8_t left_duty = signed_speed_to_duty(left_speed, &left_dir);
  uint8_t right_duty = signed_speed_to_duty(right_speed, &right_dir);
  motor_apply_pins(LEFT_IN_FWD, LEFT_IN_REV, left_dir, left_duty);
  motor_apply_pins(RIGHT_IN_FWD, RIGHT_IN_REV, right_dir, right_duty);
}

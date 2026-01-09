#include "motors_pwm.h"
#include "timers.h"
#include "ultrasonic.h"

#include <hardware/regs/io_bank0.h>
#include <hardware/structs/io_bank0.h>
#include <hardware/structs/sio.h>

#include <stdbool.h>
#include <stdint.h>

#define LEDPIN 25
#define MASK(x) (1u << (x))

static inline void led_init(void) {
  io_bank0_hw->io[LEDPIN].ctrl = GPIO_FUNC_SIO
                                 << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
  sio_hw->gpio_oe_set = MASK(LEDPIN);
}

static inline void led_on(void) { sio_hw->gpio_set = MASK(LEDPIN); }
static inline void led_off(void) { sio_hw->gpio_clr = MASK(LEDPIN); }

static uint32_t blink_period_ms_from_cm(uint32_t cm) {
  if (cm <= 15)
    return 0;
  if (cm >= 100)
    return 600;

  uint32_t in_min = 16, in_max = 100;
  uint32_t out_min = 60, out_max = 600;

  uint32_t num = (cm - in_min) * (out_max - out_min);
  uint32_t den = (in_max - in_min);
  return out_min + (num / den);
}

int main(void) {
  motors_pwm_init();
  ultrasonic_init_pio();
  led_init();

  bool moving = false;
  bool led_state = false;
  uint32_t led_tick_ms = 0;

  while (true) {
    uint32_t cm;
    if (ultrasonic_read_cm(&cm)) {
      if (!moving) {
        if (cm > 15) {
          motors_pwm_drive(MOTOR_BOTH, MOTOR_DIR_FORWARD, 100);
          moving = true;
        }
      } else {
        if (cm <= 15) {
          motors_pwm_stop(MOTOR_BOTH);
          moving = false;
        }
      }

      if (cm <= 15) {
        led_on();
      } else {
        uint32_t period = blink_period_ms_from_cm(cm);
        if (led_tick_ms >= period) {
          led_tick_ms = 0;
          led_state = !led_state;
          if (led_state)
            led_on();
          else
            led_off();
        }
      }
    }

    busy_wait_ms_hw(10);
    if (led_tick_ms < 1000000)
      led_tick_ms += 10;
  }
}

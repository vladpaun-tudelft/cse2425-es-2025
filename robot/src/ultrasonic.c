#include "ultrasonic.h"

#include <hardware/regs/io_bank0.h>
#include <hardware/structs/io_bank0.h>
#include <hardware/structs/pio.h>

#include "ultrasonic.pio.h"

#define ECHO_GPIO 8
#define TRIG_GPIO 9

#define PIO_IDX 0
#define SM_IDX 0

static inline void gpio_set_func_pio0(uint gpio) {
  io_bank0_hw->io[gpio].ctrl = GPIO_FUNC_PIO0
                               << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
}

void ultrasonic_init_pio(void) {
  gpio_set_func_pio0(ECHO_GPIO);
  gpio_set_func_pio0(TRIG_GPIO);

  PIO pio = pio0;
  uint offset = pio_add_program(pio, &hcsr04_echo_us_program);

  pio_sm_config c = hcsr04_echo_us_program_get_default_config(offset);

  sm_config_set_set_pins(&c, TRIG_GPIO, 1);
  sm_config_set_jmp_pin(&c, ECHO_GPIO);

  sm_config_set_in_shift(&c, false, false, 32);
  sm_config_set_out_shift(&c, false, false, 32);

  sm_config_set_clkdiv(&c, 125.0f);

  pio_sm_init(pio, SM_IDX, offset, &c);
  pio_sm_set_enabled(pio, SM_IDX, true);
}

bool ultrasonic_read_cm(uint32_t *out_cm) {
  PIO pio = pio0;

  if (pio_sm_is_rx_fifo_empty(pio, SM_IDX))
    return false;

  uint32_t x_end = pio_sm_get(pio, SM_IDX);

  if (x_end == 0) {
    *out_cm = 0;
    return true;
  }

  uint32_t echo_us = 65535u - (x_end & 0xFFFFu);

  *out_cm = echo_us / 58u;
  return true;
}

/**
 * Student name: Vlad Paun
 * Student number: 6152937
 */

#include "B83609.h"

#include "timers.h"

#include <hardware/gpio.h>
#include <hardware/regs/io_bank0.h>
#include <hardware/regs/pads_bank0.h>
#include <hardware/structs/io_bank0.h>
#include <hardware/structs/pads_bank0.h>
#include <hardware/structs/sio.h>

#define LEFT_PIN 3u
#define RIGHT_PIN 2u
#define DEBOUNCE_US 200u

static volatile uint32_t s_left_count = 0u;
static volatile uint32_t s_right_count = 0u;
static volatile uint64_t s_left_last_us = 0u;
static volatile uint64_t s_right_last_us = 0u;

static inline void gpio_set_func_sio(uint gpio) {
  io_bank0_hw->io[gpio].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
}

static inline void gpio_enable_input(uint gpio) {
  sio_hw->gpio_oe_clr = (1u << gpio);
  uint32_t val = pads_bank0_hw->io[gpio];
  val &= ~(PADS_BANK0_GPIO0_PUE_BITS | PADS_BANK0_GPIO0_PDE_BITS);
  val |= PADS_BANK0_GPIO0_IE_BITS;
  pads_bank0_hw->io[gpio] = val;
}

static inline bool debounce_ok(volatile uint64_t *last_us) {
  uint64_t now = time_us_hw();
  uint64_t last = *last_us;
  if ((now - last) < (uint64_t)DEBOUNCE_US) {
    return false;
  }
  *last_us = now;
  return true;
}

static void b83609_irq_callback(uint gpio, uint32_t events) {
  if ((events & GPIO_IRQ_EDGE_RISE) == 0u) {
    return;
  }

  if (gpio == LEFT_PIN) {
    if (debounce_ok(&s_left_last_us)) {
      s_left_count++;
    }
    return;
  }

  if (gpio == RIGHT_PIN) {
    if (debounce_ok(&s_right_last_us)) {
      s_right_count++;
    }
  }
}

void B83609_init(void) {
  gpio_set_func_sio(LEFT_PIN);
  gpio_enable_input(LEFT_PIN);
  gpio_set_func_sio(RIGHT_PIN);
  gpio_enable_input(RIGHT_PIN);

  s_left_count = 0u;
  s_right_count = 0u;
  s_left_last_us = 0u;
  s_right_last_us = 0u;

  gpio_set_irq_enabled_with_callback(LEFT_PIN, GPIO_IRQ_EDGE_RISE, true,
                                     &b83609_irq_callback);
  gpio_set_irq_enabled(RIGHT_PIN, GPIO_IRQ_EDGE_RISE, true);
}

void B83609_reset_counts(void) {
  s_left_count = 0u;
  s_right_count = 0u;
  s_left_last_us = 0u;
  s_right_last_us = 0u;
}

uint32_t B83609_get_left_count(void) { return s_left_count; }

uint32_t B83609_get_right_count(void) { return s_right_count; }

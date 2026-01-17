/**
 * Student name: Vlad Paun
 * Student number: 6152937
 */

#include "HCSR04.h"
#include "HCSR04.pio.h"

#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <stdint.h>

#define SM_HZ_CONTINUOUS 2000000ull
#define TRIG_BASE_CYCLES 24ull

static bool g_trig_loaded[2] = {false, false};
static bool g_echo_loaded[2] = {false, false};
static uint g_trig_off[2] = {0u, 0u};
static uint g_echo_off[2] = {0u, 0u};

static inline uint pio_index(PIO pio) { return (pio == pio1) ? 1u : 0u; }

static inline uint32_t period_ms_to_outer_count(uint64_t period_ms,
                                                uint64_t sm_hz) {
  uint64_t cycles = (sm_hz * period_ms) / 1000ull;
  if (cycles <= TRIG_BASE_CYCLES)
    return 1u;
  return (uint32_t)(cycles - TRIG_BASE_CYCLES);
}

bool HCSR04_init_continuous(const hcsr04_config_t *cfg) {
  if (!cfg)
    return false;
  if (cfg->trig_pin > 29u || cfg->echo_pin > 29u || cfg->period_ms < 60u)
    return false;
  if (cfg->pio != pio0 && cfg->pio != pio1)
    return false;

  uint pio_idx = pio_index(cfg->pio);
  if (!g_trig_loaded[pio_idx]) {
    if (!pio_can_add_program(cfg->pio, &HCSR04_trig_program))
      return false;
    g_trig_off[pio_idx] = pio_add_program(cfg->pio, &HCSR04_trig_program);
    g_trig_loaded[pio_idx] = true;
  }
  if (!g_echo_loaded[pio_idx]) {
    if (!pio_can_add_program(cfg->pio, &HCSR04_echo_program))
      return false;
    g_echo_off[pio_idx] = pio_add_program(cfg->pio, &HCSR04_echo_program);
    g_echo_loaded[pio_idx] = true;
  }
  uint off_trig = g_trig_off[pio_idx];
  uint off_echo = g_echo_off[pio_idx];

  float div = (float)clock_get_hz(clk_sys) / (float)SM_HZ_CONTINUOUS;

  // TRIG SM
  pio_gpio_init(cfg->pio, cfg->trig_pin);
  pio_sm_config ct = HCSR04_trig_program_get_default_config(off_trig);
  sm_config_set_clkdiv(&ct, div);
  pio_sm_set_consecutive_pindirs(cfg->pio, cfg->sm_trig, cfg->trig_pin, 1, true);
  sm_config_set_set_pins(&ct, cfg->trig_pin, 1);
  pio_sm_init(cfg->pio, cfg->sm_trig, off_trig, &ct);
  pio_sm_clear_fifos(cfg->pio, cfg->sm_trig);
  pio_sm_put_blocking(
      cfg->pio, cfg->sm_trig,
      period_ms_to_outer_count(cfg->period_ms, SM_HZ_CONTINUOUS));

  // ECHO SM
  pio_gpio_init(cfg->pio, cfg->echo_pin);
  gpio_pull_down(cfg->echo_pin);
  pio_sm_set_consecutive_pindirs(cfg->pio, cfg->sm_echo, cfg->echo_pin, 1,
                                 false);
  pio_sm_config ce = HCSR04_echo_program_get_default_config(off_echo);
  sm_config_set_clkdiv(&ce, div);
  sm_config_set_in_pins(&ce, cfg->echo_pin);
  sm_config_set_jmp_pin(&ce, cfg->echo_pin);
  pio_sm_init(cfg->pio, cfg->sm_echo, off_echo, &ce);
  pio_sm_clear_fifos(cfg->pio, cfg->sm_echo);

  pio_sm_set_enabled(cfg->pio, cfg->sm_echo, true);
  pio_sm_set_enabled(cfg->pio, cfg->sm_trig, true);

  return true;
}

float HCSR04_get_distance_cm(const hcsr04_config_t *cfg) {
  if (!cfg)
    return 0.0f;
  if (cfg->pio != pio0 && cfg->pio != pio1)
    return 0.0f;

  while (pio_sm_is_rx_fifo_empty(cfg->pio, cfg->sm_echo)) {
  }

  uint32_t raw = pio_sm_get(cfg->pio, cfg->sm_echo);
  uint32_t ticks = 0xFFFFFFFFu - raw;
  return ((float)ticks) / 58.0f;
}

#include "HCSR04.h"
#include "HCSR04.pio.h"

#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/irq.h>
#include <hardware/pio.h>

#define PIO_TRIG pio0
#define PIO_ECHO pio1
#define SM_TRIG 0u
#define SM_ECHO 1u

#define SM_HZ 2000000u
#define TRIG_CYCLES_PER_OUTER 1058u
#define MIN_PULSE_US 16u

static hcsr04_event_cb_t g_on_stop = NULL;
static hcsr04_event_cb_t g_on_go = NULL;

static inline uint32_t cm_to_us(uint32_t cm) { return cm * 58u; }

static inline uint32_t period_ms_to_outer_count(uint32_t period_ms) {
  uint64_t cycles = ((uint64_t)SM_HZ * (uint64_t)period_ms) / 1000ull;
  uint32_t outer = (uint32_t)(cycles / (uint64_t)TRIG_CYCLES_PER_OUTER);
  if (outer < 1u)
    outer = 1u;
  return outer;
}

static void pio1_irq0_handler(void) {
  if (pio_interrupt_get(PIO_ECHO, 0)) {
    pio_interrupt_clear(PIO_ECHO, 0);
    if (g_on_stop)
      g_on_stop();
  }
  if (pio_interrupt_get(PIO_ECHO, 1)) {
    pio_interrupt_clear(PIO_ECHO, 1);
    if (g_on_go)
      g_on_go();
  }
}

bool HCSR04_init(const hcsr04_config_t *cfg, hcsr04_event_cb_t on_stop,
                 hcsr04_event_cb_t on_go) {
  if (!cfg)
    return false;
  if (cfg->trig_pin > 29u || cfg->echo_pin > 29u || cfg->period_ms < 60u ||
      cfg->stop_cm == 0u || cfg->go_cm == 0u || cfg->go_cm <= cfg->stop_cm)
    return false;

  g_on_stop = on_stop;
  g_on_go = on_go;

  uint off_trig = pio_add_program(PIO_TRIG, &HCSR04_trig_program);
  uint off_echo = pio_add_program(PIO_ECHO, &HCSR04_echo_program);

  float div = (float)clock_get_hz(clk_sys) / (float)SM_HZ;

  // TRIG SM
  pio_gpio_init(PIO_TRIG, cfg->trig_pin);
  pio_sm_config ct = HCSR04_trig_program_get_default_config(off_trig);
  sm_config_set_clkdiv(&ct, div);
  pio_sm_set_consecutive_pindirs(PIO_TRIG, SM_TRIG, cfg->trig_pin, 1, true);
  sm_config_set_set_pins(&ct, cfg->trig_pin, 1);
  pio_sm_init(PIO_TRIG, SM_TRIG, off_trig, &ct);
  pio_sm_clear_fifos(PIO_TRIG, SM_TRIG);
  pio_sm_put_blocking(PIO_TRIG, SM_TRIG,
                      period_ms_to_outer_count(cfg->period_ms));

  // ECHO SM
  pio_gpio_init(PIO_ECHO, cfg->echo_pin);
  gpio_set_function(cfg->echo_pin, GPIO_FUNC_PIO1);
  gpio_pull_down(cfg->echo_pin);
  pio_sm_set_consecutive_pindirs(PIO_ECHO, SM_ECHO, cfg->echo_pin, 1, false);
  pio_sm_config ce = HCSR04_echo_program_get_default_config(off_echo);
  sm_config_set_clkdiv(&ce, div);
  sm_config_set_jmp_pin(&ce, cfg->echo_pin);

  pio_sm_init(PIO_ECHO, SM_ECHO, off_echo, &ce);
  pio_sm_clear_fifos(PIO_ECHO, SM_ECHO);

  uint32_t stop_us = cm_to_us(cfg->stop_cm);
  uint32_t go_us = cm_to_us(cfg->go_cm);
  if (stop_us <= MIN_PULSE_US || go_us <= MIN_PULSE_US)
    return false;
  pio_sm_put_blocking(PIO_ECHO, SM_ECHO, stop_us - MIN_PULSE_US);
  pio_sm_put_blocking(PIO_ECHO, SM_ECHO, go_us - MIN_PULSE_US);

  
  irq_set_exclusive_handler(PIO1_IRQ_0, pio1_irq0_handler);

  pio_interrupt_clear(PIO_ECHO, 0);
  pio_interrupt_clear(PIO_ECHO, 1);

  pio_set_irq0_source_enabled(PIO_ECHO, pis_interrupt0, true);
  pio_set_irq0_source_enabled(PIO_ECHO, pis_interrupt1, true);

  irq_set_enabled(PIO1_IRQ_0, true);

  pio_sm_set_enabled(PIO_ECHO, SM_ECHO, true);
  pio_sm_set_enabled(PIO_TRIG, SM_TRIG, true);

  return true;
}

#pragma once
#include <hardware/pio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  PIO pio;
  uint sm_trig;
  uint sm_echo;
  uint8_t trig_pin;
  uint8_t echo_pin;
  uint64_t period_ms;
} hcsr04_config_t;

bool HCSR04_init_continuous(const hcsr04_config_t *cfg);
float HCSR04_get_distance_cm(const hcsr04_config_t *cfg);

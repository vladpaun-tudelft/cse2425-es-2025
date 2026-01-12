#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef void (*hcsr04_event_cb_t)(void);

typedef struct {
  uint8_t trig_pin;
  uint8_t echo_pin;
  uint32_t stop_cm;
  uint32_t go_cm;
  uint64_t period_ms;
} hcsr04_config_t;

bool HCSR04_init(const hcsr04_config_t *cfg, hcsr04_event_cb_t on_stop, hcsr04_event_cb_t on_go);

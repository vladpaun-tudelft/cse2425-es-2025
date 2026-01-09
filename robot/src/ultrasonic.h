#pragma once
#include <stdbool.h>
#include <stdint.h>

void ultrasonic_init_pio(void);
bool ultrasonic_read_cm(uint32_t *out_cm);

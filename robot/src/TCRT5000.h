#pragma once

#include <stdint.h>

void TCRT5000_init(void);
uint16_t TCRT5000_read_left_raw(void);
uint16_t TCRT5000_read_right_raw(void);

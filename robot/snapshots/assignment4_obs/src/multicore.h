/**
 * Student name: Vlad Paun
 * Student number: 6152937
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

void multicore_launch_core1(void (*entry)(void));
void multicore_fifo_push_blocking(uint32_t data);
uint32_t multicore_fifo_pop_blocking(void);
bool multicore_fifo_rvalid(void);
bool multicore_fifo_wready(void);
void multicore_fifo_drain(void);

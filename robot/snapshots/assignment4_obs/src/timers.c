/**
 * Student name: Vlad Paun
 * Student number: 6152937
 */

#include "timers.h"
#include <hardware/structs/timer.h>

void busy_wait_ms_hw(uint32_t ms) {
  uint32_t start = timer_hw->timerawl;
  uint32_t delay_us = ms * 1000u;

  while ((uint32_t)(timer_hw->timerawl - start) < delay_us) {
    __asm volatile("nop");
  }
}

uint64_t time_us_hw(void) {
  uint32_t hi = timer_hw->timerawh;
  uint32_t lo = timer_hw->timerawl;
  return ((uint64_t)hi << 32) | (uint64_t)lo;
}

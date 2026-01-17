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

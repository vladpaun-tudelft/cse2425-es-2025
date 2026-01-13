#include "line_follow.h"
#include "timers.h"

#include <pico/stdio.h>
#include <stdint.h>
#include <stdio.h>

#define LOOP_MS 10u

void debug_print(uint32_t loop_count, line_follow_debug_t debug);

int main(void) {
  stdio_init_all();
  busy_wait_ms_hw(2000);

  line_follow_init();

  uint32_t loop_count = 0;

  while (true) {
    line_follow_debug_t debug = {0};
    line_follow_step(&debug);

    debug_print(loop_count, debug);
    loop_count++;

    busy_wait_ms_hw(LOOP_MS);
  }
}

void debug_print(uint32_t loop_count, line_follow_debug_t debug) {
  if ((loop_count % 20u) == 0u) {
    printf("L=%u R=%u err=%.3f corr=%.2f ls=%d rs=%d\n",
           (unsigned)debug.left_raw, (unsigned)debug.right_raw,
           (double)debug.error, (double)debug.correction, (int)debug.left_speed,
           (int)debug.right_speed);
  }
}

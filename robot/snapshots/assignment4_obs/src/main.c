#include "HCSR04.h"
#include "line_follow.h"
#include "obstacle_avoidance.h"
#include "timers.h"

#include <pico/stdio.h>
#include <stdbool.h>
#include <pico/multicore.h>

#define FRONT_TRIG_PIN 9
#define FRONT_ECHO_PIN 8
#define SIDE_TRIG_PIN 7
#define SIDE_ECHO_PIN 6
#define SENSOR_PERIOD_MS 60u
#define STOP_CM 15u
#define STOP_COUNT 3u

typedef enum {
  STATE_LINE_FOLLOW = 0,
  STATE_AVOID_OBSTACLE,
} robot_state_t;

static hcsr04_config_t g_front_cfg;
static void core1_entry(void) {
  uint8_t under_count = 0;

  while (true) {
    float cm = HCSR04_get_distance_cm(&g_front_cfg);

    if (cm < (float)STOP_CM) {
      under_count++;
      if (under_count >= STOP_COUNT) {
        under_count = 0;
        multicore_fifo_push_blocking(1u);
      }
    } else {
      under_count = 0;
    }

    busy_wait_ms_hw(SENSOR_PERIOD_MS);
  }
}

int main(void) {
  stdio_init_all();
  busy_wait_ms_hw(2000);

  line_follow_init();

  g_front_cfg = (hcsr04_config_t){
      .pio = pio0,
      .sm_trig = 0,
      .sm_echo = 1,
      .trig_pin = FRONT_TRIG_PIN,
      .echo_pin = FRONT_ECHO_PIN,
      .period_ms = SENSOR_PERIOD_MS,
  };

  hcsr04_config_t side_cfg = (hcsr04_config_t){
      .pio = pio1,
      .sm_trig = 0,
      .sm_echo = 1,
      .trig_pin = SIDE_TRIG_PIN,
      .echo_pin = SIDE_ECHO_PIN,
      .period_ms = SENSOR_PERIOD_MS,
  };

  HCSR04_init_continuous(&g_front_cfg);
  HCSR04_init_continuous(&side_cfg);
  multicore_launch_core1(core1_entry);

  robot_state_t state = STATE_LINE_FOLLOW;
  while (true) {
    if (multicore_fifo_rvalid()) {
      (void)multicore_fifo_pop_blocking();
      state = STATE_AVOID_OBSTACLE;
    }

    if (state == STATE_LINE_FOLLOW) {
      line_follow_step(NULL);
    } else {
      obstacle_avoidance_run(&g_front_cfg, &side_cfg);
      state = STATE_LINE_FOLLOW;
    }

    busy_wait_ms_hw(10);
  }
}

#include "HCSR04.h"
#include "timers.h"

#include <stdio.h>
#include <pico/stdio.h>

#define FRONT_TRIG_PIN 9
#define FRONT_ECHO_PIN 8
#define SIDE_TRIG_PIN 7
#define SIDE_ECHO_PIN 6
#define SENSOR_PERIOD_MS 60u
#define PRINT_PERIOD_MS 200u

int main(void) {
  stdio_init_all();
  busy_wait_ms_hw(2000);

  hcsr04_config_t front_cfg = (hcsr04_config_t){
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

  HCSR04_init_continuous(&front_cfg);
  HCSR04_init_continuous(&side_cfg);

  while (true) {
    float front_cm = HCSR04_get_distance_cm(&front_cfg);
    float side_cm = HCSR04_get_distance_cm(&side_cfg);
    printf("front=%.2f cm, side=%.2f cm\n", (double)front_cm, (double)side_cm);
    busy_wait_ms_hw(PRINT_PERIOD_MS);
  }
}

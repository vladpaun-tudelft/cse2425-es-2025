#include "pico/stdlib.h"

#define B1A 17
#define B2A 27
#define A1A 16
#define A2A 26

static inline void init_pin(uint pin) {
  gpio_init(pin);
  gpio_set_dir(pin, GPIO_OUT);
  gpio_put(pin, 0);
}

// Motor A
static inline void motor_a_forward(void) {
  gpio_put(A1A, 1);
  gpio_put(A2A, 0);
}
static inline void motor_a_reverse(void) {
  gpio_put(A1A, 0);
  gpio_put(A2A, 1);
}

// Motor B
static inline void motor_b_forward(void) {
  gpio_put(B1A, 1);
  gpio_put(B2A, 0);
}
static inline void motor_b_reverse(void) {
  gpio_put(B1A, 0);
  gpio_put(B2A, 1);
}

int main() {
  stdio_init_all();

  init_pin(A1A);
  init_pin(A2A);
  init_pin(B1A);
  init_pin(B2A);

  while (true) {
    // 2s one way
    motor_a_forward();
    motor_b_forward();
    sleep_ms(2000);

    // 2s the other way
    motor_a_reverse();
    motor_b_reverse();
    sleep_ms(2000);
  }
}

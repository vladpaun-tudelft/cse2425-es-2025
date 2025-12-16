#include <hardware/regs/io_bank0.h>
#include <hardware/structs/io_bank0.h>
#include <hardware/structs/sio.h>

#define LEDPIN 25
#define MASK(x) (1L << (x))

void ms_delay(int ms) {
  while (ms-- > 0) {
    volatile int x = 5000;
    while (x-- > 0) {
      __asm("nop");
    }
  }
}

void setup() {
  io_bank0_hw->io[LEDPIN].ctrl = GPIO_FUNC_SIO << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
  sio_hw->gpio_oe_set = MASK(LEDPIN);
}

void loop() {
  sio_hw->gpio_set = MASK(LEDPIN);
  ms_delay(300);
  sio_hw->gpio_clr = MASK(LEDPIN);
  ms_delay(300);
}

void main() {
  setup();
  while (true) { loop(); }
}
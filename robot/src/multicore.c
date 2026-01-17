/**
 * Student name: Vlad Paun
 * Student number: 6152937
 */

#include "multicore.h"

#include <hardware/regs/sio.h>
#include <hardware/structs/scb.h>
#include <hardware/structs/sio.h>

#define PICO_CORE1_STACK_SIZE 0x800u
#define PICO_CORE1_STACK_WORDS (PICO_CORE1_STACK_SIZE / sizeof(uint32_t))

static uint32_t __attribute__((section(".stack1"))) s_core1_stack[PICO_CORE1_STACK_WORDS];

bool multicore_fifo_rvalid(void) {
  return (sio_hw->fifo_st & SIO_FIFO_ST_VLD_BITS) != 0u;
}

bool multicore_fifo_wready(void) {
  return (sio_hw->fifo_st & SIO_FIFO_ST_RDY_BITS) != 0u;
}

void multicore_fifo_drain(void) {
  while (multicore_fifo_rvalid()) {
    (void)sio_hw->fifo_rd;
  }
}

void multicore_fifo_push_blocking(uint32_t data) {
  while (!multicore_fifo_wready()) {
    __asm volatile("nop");
  }
  sio_hw->fifo_wr = data;
  __asm volatile("sev");
}

uint32_t multicore_fifo_pop_blocking(void) {
  while (!multicore_fifo_rvalid()) {
    __asm volatile("nop");
  }
  return sio_hw->fifo_rd;
}

static void multicore_launch_core1_raw(void (*entry)(void), uint32_t *sp) {
  uint32_t cmd_sequence[] = {0u, 0u, 1u, scb_hw->vtor, (uintptr_t)sp, ((uintptr_t)entry)};
  size_t seq = 0u;
  do {
    uint32_t cmd = cmd_sequence[seq];
    if (cmd == 0u) {
      multicore_fifo_drain();
      __asm volatile("sev");
    }
    multicore_fifo_push_blocking(cmd);
    uint32_t response = multicore_fifo_pop_blocking();
    seq = (cmd == response) ? (seq + 1) : 0;
  } while (seq < 6);
}

void multicore_launch_core1(void (*entry)(void)) {
  uint32_t *stack_bottom = s_core1_stack;
  uint32_t *stack_ptr = stack_bottom + (PICO_CORE1_STACK_WORDS);

  multicore_launch_core1_raw(entry, stack_ptr);
}

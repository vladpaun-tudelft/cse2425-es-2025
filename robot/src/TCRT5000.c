/**
 * Student name: Vlad Paun
 * Student number: 6152937
 */

#include "TCRT5000.h"

#include <hardware/regs/adc.h>
#include <hardware/regs/io_bank0.h>
#include <hardware/regs/pads_bank0.h>
#include <hardware/structs/adc.h>
#include <hardware/structs/io_bank0.h>
#include <hardware/structs/pads_bank0.h>

#define A0LEFT 27u
// #define D0LEFT 17u
#define A0RIGHT 26u
// #define D0RIGHT 16u

#define ADC_CH_LEFT 1u
#define ADC_CH_RIGHT 0u

static inline void adc_gpio_init_raw(uint gpio) {
  io_bank0_hw->io[gpio].ctrl = GPIO_FUNC_NULL << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
  pads_bank0_hw->io[gpio] &= ~(PADS_BANK0_GPIO0_IE_BITS | PADS_BANK0_GPIO0_PUE_BITS | PADS_BANK0_GPIO0_PDE_BITS);
}

static inline uint16_t adc_read_channel(uint8_t chan) {
  uint32_t cs = adc_hw->cs;
  cs &= ~ADC_CS_AINSEL_BITS; //clear channel select
  cs |= (uint32_t)chan << ADC_CS_AINSEL_LSB; // set new channel select
  cs |= ADC_CS_EN_BITS; // enable adc
  adc_hw->cs = cs | ADC_CS_START_ONCE_BITS; //write the new control back along with the start bits/command

  while ((adc_hw->cs & ADC_CS_READY_BITS) == 0u) {
    __asm volatile("nop");
  }
  return (uint16_t)adc_hw->result;
}

void TCRT5000_init(void) {
  adc_gpio_init_raw(A0LEFT);
  adc_gpio_init_raw(A0RIGHT);

  adc_hw->cs = ADC_CS_EN_BITS; //enable adc
  adc_hw->fcs = 0u; // disable fifo, we re doing single reads for now.
}

uint16_t TCRT5000_read_left_raw(void) { return adc_read_channel(ADC_CH_LEFT); }
uint16_t TCRT5000_read_right_raw(void) { return adc_read_channel(ADC_CH_RIGHT); }

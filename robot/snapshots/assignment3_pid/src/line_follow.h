#ifndef LINE_FOLLOW_H
#define LINE_FOLLOW_H

#include <stdint.h>

typedef struct {
  uint16_t left_raw;
  uint16_t right_raw;
  float error;
  float correction;
  int16_t left_speed;
  int16_t right_speed;
} line_follow_debug_t;

void line_follow_init(void);
void line_follow_step(line_follow_debug_t *debug);

#endif

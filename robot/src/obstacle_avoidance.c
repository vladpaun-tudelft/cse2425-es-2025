#include "obstacle_avoidance.h"

#include "motors_pwm.h"
#include "timers.h"

void obstacle_avoidance_run(const hcsr04_config_t *front_cfg,
                            const hcsr04_config_t *side_cfg) {
  (void)front_cfg;
  (void)side_cfg;

  motors_pwm_drive_lr_signed(0, 0);
  busy_wait_ms_hw(2000u);
}

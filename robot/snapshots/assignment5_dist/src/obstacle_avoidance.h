#pragma once

#include "HCSR04.h"

void obstacle_avoidance_run(const hcsr04_config_t *front_cfg,
                            const hcsr04_config_t *side_cfg);

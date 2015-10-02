#ifndef CONFLICT_MASK_H
#define CONFLICT_MASK_H

#include <stdbool.h>

#include "schedule.h"


extern tl_config_t tl_conflict_mask[TL_COUNT];
extern bool tl_config_valid[1 << TL_COUNT];


void build_conflict_mask(void);

#endif

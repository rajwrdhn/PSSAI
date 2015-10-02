#ifndef TRAMBUS_H
#define TRAMBUS_H

#include "schedule.h"


// Bitmask for each time unit of the day which traffic light states are important
// regarding the trams/busses
extern tl_config_t trambus_day_tl_mask[86400];
// Value the traffic light state bit field is supposed to have after applying the
// bitmask
extern tl_config_t trambus_day_tl_value[86400];


void build_trambus_schedules(void);

#endif

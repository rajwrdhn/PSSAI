#include "assert.h"
#include "conflict_mask.h"
#include "schedule.h"
#include "trambus.h"


tl_config_t trambus_day_tl_mask[86400] =
#include "trambus_mask.h"
;

tl_config_t trambus_day_tl_value[86400] =
#include "trambus_value.h"
;


void build_trambus_schedules(void)
{
    for (int i = 0; i < 86400; i++) {
        assert(!(trambus_day_tl_value[i] & ~trambus_day_tl_mask[i]));
        assert(tl_config_valid[trambus_day_tl_value[i]]);
    }
}

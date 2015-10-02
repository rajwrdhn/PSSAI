#include <assert.h>
#include <stdbool.h>

#include "conflict_mask.h"
#include "schedule.h"


tl_config_t tl_conflict_mask[TL_COUNT];
bool tl_config_valid[1 << TL_COUNT];


void build_conflict_mask(void)
{
    /*
     * Every straight/right TL:
     * - conflicts with every straight/right, except the opposite
     * - conflicts with every left turn, except its own
     * - conflicts with the opposite road's pedestrian TL
     * Every left turn TL:
     * - conflicts with every straight/right, except its own
     * - conflicts with every left turn, except the opposite
     * - conflicts with the target road's pedestrian TL
     */

    // Straight/right car traffic lights
    for (int i = 0; i < 4; i++) {
        // conflict masks
        int straight = 0xf, left = 0xf, pedestrian = 0xf;

        // Cannot conflict with itself
        straight &= ~(1 << i);
        // Does not conflict with opposite
        straight &= ~(1 << ((i + 2) % 4));

        // Does not conflict with own left turn
        left &= ~(1 << i);

        // Conflicts with parallel roads' pedestrian TLs
        pedestrian &= ~(1 << ((i + 1) % 4));
        pedestrian &= ~(1 << ((i + 3) % 4));

        tl_conflict_mask[i] =  straight
                            | (left << 4)
                            | (pedestrian << 8);
    }

    // Left turn car traffic lights
    for (int i = 0; i < 4; i++) {
        int straight = 0xf, left = 0xf, pedestrian = 0xf;

        // Does not conflict with own straight/right
        straight &= ~(1 << i);

        // Cannot conflict with itself
        left &= ~(1 << i);
        // Does not conflict with opposite
        left &= ~(1 << ((i + 2) % 4));

        // Does not conflict with the opposite road or the one to the right
        pedestrian &= ~(1 << ((i + 1) % 4));
        pedestrian &= ~(1 << ((i + 2) % 4));

        tl_conflict_mask[i + 4] =  straight
                                | (left << 4)
                                | (pedestrian << 8);
    }

    // Pedestrian traffic lights
    for (int i = 0; i < 4; i++) {
        // They don't conflict with each other; so just collect the car TL
        // conflicts from what we've built already
        for (int j = 0; j < 8; j++) {
            if (tl_conflict_mask[j] & (1 << (i + 8))) {
                tl_conflict_mask[i + 8] |= 1 << j;
            }
        }
    }

    // Sanity check: Check symmetry
    for (int i = 0; i < TL_COUNT; i++) {
        for (int j = 0; j < TL_COUNT; j++) {
            assert(((tl_conflict_mask[i] >> j) & 1) == ((tl_conflict_mask[j] >> i) & 1));
        }
    }

    // Build tl_config_valid
    for (int i = 0; i < (1 << TL_COUNT); i++) {
        tl_config_valid[i] = true;
        for (int j = 0; j < 12; j++) {
            if ((i & (1 << j)) && (i & tl_conflict_mask[j])) {
                tl_config_valid[i] = false;
                break;
            }
        }
    }
}

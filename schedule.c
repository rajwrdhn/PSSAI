#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conflict_mask.h"
#include "schedule.h"
#include "trambus.h"


#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))

#define SECONDS_PER_CAR 2.f


const int tlp_min_green_time[TL_COUNT] = {
    0, 0, 0, 0,
    0, 0, 0, 0,

    22, // east:  21.45 s
    14, // north: 13.50 s
    20, // west:  19.20 s
    17  // south: 16.80 s
};

const float tlc_car_count[TLC_COUNT] = {
     6.40f,
     8.80f,
    12.78f,
    14.20f,
     3.00f,
    11.25f,
    10.30f,
     5.60f
};

const char *const tl_name[TL_COUNT] = {
    "east straight",
    "north straight",
    "west straight",
    "south straight",

    "east left",
    "north left",
    "west left",
    "south left",

    "east ped.",
    "north ped.",
    "west ped.",
    "south ped."
};


Schedule *new_schedule(int seconds, int start_time, bool is_rush)
{
    Schedule *sched = calloc(1, sizeof(*sched));

    sched->length       = seconds;
    sched->data         = calloc(sched->length, sizeof(*sched->data));
    sched->start_index  = start_time;
    sched->end_index    = sched->start_index + sched->length;
    sched->is_rush      = is_rush;

    return sched;
}


void schedule_destroy(Schedule *sched)
{
    free(sched->data);
    free(sched);
}


Schedule *schedule_copy(const Schedule *sched)
{
    Schedule *ns = calloc(1, sizeof(*ns));

    ns->length      = sched->length;
    ns->start_index = sched->start_index;
    ns->end_index   = sched->end_index;
    ns->is_rush     = sched->is_rush;

    ns->data = malloc(ns->length * sizeof(*ns->data));
    memcpy(ns->data, sched->data, ns->length * sizeof(*ns->data));

    return ns;
}


static void schedule_make_green(Schedule *sched, int timep, int tl)
{
    // Make it green
    sched->data[timep] |= (1 << tl);
    // Make all conflicting lights red
    sched->data[timep] &= ~tl_conflict_mask[tl];
}


bool schedule_initial_constraints(Schedule *sched)
{
    /*
     * c) Between switching of green phases consider 3 sec of red phase for
     *    all directions.
     * d) Consider a maximum waiting time of 2 min always
     */

    int i = sched->start_index;
    int last_index = i;
    int last_config = 0;

    while (i < sched->end_index) {
        while (i < sched->end_index && !trambus_day_tl_value[i]) {
            i++;
        }

        if (i >= sched->end_index) {
            break;
        }

        int this_config;
        if (trambus_day_tl_value[i] & ((1 << TLC_EAST_SR) | (1 << TLC_WEST_SR))) {
            this_config = 0;
        } else if (trambus_day_tl_value[i] & ((1 << TLC_EAST_L) | (1 << TLC_WEST_L))) {
            this_config = 1;
        } else if (trambus_day_tl_value[i] & ((1 << TLC_NORTH_SR) | (1 << TLC_SOUTH_SR))) {
            this_config = 2;
        } else if (trambus_day_tl_value[i] & ((1 << TLC_NORTH_L) | (1 << TLC_SOUTH_L))) {
            this_config = 3;
        } else {
            assert(0);
            return false;
        }

        int duration = i - last_index;
        int changes = ((this_config - last_config) % 4 + 4) % 4;

        if (!duration && !changes) {
            i++;
            continue;
        }

        while (!changes || ((float)duration / changes > 30.f)) {
            changes += 4;
        }

        int minimal_duration = changes / 4 * 53;
        for (int cycle_i = last_config; cycle_i < last_config + changes % 4; cycle_i++) {
            if (cycle_i % 4 == 0) {
                minimal_duration += 20;
            } else if (cycle_i % 4 == 2) {
                minimal_duration += 25;
            } else {
                minimal_duration += 4;
            }
        }

        if (duration < minimal_duration) {
            fprintf(stderr, "Error building initial schedule\n");
            fprintf(stderr, "%i %i %f %i\n", duration, changes, (float)duration / changes, minimal_duration);
            fprintf(stderr, "%02i:%02i:%02i\n", i / 3600, (i / 60) % 60, i % 60);
            return false;
        }

        int durations[changes];
        float avg_duration = (float)duration / changes;
        int remaining_duration = duration;

        if (avg_duration >= 25.f) {
            for (int j = 0; j < changes; j++) {
                durations[j] = lrint((float)remaining_duration / (changes - j));
                remaining_duration -= durations[j];
            }
        } else if (changes >= 4) {
            int rel_i = changes / 4 * 4;
            for (int cycle_i = last_config; cycle_i < last_config + changes % 4; cycle_i++) {
                if (cycle_i % 4 == 0) {
                    durations[rel_i] = 20;
                } else if (cycle_i % 4 == 2) {
                    durations[rel_i] = 25;
                } else {
                    durations[rel_i] = 4;
                }
                remaining_duration -= durations[rel_i];
                rel_i++;
            }

            int config = last_config;
            int full_cycle_changes = changes / 4 * 4;

            for (int j = 0; j < full_cycle_changes; j++) {
                avg_duration = (float)remaining_duration / (full_cycle_changes - j);
                if (avg_duration >= 25.f) {
                    durations[j] = lrint(avg_duration);
                } else {
                    minimal_duration = remaining_duration - 30 * (full_cycle_changes - j - 1);
                    if (config == 0) {
                        durations[j] = max(minimal_duration, 20);
                    } else if (config == 2) {
                        durations[j] = max(minimal_duration, 25);
                    } else {
                        durations[j] = max(minimal_duration, 4);
                    }
                }
                remaining_duration -= durations[j];
                config = (config + 1) % 4;
            }
        } else {
            int rel_i = 0;
            for (int cycle_i = last_config; cycle_i < last_config + changes; cycle_i++) {
                if (cycle_i % 4 == 0) {
                    durations[rel_i] = 20;
                } else if (cycle_i % 4 == 2) {
                    durations[rel_i] = 25;
                } else {
                    durations[rel_i] = 4;
                }
                remaining_duration -= durations[rel_i];
                rel_i++;
            }

            durations[rel_i - 1] += remaining_duration;
        }

        int config = last_config;
        int timep = last_index - sched->start_index;

        for (int j = 0; j < changes; j++) {
            for (int k = 0; k < durations[j] - 3; k++) {
                if (config == 0) {
                    schedule_make_green(sched, timep + k, TLC_EAST_SR);
                    schedule_make_green(sched, timep + k, TLC_WEST_SR);
                    schedule_make_green(sched, timep + k, TLP_NORTH);
                    schedule_make_green(sched, timep + k, TLP_SOUTH);
                } else if (config == 1) {
                    schedule_make_green(sched, timep + k, TLC_EAST_L);
                    schedule_make_green(sched, timep + k, TLC_WEST_L);
                } else if (config == 2) {
                    schedule_make_green(sched, timep + k, TLC_SOUTH_SR);
                    schedule_make_green(sched, timep + k, TLC_NORTH_SR);
                    schedule_make_green(sched, timep + k, TLP_EAST);
                    schedule_make_green(sched, timep + k, TLP_WEST);
                } else {
                    assert(config == 3);
                    schedule_make_green(sched, timep + k, TLC_NORTH_L);
                    schedule_make_green(sched, timep + k, TLC_SOUTH_L);
                }
            }

            config = (config + 1) % 4;
            timep += durations[j];
        }

        assert(config == this_config);
        assert(sched->start_index + timep == i);

        last_config = this_config;
        last_index = i++;
    }


    // And now enter the last one
    int config = last_config;
    int timep = last_index - sched->start_index;
    while (timep < sched->length) {
        for (int i = 0; i < min(sched->length - timep, 27); i++) {
            if (config == 0) {
                schedule_make_green(sched, timep + i, TLC_EAST_SR);
                schedule_make_green(sched, timep + i, TLC_WEST_SR);
                schedule_make_green(sched, timep + i, TLP_NORTH);
                schedule_make_green(sched, timep + i, TLP_SOUTH);
            } else if (config == 1) {
                schedule_make_green(sched, timep + i, TLC_EAST_L);
                schedule_make_green(sched, timep + i, TLC_WEST_L);
            } else if (config == 2) {
                schedule_make_green(sched, timep + i, TLC_SOUTH_SR);
                schedule_make_green(sched, timep + i, TLC_NORTH_SR);
                schedule_make_green(sched, timep + i, TLP_EAST);
                schedule_make_green(sched, timep + i, TLP_WEST);
            } else {
                assert(config == 3);
                schedule_make_green(sched, timep + i, TLC_NORTH_L);
                schedule_make_green(sched, timep + i, TLC_SOUTH_L);
            }
        }

        config = (config + 1) % 4;
        timep += 30;
    }

    if (!schedule_satisfied(sched, true)) {
        fprintf(stderr, "Error: Initial schedule does not satisfy constraints\n");
        return false;
    }

    return true;
}


bool schedule_satisfied(const Schedule *sched, bool verbose)
{
    int green_time[TL_COUNT] = {0};
    int red_time[TL_COUNT] = {0};
    int last_configuration = -1;
    int first_totally_red = -1;

    for (int i = 0; i < sched->length; i++) {
        int configuration = sched->data[i];

        /*
         * a) Pedestrian lights need a minimal time of green phase
         * Note: We don't have to care about the red phase for pedestrian TLs;
         * just make the final ten seconds of their green phase red. It's the
         * same thing.
         * d) Consider a maximum waiting time of 2 min always
         */
        for (int j = 0; j < TL_COUNT; j++) {
            if (configuration & (1 << j)) {
                green_time[j]++;
                red_time[j] = 0;
            } else {
                if (j > TLC_COUNT && green_time[j] > 0 &&
                    green_time[j] < tlp_min_green_time[j])
                {
                    if (verbose) {
                        fprintf(stderr, "Error: Schedule does not satisfy constraint A\n");
                    }
                    return false;
                }

                if (++red_time[j] > 120) {
                    if (verbose) {
                        fprintf(stderr, "Error: Schedule does not satisfy constraint D\n");
                    }
                    return false;
                }
            }
        }

        /*
         * 0) Two conflicting directions cannot be green at the same time
         * b) It is not allowed to have green lights for pedestrians when the
         *    cars can cross their way, except if both go in the same direction.
         */
        if (!tl_config_valid[configuration]) {
            if (verbose) {
                fprintf(stderr, "Error: Schedule does not satisfy contraints 0/B\n");
            }
            return false;
        }

        /*
         * c) Between switching of green phases consider 3 sec of red phase for
         *    all directions.
         * (Note: This breaks condition "there needs to be a green light
         *  anywhere", so we'll just ignore that one)
         */
        if (last_configuration >= 0 && configuration != last_configuration) {
            if (last_configuration) {
                if (configuration) {
                    if (verbose) {
                        fprintf(stderr, "Error: Schedule does not satisfy constraint C\n");
                        fprintf(stderr, "(Immediate green to green switch)\n");
                    }
                    return false;
                }
                first_totally_red = i;
            } else {
                if (i - first_totally_red < 3) {
                    if (verbose) {
                        fprintf(stderr, "Error: Schedule does not satisfy constraint C\n");
                        fprintf(stderr, "(Intermediate red phase too short)\n");
                    }
                    return false;
                }
            }
        }
        last_configuration = configuration;

        /*
         * e) The Buses and Trams should always pass when they are scheduled
         * (Note: Since the interpretation "always pass exactly when they are
         *  scheduled" fits this constraint, and it hopefully makes things
         *  easier, we are going to employ it, even though it is pretty stupid
         *  (a realistic interpretation would be "should be able to pass within a
         *   10 second window 30 seconds after the scheduled time"))
         */
        if ((configuration & trambus_day_tl_mask[i + sched->start_index])
            != trambus_day_tl_value[i + sched->start_index])
        {
            if (verbose) {
                fprintf(stderr, "Error: Schedule does not satisfy constraint E\n");
            }
            return false;
        }
    }

    return true;
}


bool schedule_mutate(Schedule *sched, int config_index, bool shorten)
{
    int array_index;

    if (config_index < 0) {
        array_index = rand() % sched->length;
        shorten = rand() % 2;
    } else {
        int current_config_index = 0;
        bool in_red_phase = sched->data[0] == 0;
        for (array_index = 0;
             current_config_index != config_index
             && array_index < sched->length;
             array_index++)
        {
            if (sched->data[array_index] == 0 && !in_red_phase) {
                in_red_phase = true;
            } else if (sched->data[array_index] != 0 && in_red_phase) {
                current_config_index++;
                in_red_phase = false;
            }
        }

        if (current_config_index != config_index) {
            return false;
        }
    }

    while (array_index < sched->length && sched->data[array_index] == 0) {
        array_index++;
    }

    /*
     * If we have a red phase at the end of the schedule, decrease the index
     * by 3 + 1, since no red phase lasts longer than three seconds
     */
    if (array_index >= sched->length) {
        array_index = sched->length - 3 - 1;
    }

    int configuration = sched->data[array_index];

    int index = array_index;
    while (index < sched->length && sched->data[index] != 0) {
        index++;
    }
    if (index >= sched->length) {
        return false;
    }

    if (shorten) {
        sched->data[index - 1] = 0;
        if (index + 3 < sched->length) {
            sched->data[index + 3 - 1] = sched->data[index + 3];
        }
    } else {
        sched->data[index] = configuration;
        if (index + 3 < sched->length) {
            sched->data[index + 3] = 0;
        }
    }

    return true;
}


static void schedule_evaluate_rush_hour(Schedule *sched)
{
    int no_of_seconds_green_phase[TLC_COUNT] = {0};

    sched->evaluation.total = 0.f;
    sched->evaluation.bbs_ss = 0.f;

    for (int i = 0; i < sched->length; i++) {
        for (int tl = 0; tl < TLC_COUNT; tl++) {
            if (sched->data[i] & (1 << tl)) {
                no_of_seconds_green_phase[tl]++;
            } else if (no_of_seconds_green_phase[tl] > 0) {
                float cars = no_of_seconds_green_phase[tl] / SECONDS_PER_CAR;
                cars = min(cars, tlc_car_count[tl]);
                sched->evaluation.total += cars;
                if ((tl % 2) == 0) {
                    sched->evaluation.bbs_ss += cars;
                }
                no_of_seconds_green_phase[tl] = 0;
            }
        }
    }
}


static void schedule_evaluate_non_rush_hour(Schedule *sched)
{
    int no_of_seconds_red_phase[TLC_COUNT] = {0};
    struct {
        int total, bbs_ss;
    } total_red_time = {0}, red_phase_count = {0};

    for (int i = 0; i < sched->length; i++) {
        for (int tl = 0; tl < TLC_COUNT; tl++) {
            if (!(sched->data[i] & (1 << tl))) {
                no_of_seconds_red_phase[tl]++;
            } else if (no_of_seconds_red_phase[tl] > 0) {
                total_red_time.total += no_of_seconds_red_phase[tl];
                red_phase_count.total++;
                if ((tl % 2) == 0) {
                    total_red_time.bbs_ss += no_of_seconds_red_phase[tl];
                    red_phase_count.bbs_ss++;
                }
                no_of_seconds_red_phase[tl] = 0;
            }
        }
    }

    // Negative average waiting time; negative because this is going to be
    // maximized.
    sched->evaluation.total = -(float)total_red_time.total
                            / red_phase_count.total;
    sched->evaluation.bbs_ss = -(float)total_red_time.bbs_ss
                             / red_phase_count.bbs_ss;
}


void schedule_evaluate(Schedule *sched)
{
    if (sched->is_rush) {
        schedule_evaluate_rush_hour(sched);
    } else {
        schedule_evaluate_non_rush_hour(sched);
    }
}


float schedule_compare(const Schedule *o, const Schedule *n)
{
    if (o->evaluation.total != n->evaluation.total) {
        return o->evaluation.total - n->evaluation.total;
    } else {
        // FIXME: Hardcoding the divisor is ugly
        if (o->is_rush) {
            // For rush hours, we count cars; those get up to a couple thousand.
            return (o->evaluation.bbs_ss - n->evaluation.bbs_ss) * 1.e-4f;
        } else {
            // For non-rush-hours, we count time; that gets up to below 100.
            return (o->evaluation.bbs_ss - n->evaluation.bbs_ss) * 1.e-2f;
        }
    }
}


void schedule_print(const Schedule *sched, FILE *fp)
{
    int config = 0;

    for (int i = 0; i < sched->length; i++) {
        if (sched->data[i] != config) {
            int timep = i + sched->start_index;

            fprintf(fp, "%02i:%02i:%02i: [", timep / 3600, (timep / 60) % 60, timep % 60);

            bool first = true, to_green = false;
            for (int tl = 0; tl < TL_COUNT; tl++) {
                if ((sched->data[i] ^ config) & (1 << tl)) {
                    fprintf(fp, "%s%s", first ? "" : ", ", tl_name[tl]);
                    first = false;
                    to_green = sched->data[i] & (1 << tl);
                }
            }

            fprintf(fp, "] to %s\n", to_green ? "green" : "red");

            config = sched->data[i];
        }
    }
}


void schedule_print_evaluation(const Schedule *sched, FILE *fp)
{
    if (sched->is_rush) {
        fprintf(fp, "Total cars passing: %f\n", sched->evaluation.total);
        fprintf(fp, "Cars on BBS/SS:     %f\n", sched->evaluation.bbs_ss);
    } else {
        fprintf(fp, "Average waiting time: %f\n", -sched->evaluation.total);
        fprintf(fp, "On BBS/SS:            %f\n", -sched->evaluation.bbs_ss);
    }
}

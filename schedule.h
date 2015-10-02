#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>


typedef uint16_t tl_config_t;

typedef struct Schedule {
    tl_config_t *data;
    int length;
    int start_index, end_index;
    bool is_rush;

    struct {
        float total, bbs_ss;
    } evaluation;
} Schedule;


enum {
    TLC_EAST_SR,
    TLC_NORTH_SR,
    TLC_WEST_SR,
    TLC_SOUTH_SR,

    TLC_EAST_L,
    TLC_NORTH_L,
    TLC_WEST_L,
    TLC_SOUTH_L,

    TLC_COUNT,

    TLP_EAST = TLC_COUNT,
    TLP_NORTH,
    TLP_WEST,
    TLP_SOUTH,

    TL_COUNT
};

#define TLP_COUNT (TL_COUNT - TLC_COUNT)


Schedule *new_schedule(int seconds, int start_time, bool is_rush);
void schedule_destroy(Schedule *sched);

Schedule *schedule_copy(const Schedule *sched);

bool schedule_initial_constraints(Schedule *sched);
bool schedule_satisfied(const Schedule *sched, bool verbose);

bool schedule_mutate(Schedule *sched, int config_index, bool shorten);
void schedule_evaluate(Schedule *sched);
float schedule_compare(const Schedule *o, const Schedule *n);

void schedule_print(const Schedule *sched, FILE *fp);
void schedule_print_evaluation(const Schedule *sched, FILE *fp);

#endif

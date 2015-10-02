#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "local_search.h"
#include "schedule.h"


static void true_local_search(Schedule **sched,
                              is_improvement_func_t *is_improvement,
                              void *is_improvement_opaque)
{
    int improvements_per_10000 = 0;

    schedule_initial_constraints(*sched);

    schedule_evaluate(*sched);

    for (int i = 0;; i++) {
        if (!(i % 10000)) {
            printf("[%i] (%i improvements)\n", i, improvements_per_10000);
            schedule_print_evaluation(*sched, stdout);
            printf("---\n");

            if (i) {
                if (improvements_per_10000 < 1) {
                    break;
                }
            }
            improvements_per_10000 = 0;
        }


        Schedule *new_sched = schedule_copy(*sched);

        schedule_mutate(new_sched, -1, false);
        if (!schedule_satisfied(new_sched, false)) {
            schedule_destroy(new_sched);
            continue;
        }

        schedule_evaluate(new_sched);
        float difference = schedule_compare(*sched, new_sched);
        if (!is_improvement(difference, i, is_improvement_opaque)) {
            schedule_destroy(new_sched);
            continue;
        }

        improvements_per_10000++;

        schedule_destroy(*sched);
        *sched = new_sched;
    }
}


static void hill_climber(Schedule **sched,
                         is_improvement_func_t *is_improvement,
                         void *is_improvement_opaque)
{
    schedule_initial_constraints(*sched);

    schedule_evaluate(*sched);

    for (int i = 0;; i++) {
        if (!(i % 100)) {
            printf("[%i]\n", i);
            schedule_print_evaluation(*sched, stdout);
            printf("---\n");
        }

        Schedule *best_sched = *sched;

        for (int j = 0;; j++) {
            Schedule *new_sched = schedule_copy(*sched);

            bool did_mutate = schedule_mutate(new_sched, j / 2, j % 2);
            if (!did_mutate) {
                schedule_destroy(new_sched);
                break;
            }

            if (!schedule_satisfied(new_sched, false)) {
                schedule_destroy(new_sched);
                continue;
            }

            schedule_evaluate(new_sched);
            float difference = schedule_compare(best_sched, new_sched);
            if (!is_improvement(difference, i, is_improvement_opaque)) {
                schedule_destroy(new_sched);
                continue;
            }

            if (best_sched != *sched) {
                schedule_destroy(best_sched);
            }
            best_sched = new_sched;
        }

        if (best_sched == *sched) {
            break;
        }

        schedule_destroy(*sched);
        *sched = best_sched;
    }
}


void local_search(Schedule **sched, bool whole_neighborhood,
                  is_improvement_func_t *is_improvement,
                  void *is_improvement_opaque)
{
    if (whole_neighborhood) {
        hill_climber(sched, is_improvement, is_improvement_opaque);
    } else {
        true_local_search(sched, is_improvement, is_improvement_opaque);
    }
}


// Returns a value in the interval [0, 1)
static float randf(void)
{
    return (float)rand() / ((unsigned)RAND_MAX + 1);
}


bool ls_plain(float difference, int iteration, void *opaque)
{
    (void)iteration;
    (void)opaque;
    return difference < 0.f;
}


void sim_anneal_initialize(SimAnneal *sa, float initial_temperature,
                           float base_temperature, float cooling_rate,
                           float cutoff)
{
    sa->initial_temperature = initial_temperature;
    sa->base_temperature = base_temperature;
    sa->cooling_rate = cooling_rate;
    sa->cutoff = cutoff;
}


bool ls_simulated_annealing(float difference, int iteration, void *opaque)
{
    SimAnneal *sa = opaque;

    float temperature = (sa->initial_temperature - sa->base_temperature)
                        * expf(-iteration * sa->cooling_rate)
                        + sa->base_temperature;
    if (temperature < sa->cutoff || difference < 0.f) {
        return difference < 0.f;
    }

    float p = 1.f / (1.f + expf(difference / temperature));
    return randf() < p;
}

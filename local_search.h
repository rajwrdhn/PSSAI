#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include "schedule.h"


typedef bool is_improvement_func_t(float difference, int iteration,
                                   void *opaque);


typedef struct SimAnneal {
    float initial_temperature, base_temperature, cooling_rate;
    float cutoff;
} SimAnneal;


/*
 * Plain hill-climbing local search.
 *
 * @param opaque: NULL.
 */
is_improvement_func_t ls_plain;

/*
 * Simulated annealing.
 *
 * @param opaque: SimAnneal object.
 */
is_improvement_func_t ls_simulated_annealing;


/*
 * @param sa                    Object to initialize.
 * @param initial_temperature   Initial temperature, i.e., initial "randomness".
 * @param base_temperature      The temperature to cool down to.
 * @param cooling_rate          Rate by which the temperature is decreased;
 *                              the smaller, the slower. Should be in (0, 1).
 * @param cutoff                When to return to standard hillclimbing; the
 *                              smaller, the later. Should be greater than
 *                              @base_temperature.
 */
void sim_anneal_initialize(SimAnneal *sa, float initial_temperature,
                           float base_temperature, float cooling_rate,
                           float cutoff);


/*
 * @param sched                 Schedule to improve.
 * @param whole_neighborhood    If true, the whole neighborhood of each schedule
 *                              will be scanned for the best improvement
 *                              (hill-climbing vs. local search).
 * @param is_improvement        Function which tells the local search whether a
 *                              certain difference is considered an improvement.
 * @param is_improvement_opaque Opaque parameter for that function.
 */
void local_search(Schedule **sched, bool whole_neighborhood,
                  is_improvement_func_t *is_improvement,
                  void *is_improvement_opaque);

#endif

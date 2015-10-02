#define _GNU_SOURCE

#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "conflict_mask.h"
#include "local_search.h"
#include "schedule.h"
#include "trambus.h"


const struct PartOfTheDay {
    const char *name;
    int start;
    int length;
    bool rush_hour;
} parts_of_the_day[] = {
    { "Morning",          ( 6 * 60 +  0) * 60, (1 * 60 + 30) * 60, false },
    { "First rush hour",  ( 7 * 60 + 30) * 60, (2 * 60 +  0) * 60,  true },
    { "Day",              ( 9 * 60 + 30) * 60, (7 * 60 + 30) * 60, false },
    { "Second rush hour", (17 * 60 +  0) * 60, (2 * 60 +  0) * 60,  true },
    { "Evening",          (19 * 60 +  0) * 60, (2 * 60 +  0) * 60, false },
};

#define ARRAY_SIZE(x) ((int)(sizeof(x) / sizeof((x)[0])))


static void usage(const char *argv0)
{
    fprintf(stderr, "Usage: %s [-o dir] [-c] [-s]\n", argv0);
    fprintf(stderr, "  -h, --help       Prints this text and exits.\n");
    fprintf(stderr, "  -o, --output=dir Writes each optimized schedule to a file in @dir.\n");
    fprintf(stderr, "  -c, --complete   Scans the whole neighborhood of a schedule instead of\n");
    fprintf(stderr, "                   selecting one neighbor at random.\n");
    fprintf(stderr, "  -s, --sim-anneal Uses simulated annealing instead of plain hill-climbing.\n");
}


int main(int argc, char *argv[])
{
    const char *target_dir = NULL;
    bool whole_neighborhood = false, sim_anneal = false;

    for (;;) {
        static const struct option options[] = {
            {"help", no_argument, NULL, 'h'},
            {"output", required_argument, NULL, 'o'},
            {"complete", no_argument, NULL, 'c'},
            {"sim-anneal", no_argument, NULL, 's'},

            {NULL, 0, NULL, 0}
        };

        int option = getopt_long(argc, argv, "ho:cs", options, NULL);
        if (option == -1) {
            break;
        }

        switch (option) {
            case 'h':
            case '?':
                usage(argv[0]);
                return 1;

            case 'o':
                target_dir = optarg;
                break;

            case 'c':
                whole_neighborhood = true;
                break;

            case 's':
                sim_anneal = true;
                break;
        }
    }

    srand(time(NULL));

    build_conflict_mask();
    build_trambus_schedules();

    for (int i = 0; i < ARRAY_SIZE(parts_of_the_day); i++) {
        const struct PartOfTheDay *potd = &parts_of_the_day[i];

        if (i) {
            printf("\n\n");
        }
        printf("Finding improved “%s” schedule:\n===\n", potd->name);

        Schedule *sched = new_schedule(potd->length, potd->start, potd->rush_hour);

        if (sim_anneal) {
            SimAnneal sa;
            sim_anneal_initialize(&sa, 42.f, 1.e-5f,
                                  whole_neighborhood ? 1.e-2f : 1.e-4f,
                                  1.001e-5f);

            local_search(&sched, whole_neighborhood,
                         ls_simulated_annealing, &sa);
        } else {
            local_search(&sched, whole_neighborhood, ls_plain, NULL);
        }

        if (target_dir) {
            char *name;
            int s = potd->start, e = potd->start + potd->length;
            int ret = asprintf(&name, "%s/%02i_%02i_%02i-%02i_%02i_%02i",
                               target_dir,
                               s / 3600, (s / 60) % 60, s % 60,
                               e / 3600, (e / 60) % 60, e % 60);
            if (ret < 0) {
                perror("asprintf() failed");
                return 1;
            }

            FILE *fp = fopen(name, "w");
            if (!fp) {
                fprintf(stderr, "Failed to open “%s”: %s\n", name, strerror(errno));
                return 1;
            }
            schedule_print(sched, fp);
            fclose(fp);
        }

        schedule_destroy(sched);
    }

    return 0;
}

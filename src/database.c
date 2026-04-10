#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "database.h"

#define MAX_GPU 50

gpu_entry gpu_db[MAX_GPU];
int gpu_db_size = 0;

cpu_entry cpu_db[] = {
    {"Intel i3", ENTRY},
    {"Intel i5", MID},
    {"Intel i7", HIGH},
    {"Intel i9", ENTHUSIAST},
    {"Ryzen 3", ENTRY},
    {"Ryzen 5", MID},
    {"Ryzen 7", HIGH},
    {"Ryzen 9", ENTHUSIAST}
};

const int cpu_db_size = sizeof(cpu_db) / sizeof(cpu_db[0]);

static tier parse_tier(const char *t) {
    if (strcmp(t, "ENTRY") == 0) return ENTRY;
    if (strcmp(t, "MID") == 0) return MID;
    if (strcmp(t, "HIGH") == 0) return HIGH;
    if (strcmp(t, "ENTHUSIAST") == 0) return ENTHUSIAST;
    return UNKNOWN_TIER;
}

void load_gpu_data(const char *filename) {
    FILE *fp = fopen(filename, "r");
    char line[256];

    if (!fp) {
        printf("Error: could not open %s\n", filename);
        return;
    }

    gpu_db_size = 0;

    while (fgets(line, sizeof(line), fp)) {
        char *model;
        char *tier_str;
        char *intel;
        char *amd;

        model = strtok(line, "|");
        tier_str = strtok(NULL, "|");
        intel = strtok(NULL, "|");
        amd = strtok(NULL, "\n");

        if (!model || !tier_str || !intel || !amd) {
            continue;
        }

        if (gpu_db_size >= MAX_GPU) {
            break;
        }

        strncpy(gpu_db[gpu_db_size].model, model, sizeof(gpu_db[gpu_db_size].model) - 1);
        gpu_db[gpu_db_size].model[sizeof(gpu_db[gpu_db_size].model) - 1] = '\0';

        gpu_db[gpu_db_size].performance_tier = parse_tier(tier_str);

        strncpy(gpu_db[gpu_db_size].recommended_intel, intel,
                sizeof(gpu_db[gpu_db_size].recommended_intel) - 1);
        gpu_db[gpu_db_size].recommended_intel[sizeof(gpu_db[gpu_db_size].recommended_intel) - 1] = '\0';

        strncpy(gpu_db[gpu_db_size].recommended_amd, amd,
                sizeof(gpu_db[gpu_db_size].recommended_amd) - 1);
        gpu_db[gpu_db_size].recommended_amd[sizeof(gpu_db[gpu_db_size].recommended_amd) - 1] = '\0';

        gpu_db_size++;
    }

    fclose(fp);
}
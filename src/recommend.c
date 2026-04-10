#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "recommend.h"
#include "database.h"

static void build_gpu_model_string(const gpu *g, char *out, int size) {
    char series_upper[20];
    char suffix_upper[20];
    int i;

    strncpy(series_upper, g->series, sizeof(series_upper) - 1);
    series_upper[sizeof(series_upper) - 1] = '\0';

    for (i = 0; series_upper[i] != '\0'; i++) {
        series_upper[i] = (char)toupper((unsigned char)series_upper[i]);
    }

    strncpy(suffix_upper, g->suffix, sizeof(suffix_upper) - 1);
    suffix_upper[sizeof(suffix_upper) - 1] = '\0';

    for (i = 0; suffix_upper[i] != '\0'; i++) {
        suffix_upper[i] = (char)toupper((unsigned char)suffix_upper[i]);
    }

    if (strcmp(g->suffix, "none") == 0) {
        snprintf(out, size, "%s %d", series_upper, g->type);
    } else {
        snprintf(out, size, "%s %d %s", series_upper, g->type, suffix_upper);
    }
}

void recommend_gpu_for_cpu(const cpu *c) {
    tier target = cpu_to_tier(c);

    if (target == UNKNOWN_TIER) {
        printf("Unable to determine CPU tier.\n");
        return;
    }

    printf("Recommended GPUs for a %s CPU:\n", tier_to_string(target));

    for (int i = 0; i < gpu_db_size; i++) {
        if (gpu_db[i].performance_tier == target) {
            printf("- %s\n", gpu_db[i].model);
        }
    }
}

void recommend_cpu_for_gpu(const gpu *g) {
    char model[100];
    int found = 0;
    tier target = gpu_to_tier(g);

    build_gpu_model_string(g, model, sizeof(model));

    for (int i = 0; i < gpu_db_size; i++) {
        if (case_equal(gpu_db[i].model, model)) {
            printf("Recommended CPUs for %s:\n", gpu_db[i].model);
            printf("  Intel: %s\n", gpu_db[i].recommended_intel);
            printf("  AMD:   %s\n", gpu_db[i].recommended_amd);
            found = 1;
            break;
        }
    }

    if (!found) {
        if (target == UNKNOWN_TIER) {
            printf("Unable to determine GPU tier.\n");
            return;
        }

        printf("Exact GPU model not found. Showing tier-based recommendations for this %s GPU:\n",
        tier_to_string(target));

        for (int i = 0; i < gpu_db_size; i++) {
            if (gpu_db[i].performance_tier == target) {
                printf("- %s\n", gpu_db[i].model);
                printf("  Intel: %s\n", gpu_db[i].recommended_intel);
                printf("  AMD:   %s\n", gpu_db[i].recommended_amd);
            }
        }
    }
}

void analyze_pairing(const cpu *c, const gpu *g) {
    tier cpu_t = cpu_to_tier(c);
    tier gpu_t = gpu_to_tier(g);
    int cpu_score = cpu_model_score(c);
    int gpu_score = gpu_model_score(g);
    int diff = cpu_score - gpu_score;

    if (cpu_t == UNKNOWN_TIER || gpu_t == UNKNOWN_TIER) {
        printf("Unable to analyze pairing.\n");
        return;
    }

    printf("Pairing analysis:\n");
    printf("CPU tier: %s\n", tier_to_string(cpu_t));
    printf("GPU tier: %s\n", tier_to_string(gpu_t));

    if (diff == 0) {
        printf("Verdict: Balanced pairing.\n");
        printf("This CPU and GPU are well matched for a balanced build.\n");
    } else if (diff <= -2) {
        printf("Verdict: Significant CPU bottleneck risk.\n");
        printf("The GPU is much stronger than the CPU.\n");
        printf("Consider upgrading to a higher-tier CPU for better balance.\n");
    } else if (diff == -1) {
        printf("Verdict: Mild CPU bottleneck risk.\n");
        printf("The GPU is slightly stronger than the CPU.\n");
        printf("The pairing may still work well, but some performance could be limited.\n");
    } else if (diff >= 2) {
        printf("Verdict: Significant CPU overkill.\n");
        printf("The CPU is much stronger than needed for this GPU.\n");
        printf("A lower-tier CPU could likely provide similar practical results.\n");
    } else if (diff == 1) {
        printf("Verdict: Slight CPU overkill.\n");
        printf("The CPU is somewhat stronger than the GPU requires.\n");
        printf("This is still a reasonable pairing, but it is not perfectly balanced.\n");
    }
}
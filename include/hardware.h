#ifndef HARDWARE_H
#define HARDWARE_H

typedef enum {
    ENTRY,
    MID,
    HIGH,
    ENTHUSIAST,
    UNKNOWN_TIER
} tier;

typedef struct {
    char brand[10];
    char family[10];   // i / ryzen
    int series;        // 3 / 5 / 7 / 9
    int model;         // 12400 / 12600 / 7600 / 7700
    char suffix[10];   // f / k / x / none
} cpu;

typedef struct {
    char series[10];
    int type;
    char suffix[10];
} gpu;

typedef struct {
    char model[50];
    tier performance_tier;
    char recommended_intel[100];
    char recommended_amd[100];
} gpu_entry;

typedef struct {
    char model[50];
    tier performance_tier;
} cpu_entry;

int case_equal(const char *a, const char *b);
void to_lower_str(char *s);
void clear_line(void);

int choose_direction(void);
void for_cpu(cpu *c);
void for_gpu(gpu *g);

tier cpu_to_tier(const cpu *c);
tier gpu_to_tier(const gpu *g);
const char *tier_to_string(tier t);

int tier_to_score(tier t);
int cpu_model_score(const cpu *c);
int gpu_model_score(const gpu *g);

#endif
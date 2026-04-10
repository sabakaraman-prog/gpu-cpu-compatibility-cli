#ifndef RECOMMEND_H
#define RECOMMEND_H

#include "hardware.h"

void recommend_gpu_for_cpu(const cpu *c);
void recommend_cpu_for_gpu(const gpu *g);
void print_bottleneck_warning(tier cpu_t, tier gpu_t);
void analyze_pairing(const cpu *c, const gpu *g);

#endif
#ifndef DATABASE_H
#define DATABASE_H

#include "hardware.h"

extern gpu_entry gpu_db[];
extern int gpu_db_size;

extern cpu_entry cpu_db[];
extern const int cpu_db_size;

void load_gpu_data(const char *filename);

#endif
#include <stdio.h>
#include "hardware.h"
#include "recommend.h"
#include "database.h"

int main(void) {
    load_gpu_data("gpu_data.txt");
    cpu c = {0};
    gpu g = {0};

    int choice = choose_direction();

    if (choice == 1) {
        for_cpu(&c);
        recommend_gpu_for_cpu(&c);
    } else if (choice == 2) {
        for_gpu(&g);
        recommend_cpu_for_gpu(&g);
    } else if (choice == 3) {
        for_cpu(&c);
        for_gpu(&g);
        analyze_pairing(&c, &g);
    } else {
        printf("Invalid selection.\n");
    }

    return 0;
}
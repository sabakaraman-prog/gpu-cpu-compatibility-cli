#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "hardware.h"

static int parse_cpu_model(const char *line, cpu *c);

int case_equal(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

void to_lower_str(char *s) {
    while (*s) {
        *s = (char)tolower((unsigned char)*s);
        s++;
    }
}

void clear_line(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

int choose_direction(void) {
    int choice;

    printf("What do you want to do?\n");
    printf("1) I have a CPU -> recommend GPU\n");
    printf("2) I have a GPU -> recommend CPU\n");
    printf("3) I have both a CPU and GPU -> analyze pairing\n");
    printf("Enter choice: ");

    if (scanf("%d", &choice) != 1) {
        clear_line();
        return 0;
    }

    clear_line();
    return choice;
}

static int valid_cpu_model(const cpu *c) {
    if (case_equal(c->brand, "intel")) {
        if (c->model >= 10000 && c->model <= 14999) {
            return 1;
        }
        return 0;
    }

    if (case_equal(c->brand, "amd")) {
        if (c->model >= 1000 && c->model <= 9999) {
            return 1;
        }
        return 0;
    }

    return 0;
}

static int parse_cpu_series_token(const char *token, char *name_out, int *series_out) {
    char temp[32];

    strncpy(temp, token, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    to_lower_str(temp);

    if (temp[0] == 'i' && isdigit((unsigned char)temp[1])) {
        strcpy(name_out, "i");
        *series_out = temp[1] - '0';
        return 1;
    }

    if (strncmp(temp, "ryzen", 5) == 0 && isdigit((unsigned char)temp[5])) {
        strcpy(name_out, "ryzen");
        *series_out = temp[5] - '0';
        return 1;
    }

    return 0;
}

static int extract_suffix_from_stuck_number(const char *token, int *num_out, char *suffix_out) {
    int i = 0;
    char digits[16];

    while (isdigit((unsigned char)token[i])) {
        i++;
    }

    if (i == 0) {
        return 0;
    }

    memcpy(digits, token, i);
    digits[i] = '\0';
    *num_out = atoi(digits);

    if (token[i] == '\0') {
        strcpy(suffix_out, "none");
    } else {
        strncpy(suffix_out, token + i, 9);
        suffix_out[9] = '\0';
        to_lower_str(suffix_out);
    }

    return 1;
}

void for_cpu(cpu *c) {
    char line[128];

    printf("\n");
    printf("Which CPU do you have? (e.g., Intel i5 12400F, Ryzen 7 7700X): ");

    if (fgets(line, sizeof(line), stdin) == NULL) {
        printf("Input error.\n");
        c->brand[0] = '\0';
        return;
    }

    if (!parse_cpu_model(line, c)) {
        printf("Invalid CPU format.\n");
        c->brand[0] = '\0';
        return;
    }
}

void for_gpu(gpu *g) {
    char line[128];
    char a[32] = {0};
    char b[32] = {0};
    char c[32] = {0};
    int n;

    printf("\n");
    printf("Which GPU do you have? (e.g., RTX 4070 Ti): ");

    if (fgets(line, sizeof(line), stdin) == NULL) {
        printf("Input error.\n");
        g->series[0] = '\0';
        return;
    }

    n = sscanf(line, "%31s %31s %31s", a, b, c);

    if (n < 2) {
        printf("Invalid GPU format.\n");
        g->series[0] = '\0';
        return;
    }

    strncpy(g->series, a, sizeof(g->series) - 1);
    g->series[sizeof(g->series) - 1] = '\0';
    to_lower_str(g->series);

    to_lower_str(b);

    if (!extract_suffix_from_stuck_number(b, &g->type, g->suffix)) {
        printf("Invalid GPU number.\n");
        g->series[0] = '\0';
        return;
    }

    if (n == 3) {
        strncpy(g->suffix, c, sizeof(g->suffix) - 1);
        g->suffix[sizeof(g->suffix) - 1] = '\0';
        to_lower_str(g->suffix);
    }
}

tier cpu_to_tier(const cpu *c) {
    if (c->brand[0] == '\0') {
        return UNKNOWN_TIER;
    }

    if (c->series <= 3) {
        return ENTRY;
    } else if (c->series == 5) {
        return MID;
    } else if (c->series == 7) {
        return HIGH;
    } else if (c->series >= 9) {
        return ENTHUSIAST;
    }

    return UNKNOWN_TIER;
}

tier gpu_to_tier(const gpu *g) {
    int tier_num;

    if (g->series[0] == '\0') {
        return UNKNOWN_TIER;
    }

    tier_num = g->type % 100;

    if (tier_num == 0) {
        if (g->type >= 7900) {
            tier_num = 90;
        } else if (g->type >= 7800) {
            tier_num = 80;
        } else if (g->type >= 7700) {
            tier_num = 70;
        }
    }

    if (tier_num == 50) {
        return ENTRY;
    } else if (tier_num == 60) {
        return MID;
    } else if (tier_num == 70) {
        return HIGH;
    } else if (tier_num == 80 || tier_num == 90) {
        return ENTHUSIAST;
    }

    return UNKNOWN_TIER;
}

const char *tier_to_string(tier t) {
    if (t == ENTRY) {
        return "entry-level";
    } else if (t == MID) {
        return "midrange";
    } else if (t == HIGH) {
        return "high-end";
    } else if (t == ENTHUSIAST) {
        return "enthusiast";
    }

    return "unknown";
}

int tier_to_score(tier t) {
    if (t == ENTRY) return 1;
    if (t == MID) return 2;
    if (t == HIGH) return 3;
    if (t == ENTHUSIAST) return 4;
    return 0;
}

static int parse_cpu_model(const char *line, cpu *c) {
    char a[32] = {0};
    char b[32] = {0};
    char d[32] = {0};
    int n;

    n = sscanf(line, "%31s %31s %31s", a, b, d);

    if (n < 2) {
        return 0;
    }

    to_lower_str(a);
    to_lower_str(b);
    to_lower_str(d);

    if (case_equal(a, "intel")) {
        strcpy(c->brand, "intel");
        strcpy(c->family, "i");

        if (b[0] != 'i' || !isdigit((unsigned char)b[1])) {
            return 0;
        }

        c->series = b[1] - '0';

        if (n == 3) {
            char digits[16] = {0};
            int i = 0;
            int j = 0;

            while (isdigit((unsigned char)d[i])) {
                digits[j++] = d[i++];
            }
            digits[j] = '\0';

            if (digits[0] == '\0') {
                return 0;
            }

            c->model = atoi(digits);

            if (!valid_cpu_model(c)) {
                return 0;
            }

            if (d[i] == '\0') {
                strcpy(c->suffix, "none");
            } else {
                strncpy(c->suffix, d + i, sizeof(c->suffix) - 1);
                c->suffix[sizeof(c->suffix) - 1] = '\0';
            }
        } else {
            c->model = 0;
            strcpy(c->suffix, "none");
        }

        return 1;
    }

    if (case_equal(a, "amd") || case_equal(a, "ryzen")) {
        if (case_equal(a, "amd")) {
            if (n < 3 || strncmp(b, "ryzen", 5) != 0) {
                return 0;
            }

            strcpy(c->brand, "amd");
            strcpy(c->family, "ryzen");

            if (isdigit((unsigned char)b[5])) {
                c->series = b[5] - '0';

                char digits[16] = {0};
                int i = 0;
                int j = 0;

                while (isdigit((unsigned char)d[i])) {
                    digits[j++] = d[i++];
                }
                digits[j] = '\0';

                if (digits[0] == '\0') {
                    c->model = 0;
                    strcpy(c->suffix, "none");
                    return 1;
                }

                c->model = atoi(digits);

                if (!valid_cpu_model(c)) {
                    return 0;
                }

                if (d[i] == '\0') {
                    strcpy(c->suffix, "none");
                } else {
                    strncpy(c->suffix, d + i, sizeof(c->suffix) - 1);
                    c->suffix[sizeof(c->suffix) - 1] = '\0';
                }

                return 1;
            }

            if (!isdigit((unsigned char)d[0])) {
                c->series = b[0] - '0';
                return 0;
            }
        } else {
            strcpy(c->brand, "amd");
            strcpy(c->family, "ryzen");

            if (strncmp(a, "ryzen", 5) == 0 && isdigit((unsigned char)a[5])) {
                c->series = a[5] - '0';
            } else if (isdigit((unsigned char)b[0])) {
                c->series = b[0] - '0';
            } else {
                return 0;
            }

            if (n >= 3) {
                char digits[16] = {0};
                int i = 0;
                int j = 0;

                while (isdigit((unsigned char)d[i])) {
                    digits[j++] = d[i++];
                }
                digits[j] = '\0';

                if (digits[0] == '\0') {
                    c->model = 0;
                    strcpy(c->suffix, "none");
                    return 1;
                }

                c->model = atoi(digits);

                if (!valid_cpu_model(c)) {
                    return 0;
                }

                if (d[i] == '\0') {
                    strcpy(c->suffix, "none");
                } else {
                    strncpy(c->suffix, d + i, sizeof(c->suffix) - 1);
                    c->suffix[sizeof(c->suffix) - 1] = '\0';
                }
            } else {
                c->model = 0;
                strcpy(c->suffix, "none");
            }

            return 1;
        }
    }

    return 0;
}

int cpu_model_score(const cpu *c) {
    if (c->brand[0] == '\0') {
        return 0;
    }

    if (case_equal(c->brand, "intel")) {
        if (c->series == 3) return 1;

        if (c->series == 5) {
            if (c->model >= 13600) return 3;
            return 2;
        }

        if (c->series == 7) {
            if (c->model >= 12700) return 4;
            return 3;
        }

        if (c->series == 9) return 5;
    }

    if (case_equal(c->brand, "amd")) {
        if (c->series == 3) return 1;

        if (c->series == 5) {
            if (c->model >= 7600) return 3;
            return 2;
        }

        if (c->series == 7) {
            if (c->model >= 7700) return 4;
            return 3;
        }

        if (c->series == 9) return 5;
    }

    return tier_to_score(cpu_to_tier(c));
}

int gpu_model_score(const gpu *g) {
    int score = 0;

    if (g->series[0] == '\0') {
        return 0;
    }

    if (case_equal(g->series, "rtx")) {
        if (g->type >= 5090) score = 5;
        else if (g->type >= 5080) score = 4;
        else if (g->type >= 5070) score = 3;
        else if (g->type >= 5060) score = 2;
        else if (g->type >= 4050) score = 1;
    } else if (case_equal(g->series, "rx")) {
        if (g->type >= 7900) score = 5;
        else if (g->type >= 7800) score = 4;
        else if (g->type >= 7700) score = 3;
        else if (g->type >= 7600) score = 2;
        else if (g->type >= 6600) score = 1;
    }

    if (score == 0) {
        return tier_to_score(gpu_to_tier(g));
    }

    if (case_equal(g->suffix, "ti") || case_equal(g->suffix, "super") || case_equal(g->suffix, "xt")) {
        score += 1;
    } else if (case_equal(g->suffix, "xtx")) {
        score += 1;
    }

    if (score > 5) {
        score = 5;
    }

    return score;
}
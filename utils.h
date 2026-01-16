#ifndef UTILS_H
#define UTILS_H

#include "solvers.h"

#define PINSET_LENGTH 14

struct pinset_data {
    int *unique_rows;
    int n_unique_rows;
    int *pinsets;
    int n_pinsets;
    int *pinset_mappings;
    int n_pinset_mappings;
    int *moves;
    SOLUTION_T *solution_info;

} typedef DATA_T;

// extern const char *move_names[30];

int is_valid_number(const char *str);
int get_scramble(int *scramble, int size);
void cleanup(DATA_T *program_data);
void print_solutions(DATA_T *program_data, int metrics, FILE *output_destination);
void read_data(DATA_T *program_data);
long count_lines(FILE *fp);
int parse_scramble(int *scramble, char *buffer, int size);


#endif
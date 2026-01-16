#ifndef SOLVERS_H
#define SOLVERS_H

#include "utils.h"

#define PINSET_LENGTH 14
#define BUFFER_SIZE 256

#define NOT_SIMUL 0
#define NEXT_SIMUL 1
#define POSSIBLE_SKIP 2

// Forward declaration of DATA_T
typedef struct pinset_data DATA_T;

struct thread_args {
    // input args
    int thread_num;
    DATA_T *program_data;
    int *scramble;
    int move_start;
    int move_end;
    int pinset_start;
    int pinset_end;

    // return values
    int optmoves;
    int optmove_tickcount;
    int move_pinset;

    int optticks;
    int opttick_movecount;
    int tick_pinset;


    int optsimul;
    int optsimul_movecount;
    int optsimul_tickcount;
    int simul_pinset;


    int optsimticks;
    int optsimtick_simulcount;
    int optsimtick_movecount;
    int simtick_pinset;
};

struct solutioninfo {
    int optmoves;
    int optmove_tickcount;
    int move_pinset;

    int optticks;
    int opttick_movecount;
    int tick_pinset;

    int optsimul;
    int optsimul_movecount;
    int optsimul_tickcount;
    int simul_pinset;


    int optsimticks;
    int optsimtick_simulcount;
    int optsimtick_movecount;
    int simtick_pinset;

} typedef SOLUTION_T;



// Function prototypes
void calculate_all_moves(int *scramble, DATA_T *program_data);
void *calculate_all_moves_p(void *args);

void find_all_optimal(int *scramble, DATA_T *program_data);
void *find_all_optimal_p(void *args);

void *find_move_optimal_p(void *args);
void *find_tick_optimal_p(void *args);

void all_optimal(struct thread_args *thread_data, pthread_t *threads, DATA_T *program_data, int scramble[PINSET_LENGTH], int num_threads);
void move_optimal(struct thread_args *thread_data, pthread_t *threads, DATA_T *program_data, int scramble[PINSET_LENGTH], int num_threads);
void tick_optimal(struct thread_args *thread_data, pthread_t *threads, DATA_T *program_data, int scramble[PINSET_LENGTH], int num_threads);

#endif
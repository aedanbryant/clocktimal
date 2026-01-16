#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "utils.h"
#include "solvers.h"



void *calculate_all_moves_p(void *args) {

    // multithread variables
    struct thread_args *t;
    t = (struct thread_args *)args;
    DATA_T *program_data = t->program_data;
    int *scramble = t->scramble;

    // counter variables
    int i, j, c;


    // printf("Thread %d, START: %d END: %d\n", t->thread_num, t->move_start, t->move_end);

    // calculate all unique moves
    for (i = t->move_start; i < t->move_end; i += 1) {
        c = 0;
        for (j = 0; j < PINSET_LENGTH; j++) {
            c += (program_data->unique_rows)[(i * PINSET_LENGTH) + j] * scramble[j];
        }

        c %= 12;
        (program_data->moves)[i] = c < 0 ? c + 12 : c;
    }

    pthread_exit(NULL);
}

// void calculate_all_moves(int *scramble, int **moves, int *unique_rows, int n_unique_rows) {
void calculate_all_moves(int *scramble, DATA_T *program_data) {
    int i, j, c;

    // allocate new array for all moves
    program_data->moves = (int *)malloc(program_data->n_unique_rows * sizeof(int));
    if (program_data->moves == NULL) {
        perror("error allocating moves array");
        exit(1);
    }

    // calculate all unique moves
    for (i = 0; i < program_data->n_unique_rows; i += 1) {
        c = 0;
        for (j = 0; j < PINSET_LENGTH; j++) {
            c += (program_data->unique_rows)[(i * PINSET_LENGTH) + j] * scramble[j];
        }

        c %= 12;
        (program_data->moves)[i] = c < 0 ? c + 12 : c;
    }
}


void find_all_optimal(int *scramble, DATA_T *program_data) {
    // Variables for optimal solving
    int i, j;
    int move, lastmove;
    int *pinset;

    /* Optimal Movecount */
    int min_moves = __INT_MAX__; // lowest movecount
    int min_moves_tickcount = __INT_MAX__; // lowest ticks for lowest movecount
    int move_pinset;
    int movecount;
    

    /* Optimal Tickcount */
    int min_ticks = __INT_MAX__; // Lowest tick count
    int min_ticks_movecount = __INT_MAX__; // Lowest movecount for the lowest tick count 
    int tick_pinset;
    int tickcount;

    /* Simul Optimal */
    int min_simul = __INT_MAX__;
    int min_simul_movecount = __INT_MAX__;
    int min_simul_tickcount = __INT_MAX__;
    int simul_pinset;
    int simulcount;

    /* Simtick Optimal */
    int min_simticks = __INT_MAX__;
    int min_simtick_simulcount = __INT_MAX__;
    int min_simtick_movecount = __INT_MAX__;
    int simtick_pinset;
    int simtickcount;

    int tick_counts[12] = {0,1,2,3,4,5,6,5,4,3,2,1}; // accounts for stuff like the fact that 11 is actually only 1 tick

    for (i = 0; i < program_data->n_pinsets; i++) {
        movecount = 0;
        tickcount = 0;
        simulcount = 0;
        simtickcount = 0;

        pinset = &((program_data->pinsets)[i * PINSET_LENGTH]);

        for (j = 0; j < PINSET_LENGTH; j++) {
            move = (program_data->moves)[ (program_data->pinset_mappings)[(i * PINSET_LENGTH) + j] ];

            if (move != 0) movecount++; // COUNT MOVES
            tickcount += tick_counts[move]; // COUNT TICKS

            if (j > 0) {
                if (pinset[j] % 2 == 1 && (pinset[j - 1] == pinset[j] - 1) && pinset[j] < 28) { // Last move is this moves compliment
                    if (lastmove == 0) simulcount++;

                    if (tick_counts[lastmove] < tick_counts[move]) {
                        simtickcount += (tick_counts[move] - tick_counts[lastmove]);
                    }
                } else {
                    if (move != 0) simulcount++;
                    simtickcount += tick_counts[move];
                }
            } else {
                if (move != 0) simulcount++;
                simtickcount += tick_counts[move];
            }

            lastmove = move;

        }

        // Update optimal movecount, and minimize ticks
        if ((movecount < min_moves) ||
            ((movecount == min_moves) && (tickcount < min_moves_tickcount))) {
            min_moves = movecount;
            min_moves_tickcount = tickcount;
            move_pinset = i;
        }

        // Update optimal tickcount, and minimize moves
        if ((tickcount < min_ticks) || 
            ((tickcount == min_ticks) && (movecount < min_ticks_movecount))) {
            min_ticks = tickcount;
            min_ticks_movecount = movecount;
            tick_pinset = i;
        }

        // Update optimal simulcount
        if (simulcount < min_simul || 
            ((simulcount == min_simul) && (movecount < min_simul_movecount)) || 
            ((simulcount == min_simul && movecount == min_simul_movecount) && (simtickcount < min_simul_tickcount))) {
            min_simul = simulcount;
            min_simul_movecount = movecount;
            min_simul_tickcount = simtickcount;
            simul_pinset = i;
        }

        if (simtickcount < min_simticks ||
            ((simtickcount == min_simticks) && (simulcount < min_simtick_simulcount)) || 
            ((simtickcount == min_simticks && simulcount == min_simtick_simulcount) && (movecount < min_simtick_movecount))) {
            min_simticks = simtickcount;
            min_simtick_simulcount = simulcount;
            min_simtick_movecount = movecount;
            simtick_pinset = i;
        }

    }

    (program_data->solution_info)->optmoves = min_moves;
    (program_data->solution_info)->move_pinset = move_pinset;

    (program_data->solution_info)->optticks = min_ticks;
    (program_data->solution_info)->tick_pinset = tick_pinset;

    (program_data->solution_info)->optsimul = min_simul;
    (program_data->solution_info)->simul_pinset = simul_pinset;

    (program_data->solution_info)->optsimticks = min_simticks;
    (program_data->solution_info)->simtick_pinset = simtick_pinset;

}



void *find_all_optimal_p(void *args) {
    // multithread variables
    struct thread_args *t;
    t = (struct thread_args *)args;
    DATA_T *program_data = t->program_data;

    // Variables for optimal solving
    int i, j;
    int move, lastmove;
    int *pinset;

    /* Optimal Movecount */
    int min_moves = __INT_MAX__; // lowest movecount
    int min_moves_tickcount = __INT_MAX__; // lowest ticks for lowest movecount
    int move_pinset;
    int movecount;
    

    /* Optimal Tickcount */
    int min_ticks = __INT_MAX__; // Lowest tick count
    int min_ticks_movecount = __INT_MAX__; // Lowest movecount for the lowest tick count 
    int tick_pinset;
    int tickcount;

    /* Simul Optimal */
    int min_simul = __INT_MAX__;
    int min_simul_movecount = __INT_MAX__;
    int min_simul_tickcount = __INT_MAX__;
    int simul_pinset;
    int simulcount;

    /* Simtick Optimal */
    int min_simticks = __INT_MAX__;
    int min_simtick_simulcount = __INT_MAX__;
    int min_simtick_movecount = __INT_MAX__;
    int simtick_pinset;
    int simtickcount;

    int tick_counts[12] = {0,1,2,3,4,5,6,5,4,3,2,1}; // accounts for stuff like the fact that 11 is actually only 1 tick

    for (i = t->pinset_start; i < t->pinset_end; i++) {
        movecount = 0;
        tickcount = 0;
        simulcount = 0;
        simtickcount = 0;

        pinset = &((program_data->pinsets)[i * PINSET_LENGTH]);

        for (j = 0; j < PINSET_LENGTH; j++) {
            move = (program_data->moves)[ (program_data->pinset_mappings)[(i * PINSET_LENGTH) + j] ];

            if (move != 0) movecount++; // COUNT MOVES
            tickcount += tick_counts[move]; // COUNT TICKS

            if (j > 0) {
                if (pinset[j] % 2 == 1 && (pinset[j - 1] == pinset[j] - 1) && pinset[j] < 28) { // Last move is this moves compliment
                    if (lastmove == 0) simulcount++;

                    if (tick_counts[lastmove] < tick_counts[move]) {
                        simtickcount += (tick_counts[move] - tick_counts[lastmove]);
                    }
                } else {
                    if (move != 0) simulcount++;
                    simtickcount += tick_counts[move];
                }
            } else {
                if (move != 0) simulcount++;
                simtickcount += tick_counts[move];
            }

            lastmove = move;

        }

        // Update optimal movecount, and minimize ticks
        if ((movecount < min_moves) ||
            ((movecount == min_moves) && (tickcount < min_moves_tickcount))) {
            min_moves = movecount;
            min_moves_tickcount = tickcount;
            move_pinset = i;
        }

        // Update optimal tickcount, and minimize moves
        if ((tickcount < min_ticks) || 
            ((tickcount == min_ticks) && (movecount < min_ticks_movecount))) {
            min_ticks = tickcount;
            min_ticks_movecount = movecount;
            tick_pinset = i;
        }

        // Update optimal simulcount
        if (simulcount < min_simul || 
            ((simulcount == min_simul) && (movecount < min_simul_movecount)) || 
            ((simulcount == min_simul && movecount == min_simul_movecount) && (simtickcount < min_simul_tickcount))) {
            min_simul = simulcount;
            min_simul_movecount = movecount;
            min_simul_tickcount = simtickcount;
            simul_pinset = i;
        }

        if (simtickcount < min_simticks ||
            ((simtickcount == min_simticks) && (simulcount < min_simtick_simulcount)) || 
            ((simtickcount == min_simticks && simulcount == min_simtick_simulcount) && (movecount < min_simtick_movecount))) {
            min_simticks = simtickcount;
            min_simtick_simulcount = simulcount;
            min_simtick_movecount = movecount;
            simtick_pinset = i;
        }

    }

    t->optmoves = min_moves;
    t->optmove_tickcount = min_moves_tickcount;
    t->move_pinset = move_pinset;

    t->optticks = min_ticks;
    t->opttick_movecount = min_ticks_movecount;
    t->tick_pinset = tick_pinset;

    t->optsimul = min_simul;
    t->optsimul_movecount = min_simul_movecount;
    t->optsimul_tickcount = min_simul_tickcount;
    t->simul_pinset = simul_pinset;

    t->optsimticks = min_simticks;
    t->optsimtick_simulcount = min_simtick_simulcount;
    t->optsimtick_movecount = min_simtick_movecount;
    t->simtick_pinset = simtick_pinset;

    pthread_exit(NULL);
}

void *find_move_optimal_p(void *args) {
    // multithread variables
    struct thread_args *t;
    t = (struct thread_args *)args;
    DATA_T *program_data = t->program_data;

    // Variables for optimal solving
    int i, j;
    int move;
    int *pinset;

    /* Optimal Movecount */
    int min_moves = __INT_MAX__; // lowest movecount
    int min_moves_tickcount = __INT_MAX__; // lowest ticks for lowest movecount
    int move_pinset;
    int movecount;

    for (i = t->pinset_start; i < t->pinset_end; i++) {
        movecount = 0;

        pinset = &((program_data->pinsets)[i * PINSET_LENGTH]);

        for (j = 0; j < PINSET_LENGTH; j++) {
            move = (program_data->moves)[ (program_data->pinset_mappings)[(i * PINSET_LENGTH) + j] ];

            if (move != 0) movecount++; // COUNT MOVES

            if (movecount >= min_moves) {
                movecount += 2;
                break;
            }

        }

        // Update optimal movecount, and minimize ticks
        if (movecount < min_moves) {
            min_moves = movecount;
            move_pinset = i;
        }

    }

    t->optmoves = min_moves;
    t->move_pinset = move_pinset;


    pthread_exit(NULL);
}


void *find_tick_optimal_p(void *args) {
    // multithread variables
    struct thread_args *t;
    t = (struct thread_args *)args;
    DATA_T *program_data = t->program_data;

    // Variables for optimal solving
    int i, j;
    int move;
    int *pinset;


    /* Optimal Tickcount */
    int min_ticks = __INT_MAX__; // Lowest tick count
    int min_ticks_movecount = __INT_MAX__; // Lowest movecount for the lowest tick count 
    int tick_pinset;
    int tickcount;


    int tick_counts[12] = {0,1,2,3,4,5,6,5,4,3,2,1}; // accounts for stuff like the fact that 11 is actually only 1 tick

    for (i = t->pinset_start; i < t->pinset_end; i++) {
        tickcount = 0;

        pinset = &((program_data->pinsets)[i * PINSET_LENGTH]);

        for (j = 0; j < PINSET_LENGTH; j++) {
            move = (program_data->moves)[ (program_data->pinset_mappings)[(i * PINSET_LENGTH) + j] ];

            tickcount += tick_counts[move]; // COUNT TICKS

            if (tickcount >= min_ticks) {
                tickcount += 2;
                break;
            }

        }

        // Update optimal tickcount, and minimize moves
        if (tickcount < min_ticks) {
            min_ticks = tickcount;
            tick_pinset = i;
        }

    }

    t->optticks = min_ticks;
    t->tick_pinset = tick_pinset;

    pthread_exit(NULL);
}

void all_optimal(struct thread_args *thread_data, pthread_t *threads, DATA_T *program_data, int scramble[PINSET_LENGTH], int num_threads) {
    int t, result;

    // parallel calculate all moves
    for (t = 0; t < num_threads; t++) {
        thread_data[t].scramble = scramble;

        result = pthread_create(&threads[t], NULL, calculate_all_moves_p, (void *)&thread_data[t]);

        if (result) {
            perror("error creating thread\n");
            exit(1);
        }
    }

    for(t=0;t<num_threads;t++) {
        pthread_join(threads[t],NULL);
    }



    // parallel find all optimal
    for (t = 0; t < num_threads; t++) {
        thread_data[t].scramble = scramble;

        result = pthread_create(&threads[t], NULL, find_all_optimal_p, (void *)&thread_data[t]);

        if (result) {
            perror("error creating thread\n");
            exit(1);
        }
    }


    (program_data->solution_info)->optmoves = __INT_MAX__;
    (program_data->solution_info)->optticks = __INT_MAX__;
    (program_data->solution_info)->optsimul = __INT_MAX__;
    (program_data->solution_info)->optsimticks = __INT_MAX__;

    for(t=0;t<num_threads;t++) {
        pthread_join(threads[t],NULL);

        if (thread_data[t].optmoves < (program_data->solution_info)->optmoves ||
            ((thread_data[t].optmoves == (program_data->solution_info)->optmoves) && (thread_data[t].optmove_tickcount < (program_data->solution_info)->optmove_tickcount))) {
            (program_data->solution_info)->optmoves = thread_data[t].optmoves;
            (program_data->solution_info)->move_pinset = thread_data[t].move_pinset;
            (program_data->solution_info)->optmove_tickcount = thread_data[t].optmove_tickcount;
        }
        if (thread_data[t].optticks < (program_data->solution_info)->optticks ||
            ((thread_data[t].optticks == (program_data->solution_info)->optticks) && (thread_data[t].opttick_movecount < (program_data->solution_info)->opttick_movecount))) {
            (program_data->solution_info)->optticks = thread_data[t].optticks;
            (program_data->solution_info)->tick_pinset = thread_data[t].tick_pinset;
            (program_data->solution_info)->opttick_movecount = thread_data[t].opttick_movecount;
        }
        if (thread_data[t].optsimul < (program_data->solution_info)->optsimul ||
            ((thread_data[t].optsimul == (program_data->solution_info)->optsimul) && (thread_data[t].optsimul_movecount < (program_data->solution_info)->optsimul_movecount)) || 
            ((thread_data[t].optsimul == (program_data->solution_info)->optsimul && thread_data[t].optsimul_movecount == (program_data->solution_info)->optsimul_movecount) && (thread_data[t].optsimul_tickcount < (program_data->solution_info)->optsimul_tickcount))) {
            (program_data->solution_info)->optsimul = thread_data[t].optsimul;
            (program_data->solution_info)->simul_pinset = thread_data[t].simul_pinset;
        }
        if (thread_data[t].optsimticks < (program_data->solution_info)->optsimticks ||
            ((thread_data[t].optsimticks == (program_data->solution_info)->optsimticks) && (thread_data[t].optsimtick_simulcount < (program_data->solution_info)->optsimtick_simulcount)) || 
            ((thread_data[t].optsimticks == (program_data->solution_info)->optsimticks && thread_data[t].optsimtick_simulcount == (program_data->solution_info)->optsimtick_simulcount) && (thread_data[t].optsimtick_movecount < (program_data->solution_info)->optsimtick_movecount))) {
            (program_data->solution_info)->optsimticks = thread_data[t].optsimticks;
            (program_data->solution_info)->simtick_pinset = thread_data[t].simtick_pinset;
        }

    }
}

void move_optimal(struct thread_args *thread_data, pthread_t *threads, DATA_T *program_data, int scramble[PINSET_LENGTH], int num_threads) {
    int t, result;
    
    // parallel calculate all moves
    for (t = 0; t < num_threads; t++) {
        thread_data[t].scramble = scramble;

        result = pthread_create(&threads[t], NULL, calculate_all_moves_p, (void *)&thread_data[t]);

        if (result) {
            perror("error creating thread\n");
            exit(1);
        }
    }

    for(t=0;t<num_threads;t++) {
        pthread_join(threads[t],NULL);
    }



    // parallel find all optimal
    for (t = 0; t < num_threads; t++) {
        thread_data[t].scramble = scramble;

        result = pthread_create(&threads[t], NULL, find_move_optimal_p, (void *)&thread_data[t]);

        if (result) {
            perror("error creating thread\n");
            exit(1);
        }
    }

    (program_data->solution_info)->optmoves = __INT_MAX__;

    for(t=0;t<num_threads;t++) {
        pthread_join(threads[t],NULL);

        if (thread_data[t].optmoves < (program_data->solution_info)->optmoves) {
            (program_data->solution_info)->optmoves = thread_data[t].optmoves;
            (program_data->solution_info)->move_pinset = thread_data[t].move_pinset;
        }
    }
}

void tick_optimal(struct thread_args *thread_data, pthread_t *threads, DATA_T *program_data, int scramble[PINSET_LENGTH], int num_threads) {
    int t, result;
    
    // parallel calculate all moves
    for (t = 0; t < num_threads; t++) {
        thread_data[t].scramble = scramble;

        result = pthread_create(&threads[t], NULL, calculate_all_moves_p, (void *)&thread_data[t]);

        if (result) {
            perror("error creating thread\n");
            exit(1);
        }
    }

    for(t=0;t<num_threads;t++) {
        pthread_join(threads[t],NULL);
    }



    // parallel find all optimal
    for (t = 0; t < num_threads; t++) {
        thread_data[t].scramble = scramble;

        result = pthread_create(&threads[t], NULL, find_tick_optimal_p, (void *)&thread_data[t]);

        if (result) {
            perror("error creating thread\n");
            exit(1);
        }
    }

    (program_data->solution_info)->optticks = __INT_MAX__;

    for(t=0;t<num_threads;t++) {
        pthread_join(threads[t],NULL);

        if (thread_data[t].optticks < (program_data->solution_info)->optticks) {
            (program_data->solution_info)->optticks = thread_data[t].optticks;
            (program_data->solution_info)->tick_pinset = thread_data[t].tick_pinset;
        }
    }
}
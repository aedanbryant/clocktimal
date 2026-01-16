#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"
#include "solvers.h"
#include "clocktimal.h"


int main(int argc, char *argv[]) {

    // Multithreading
    int num_threads = 1;
	pthread_t *threads;
	struct thread_args *thread_data;
    int t, result;
    int scramble[PINSET_LENGTH];
    int i, j;
    int scrambles = 100;
    int metrics = MOVECOUNT;
    int verbose = 0;
    int move_display = 14;

    DATA_T *program_data;

    /* Command Line Arguments */
    int opt;
    while ((opt = getopt(argc, argv, "t:n:mkscv:")) != -1) {
        switch (opt) {
        case 't':
            num_threads = atoi(optarg);
            break;
        case 'n':
            scrambles = atoi(optarg);
            break;
        case 'm':
            if (metrics != MOVECOUNT) {
                printf("Only one metric allowed\n");
                return -1;
            }
            metrics = MOVECOUNT;
            move_display = 13;
            break;
        case 'k':
            if (metrics != MOVECOUNT) {
                printf("Only one metric allowed\n");
                return -1;
            }
            metrics = TICKCOUNT;
            move_display = 40;
            break;
        case 's':
            if (metrics != MOVECOUNT) {
                printf("Only one metric allowed\n");
                return -1;
            }
            metrics = SIMLULCOUNT;
            move_display = 8;
            break;
        case 'c':
            if (metrics != MOVECOUNT) {
                printf("Only one metric allowed\n");
                return -1;
            }
            metrics = SIMTICKCOUNT;
            move_display = 30;
            break;
        case 'v':
            verbose = atoi(optarg);
            break;
        case ':':       /* -f or -o without operand */
            fprintf(stderr, "Option -%c requires an operand\n", optopt);
            break;
        case '?':
            fprintf(stderr, "Unrecognized option: '-%c'\n", optopt);
        }
    }
    
    program_data = (DATA_T *)malloc(sizeof(DATA_T));
    if (program_data == NULL) {
        perror("Program data allocation failed");
        exit(1);
    }

    // load data from files
    read_data(program_data);

    // allocate new array for all moves
    program_data->moves = (int *)malloc(program_data->n_unique_rows * sizeof(int));
    if (program_data->moves == NULL) {
        perror("error allocating moves array");
        exit(1);
    }

    // allocate threads
	threads=calloc(num_threads, sizeof(pthread_t));
	if (threads==NULL) perror("allocating threads");

	// allocate thread data
	thread_data=calloc(num_threads,sizeof(struct thread_args));
	if (thread_data==NULL) perror("allocating thread_data");

    // Chunk size for move and pinset arrays
    int move_size = program_data->n_unique_rows / num_threads;
    int pinset_size = program_data->n_pinsets / num_threads;

    // General Thread data initialization
    for (t = 0; t < num_threads; t++) {
        thread_data[t].thread_num = t;
        thread_data[t].program_data = program_data;

        thread_data[t].move_start = t * move_size;
        thread_data[t].pinset_start = t * pinset_size;
        
        if (t < num_threads - 1) {
            thread_data[t].move_end = (t + 1) * move_size;
            thread_data[t].pinset_end = (t + 1) * pinset_size;
        } else {
            thread_data[t].move_end = program_data->n_unique_rows;
            thread_data[t].pinset_end = program_data->n_pinsets;
        }
    }


    int movecount_stats[move_display];

    for (i = 0; i < move_display; i++) {
        movecount_stats[i] = 0;
    }


    srand(time(NULL));



    for (i = 0; i < scrambles; i++) {

        // int scramble[PINSET_LENGTH] = {4, 7, 6, 6, 5, 4, 9, 9, 4, 10, 10, 7, 11, 4};
        for (j = 0; j < PINSET_LENGTH; j++) {
            scramble[j] = rand() % 12;  // Generates a number from 0 to 11
        }

        if (metrics == MOVECOUNT) {
            move_optimal(thread_data, threads, program_data, scramble, num_threads);
            movecount_stats[(program_data->solution_info)->optmoves]++;
        } else if (metrics == TICKCOUNT) {
            tick_optimal(thread_data, threads, program_data, scramble, num_threads);
            movecount_stats[(program_data->solution_info)->optticks]++;
        } else if (metrics == SIMLULCOUNT) {
            all_optimal(thread_data, threads, program_data, scramble, num_threads);
            movecount_stats[(program_data->solution_info)->optsimul]++;
        } else if (metrics == SIMTICKCOUNT) {
            all_optimal(thread_data, threads, program_data, scramble, num_threads);
            movecount_stats[(program_data->solution_info)->optsimticks]++;
        }

        if ((verbose > 0) && ((i + 1) % verbose == 0)) {
            printf("\rCount: %d", i + 1);
            fflush(stdout);
        }

        // print_solutions(program_data);

    }
    printf("\n");

    int sum = 0;
    for (i = 0; i < move_display; i++) {
        printf("%d Movers: %d\n", i, movecount_stats[i]);
        sum += i * movecount_stats[i];
    }

    printf("Mean = %f\n", (float)sum / scrambles);



    // free memory
    cleanup(program_data);
}



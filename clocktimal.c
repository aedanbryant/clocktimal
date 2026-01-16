#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#include "utils.h"
#include "solvers.h"
#include "clocktimal.h"



int main(int argc, char *argv[]) {

    // Multithreading
    int num_threads = 1;
	pthread_t *threads;
	struct thread_args *thread_data;
    int t;

    int batch_solving = 0;
    char* input_file = NULL;
    char* output_file = NULL;

    int metrics = 0;
    int verbose = 0;

    DATA_T *program_data;
    
    int scramble[PINSET_LENGTH];

    /* Command Line Arguments */
    int opt;
    while ((opt = getopt(argc, argv, "t:i:o:mkscv:")) != -1) {
        switch (opt) {
        case 't':
            num_threads = atoi(optarg);
            break;
        case 'i':
            input_file = optarg;
            batch_solving = 1;
            break;
        case 'o':
            output_file = optarg;
            break;
        case 'm':
            metrics |= MOVECOUNT;
            break;
        case 'k':
            metrics |= TICKCOUNT;
            break;
        case 's':
            metrics |= SIMLULCOUNT;
            break;
        case 'c':
            metrics |= SIMTICKCOUNT;
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

    // chunk size for move and pinset arrays
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


    /* Batch Solving */
    if (batch_solving == 1) {
        FILE *file;
        long num_scrambles;
        char buffer[BUFFER_SIZE];
        
        

        file = fopen(input_file, "r");
        if (!file) {
            perror("Error opening input file");
            exit(1);
        }

        // Get number of scrambles in file
        num_scrambles = count_lines(file);

        // Read all scrambles into array
        int all_scrambles[num_scrambles][PINSET_LENGTH];
        for (int i = 0; i < num_scrambles; i++) {

            fgets(buffer, sizeof buffer, file);

            parse_scramble(all_scrambles[i], buffer, PINSET_LENGTH);
        }
        printf("Read %ld scrambles\n", num_scrambles);

        fclose(file);
        
        // Open output file
        if (output_file == NULL) {
            printf("No output file given, defaulting to out.txt\n");
            output_file = "out.txt";
        }
        file = fopen(output_file, "w");

        // Compute optimals and write to file
        for (int i = 0; i < num_scrambles; i++) {
            all_optimal(thread_data, threads, program_data, all_scrambles[i], num_threads);
            print_solutions(program_data, metrics, file);

            if ((verbose > 0) && (i % verbose == 0)) {
                printf("%d scrambles completed\n", i);
            }
        }

        fclose(file);

        return 0;

    }

    /* All Solutions */
    while(1) {

        if (get_scramble(scramble, PINSET_LENGTH)) {
            continue;
        }

        if (metrics == MOVECOUNT) {
            move_optimal(thread_data, threads, program_data, scramble, num_threads);
        } else if (metrics == TICKCOUNT) {
            tick_optimal(thread_data, threads, program_data, scramble, num_threads);
        } else {
            all_optimal(thread_data, threads, program_data, scramble, num_threads);
        }

        print_solutions(program_data, metrics, stdout);


    }
    // free memory
    cleanup(program_data);
}



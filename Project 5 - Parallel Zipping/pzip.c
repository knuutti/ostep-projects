#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>

typedef struct thread_args {
    int index;
    int n_thread;
} thread_args;

// The function to be executed by all threads
void * parallelZip(void * args) { 

    thread_args * actual_arguments = args;

    size_t bufsize = 0;
    FILE * input = NULL;
    FILE * output = NULL;
    
    if((input = fopen("ksii.txt", "r")) == NULL) {
        fprintf(stderr, "error: cannot open file\n");
        exit(1);
    }

    char output_name[20];
    sprintf(output_name, "./temp/temp%d.z", actual_arguments->index);
    if((output = fopen(output_name, "w")) == NULL) {
        fprintf(stderr, "error: cannot open file\n");
        exit(1);
    }

    fseek(input, 0, SEEK_END);
    int file_size = ftell(input);
    int seg_size = file_size / actual_arguments->n_thread;

    //printf("i=%d, threads=%d\n\n", actual_arguments->index, actual_arguments->n_thread);

    if (actual_arguments->index == actual_arguments->n_thread - 1) {
        seg_size = file_size - ((actual_arguments->n_thread - 1) * seg_size);
        fseek(input, -seg_size, SEEK_END);
    } else {
        fseek(input, seg_size * actual_arguments->index, SEEK_SET);
    }

    int current = -1;
    int temp;
    int counter;
    while(1) {
        temp = fgetc(input);
        // First iteration
        if (current == -1) {
            counter = 1;
            current = temp;
        }
        else if (current == temp) {
            counter++;
        }
        else {
            fwrite(&counter, sizeof(int), 1, output);
            fwrite(&current, sizeof(char), 1, output);
            counter = 1;
            current = temp;
        }
        if  (ftell(input) == seg_size * (actual_arguments->index + 1)) {
            fwrite(&counter, sizeof(int), 1, output);
            fwrite(&current, sizeof(char), 1, output);
            break;
        }
        if (feof(input)) {
            break;
        }
    }

    fclose(input);
    fclose(output);

}

int main(int argc, char * argv[])
{
    int threads = get_nprocs_conf();

    struct stat st = {0};

    if (stat("temp", &st) == -1) {
        mkdir("temp", 0700);
    }

    // Zipping part in threads
    pthread_t tid;
    thread_args * args = NULL;
    for (int i = 0; i < threads; i++) {
        args = malloc(sizeof *args);
        args->index = i;
        args->n_thread = threads;
        pthread_create(&tid, NULL, parallelZip, args);
    }
    pthread_exit(NULL);

    // Combining the temporary files
    FILE * zip_file = NULL;
    if((zip_file = fopen("output.z", "w")) == NULL) {
        fprintf(stderr, "error: cannot open file\n");
        exit(1);
    }
    for (int i = 0; i < threads; i++) {

    }

    return 0;
}
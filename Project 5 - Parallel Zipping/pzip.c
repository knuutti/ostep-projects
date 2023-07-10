#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
    if((input = fopen("ksii.txt", "r")) == NULL) {
        fprintf(stderr, "error: cannot open file\n");
        exit(1);
    }

    fseek(input, 0, SEEK_END);
    int file_size = ftell(input);
    int seg_size = file_size / actual_arguments->n_thread;

    printf("i=%d, threads=%d\n\n", actual_arguments->index, actual_arguments->n_thread);

    if (actual_arguments->index == actual_arguments->n_thread - 1) {
        seg_size = file_size - ((actual_arguments->n_thread - 1) * seg_size);
        fseek(input, -seg_size, SEEK_END);
        printf("#################");
    } else {
        fseek(input, seg_size * actual_arguments->index, SEEK_SET);
    }

    printf("\n\n%d, %d, %d\n\n", file_size, seg_size, actual_arguments->index);
    

    printf("Fseekin arvo: %ld\n\n", ftell(input));

    int current = -1;
    int temp;
    int counter;
    while(1) {
        temp = fgetc(input);
        printf("%c", temp);
        //fwrite(&current, sizeof(char), 1, stdout);
        if (feof(input) || ftell(input) == seg_size * (actual_arguments->index + 1)) {
            break;
        }
    }

    fclose(input);

    //printf("Number: %d\n", actual_arguments->index);

}

// int zip(FILE * input, int procs, int segs) {

//     int i;
//     pthread_t tid;

//     fseek(input, 0, SEEK_END);
//     int file_size = ftell(input);
//     fseek(input, 0, SEEK_SET);

//     printf("Size: %d, Threads: %d\n", file_size, procs);

//     thread_args *args = malloc(sizeof(thread_args));
  
//     for (i = 0; i < procs; i++)

//         args->index = i;
//         args->file = input;
//         printf("123123\n",i);

//         pthread_create(&tid, NULL, parallelZip, args);
  
//     pthread_exit(NULL);

//     return(0);
// }
  
int main(int argc, char * argv[])
{

    pthread_t tid;
    int threads = 4;// get_nprocs_conf();

    thread_args * args = malloc(sizeof *args);
  
    int i = 0;
    while (i < threads) {
            
        args->index = i;
        args->n_thread = threads;
        sleep(1);

        pthread_create(&tid, NULL, parallelZip, args);

    }
  
    pthread_exit(NULL);

    return 0;
}
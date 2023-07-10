#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct thread_args {
    int index;
} thread_args;

// The function to be executed by all threads
void * parallelZip(void * args) { 
    
    thread_args * actual_arguments = args;
    printf("Number: %d\n", actual_arguments->index);

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
    int threads = get_nprocs_conf() - 1;

    thread_args * args = malloc(sizeof *args);
  
    for (int i = 0; i < threads; i++) {

        printf("Loop %d\n", i+1);
        args->index = i;

        pthread_create(&tid, NULL, parallelZip, args);

    }
  
    pthread_exit(NULL);

    return 0;
}
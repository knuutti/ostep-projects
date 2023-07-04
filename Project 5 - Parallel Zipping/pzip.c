#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// The function to be executed by all threads
void *myThreadFun(void *vargp)
{
    // Store the value argument passed to this thread
    int *myid = (int *)vargp;
  
    // Variable for tracking the thread number
    static int i = 0;
    i++;

    //fseek(input, 0, SEEK_CUR);

    // Print the argument, static and global variables
    printf("Thread ID: %d, Thread number: %d\n", *myid, i);
}

int zip(FILE * input, int procs, int segs) {

    int i;
    pthread_t tid;

    fseek(input, 0, SEEK_END);
    int file_size = ftell(input);
    fseek(input, 0, SEEK_SET);

    printf("%d\n", file_size);
  
    for (i = 0; i < procs; i++)
        pthread_create(&tid, NULL, myThreadFun, (void *)&tid);
  
    pthread_exit(NULL);

    return(0);
}
  
int main(int argc, char * argv[])
{

    FILE * input = fopen(argv[1], "r");
    int procs = get_nprocs_conf();
    int segs = procs - 1;

    zip(input, procs, segs);

    fclose(input);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
  
// Let us create a global variable to change it in threads
int g = 0;
  
// The function to be executed by all threads
void *myThreadFun(void *vargp)
{
    // Store the value argument passed to this thread
    int *myid = (int *)vargp;
  
    // Let us create a static variable to observe its changes
    static int s = 0;
    // Change static and global variables
    ++s; ++g;
    sleep(1);
    // Print the argument, static and global variables
    printf("Thread ID: %d, Static: %d, Global: %d\n", *myid, ++s, ++g);
}
  
int main()
{
    int i;
    pthread_t tid;

    printf("%d\n", get_nprocs());
  
    // Let us create three threads
    for (i = 0; i < 13; i++)
        pthread_create(&tid, NULL, myThreadFun, (void *)&tid);
  
    pthread_exit(NULL);
    return 0;
}
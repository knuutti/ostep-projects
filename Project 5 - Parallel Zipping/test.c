#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>

typedef struct {
    char* filename;
    char* tmpfile;
    int start;
    int end;
} ThreadArgs;

// Define the structure for the queue
typedef struct {
    ThreadArgs* elements;
    int front;
    int rear;
    int size;
    int capacity;
} Queue;

void* zip_segment(ThreadArgs* args);
int zip_file(char* filename, int num_threads, int num_segments);
void* worker();

Queue* create_queue(int capacity);
void add_to_queue(Queue* queue, ThreadArgs element);
ThreadArgs pop_from_queue(Queue* queue);
void free_queue(Queue* queue);

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
Queue* job_queue = NULL;
int done_segments[100] = {0};

Queue* create_queue(int capacity) {
    // Function to create a new queue
    Queue* queue = (Queue*) malloc(sizeof(Queue));
    queue->elements = (ThreadArgs*) malloc(capacity * sizeof(ThreadArgs));
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
    queue->capacity = capacity;
    return queue;
}

void add_to_queue(Queue* queue, ThreadArgs element) {
    // Function to add an element to a queue
    if (queue->size == queue->capacity) {
        printf("Queue is full. Unable to add element.\n");
        return;
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->elements[queue->rear] = element;
    queue->size++;
}

ThreadArgs pop_from_queue(Queue* queue) {
    // Function to remove and return the front element from the queue
    if (queue->size == 0) {
        //printf("Queue is empty. Unable to remove element.\n");
        ThreadArgs null_element = {NULL, NULL, 0, 0};
        return null_element;
    }
    ThreadArgs popped_element = queue->elements[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return popped_element;
}

void free_queue(Queue* queue) {
    free(queue->elements);
    free(queue);
}


void* zip_segment(ThreadArgs* args) {
    /* A function, that takes in a ThreadArgs struct and writes the RLE of the text in it to the ThreadArgs tmpfile. */
    //ThreadArgs* args = (ThreadArgs*) arg;
    char* filename = args->filename;
    char* tmpfile = args->tmpfile;
    int start = args->start;
    int end = args->end;

    //printf("Zipping from: %d, %d\n", start, end);
    
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file %s", filename);
        return NULL;
    }

    FILE* fpout = fopen(tmpfile, "w");
    if (fpout == NULL) {
        printf("Error opening file %s", tmpfile);
        return NULL;
    }
    if (start > 0){
        fseek(fp, start, SEEK_SET);
    }

    char curr_char;
    int count = 0;
    char rle_char;

    while (ftell(fp) < end && (curr_char = fgetc(fp)) != EOF) {
        if (count == 0) {
            rle_char = curr_char;
            count++;
        } else if (rle_char == curr_char) {
            count++;
        } else {
            fwrite(&count, 4, 1, fpout);
            fwrite(&rle_char, 1, 1, fpout);
            rle_char = curr_char;
            count = 1;
        }
    }
    /*
    if (ftell(fp) >= end) {
        printf("Reached end of segment.\n Pos: %ld, end: %d\n", ftell(fp), end);
    }
    else if (curr_char == EOF) {
        printf("Reached end of file.\n");
    }
    else{
        printf("Something went wrong.\n");
    }
    */
    if (count > 0) {
        fwrite(&count, 4, 1, fpout);
        fwrite(&rle_char, 1, 1, fpout);
    }

    fclose(fp);
    fclose(fpout);
    return NULL;
}

void* worker() {
    /* This function is the target of a thread. It picks a ThreadArgs from a queue until there are no more jobs. */
    while (1) {
        // Lock the mutex
        pthread_mutex_lock(&queue_mutex);
        // If there are no jobs, unlock the mutex and exit
        if (job_queue->size == 0) {
            pthread_mutex_unlock(&queue_mutex);
            return NULL;
        }
        // Pop the first job from the queue
        ThreadArgs job = pop_from_queue(job_queue);
        
        //printf("Doing job %s on thread %d\n", job.tmpfile, (int) pthread_self());
        // Unlock the mutex
        pthread_mutex_unlock(&queue_mutex);
        // Run the job
        zip_segment(&job);
        char last_char_index;
        // Find the last character in the filename, the remaining characters are the index
        for (int i = 0; i < (int) strlen(job.tmpfile); i++) {
            if (isdigit(job.tmpfile[i])) {
                last_char_index = i-1;
                break;
            }
        }
        // Get the index of the file, the nth segment
        char index[10];
        for (int i = 0; i < (int) strlen(job.tmpfile) - last_char_index; i++) {
            index[i] = job.tmpfile[last_char_index + i + 1];
        }
        int nsegment = atoi(index);
        //printf("Done segment %d\n", nsegment);
        // Mark the segment as done
        done_segments[nsegment] = 1;
    }
}

int zip_file(char* filename, int num_threads, int num_segments) {
    /* Takes in a file, number fo threads to use for RLE and total number of segments.
    num_threads are created and each thread is assigned a segment until all segments are assigned.
    The results are written to temporary files, which are then merged into the final output.
    */
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file %s", filename);
        return 1;
    }

    // Get the size of the file
    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    fclose(fp);

    // Divide the file into segments
    int segment_size = file_size / num_segments;
    // Create the queue
    job_queue = create_queue(num_segments);

    // Create the arguments for each thread
    char tmpfiles[num_segments][100];
    mkdir("./tmp-pzip", 0777);
    for (int i = 0; i < num_segments; i++) {
        ThreadArgs thread_args;
        thread_args.filename = filename;
        thread_args.start = i * segment_size;
        // The end is either at (i + 1) * segment_size or the end of the file if it's the last segment
        thread_args.end = (i == num_segments - 1) ? file_size : (i + 1) * segment_size;
        sprintf(tmpfiles[i], "./tmp-pzip/tmpfile%d", i);
        thread_args.tmpfile = tmpfiles[i];
        add_to_queue(job_queue, thread_args);
        //printf("Added job %s to queue\n", tmpfiles[i]);
        //printf("Start, end: %d, %d\n", thread_args.start, thread_args.end);
    }

    // Allocate the threads
    pthread_t* threads = (pthread_t*) malloc(num_threads * sizeof(pthread_t));

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
        pthread_detach(threads[i]);
    }

    // Merge the temporary files in order
    int file_index = 0;
    while(file_index < num_segments){
        if (done_segments[file_index] == 0) {
            //printf("Waiting for file %s\n", tmpfiles[file_index]);
            continue;
        }
        //printf("Merging file %s\n", tmpfiles[file_index]);
        //printf("Merging file %s\n", tmpfiles[file_index]);
        FILE* fpin = fopen(tmpfiles[file_index], "r");
        if (fpin == NULL) {
            printf("Error opening file %s", tmpfiles[file_index]);
            return 1;
        }

        int count;
        char rle_char;
        while (fread(&count, 4, 1, fpin) != 0) {
            //printf("Count: %d\n", count);
            fread(&rle_char, 1, 1, fpin);
            //printf("Char: %c\n", rle_char);
            fwrite(&count, 4, 1, stdout);
            fwrite(&rle_char, 1, 1, stdout);
        }
        
        
       /*
       char c;
       c = fgetc(fpin);
       while(c != EOF){
            fputc(c, stdout);
            c = fgetc(fpin);
       }
    */
        fclose(fpin);
        remove(tmpfiles[file_index]);
        file_index++;  
    }
    /* Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }*/


    free(threads);
    //fclose(fp);
    remove("./tmp-pzip");
    return 0;
}

int main(int argc, char *argv[])
{
    /* Main function accepts two optional variables and any number of files to zip with RLE*/
    // Do Run-length encoding for the input file and print the result to stdout
    if (argc == 1) {
        printf("pzip: -t N -s M file1 [file2 ...]\n");
        return 1;
    }
    int num_threads = -1;
    int num_segments = -1;
    int reduce_from_nfiles = 0;
    // If the first argument is -t, get the number of threads
    if (argc >= 3 && strcmp(argv[1], "-t") == 0) {
        num_threads = atoi(argv[2]);
        reduce_from_nfiles = reduce_from_nfiles + 2;
    }
    // If the second argument is -s, get the number of segments
    if (argc >= 5 && strcmp(argv[3], "-s") == 0) {
        num_segments = atoi(argv[4]);
        reduce_from_nfiles = reduce_from_nfiles + 2;
    }

    int nfiles = argc - 1 - reduce_from_nfiles;
    //printf("nfiles: %d", nfiles);
    // If the number of threads or segments is not specified, use the default values
    if (num_threads == -1) {
        num_threads = get_nprocs() -1;
    }
    if (num_segments == -1) {
        num_segments = num_threads;
    }
    printf("Using %d threads and %d segments\n", num_threads, num_segments);
    char files[20][200];
    // If no files are specified, use stdin
    if (nfiles == 0) {
        strcpy(files[0], "stdin");
        nfiles = 1;
    }

    // If files are specified, use them
    else{
        for (int i = 0; i < nfiles; i++) {
            strcpy(files[i], argv[i + 1 + reduce_from_nfiles]);
            //files[i] = argv[i + 2];
        }
    }
    pthread_mutex_init(&queue_mutex, NULL);
    int exit_status;
    for (int i = 0; i < nfiles; i++){
        exit_status = zip_file(files[i],num_threads,num_segments);
        if (exit_status == 1){
            return exit_status;
        }
        //printf("Done with file %s\n", files[i]);
        for (int i = 0; i < num_segments; i++){
            done_segments[i] = 0;
        }
    }
    free_queue(job_queue);
    pthread_mutex_destroy(&queue_mutex);
}
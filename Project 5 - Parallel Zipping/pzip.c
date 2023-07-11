/* 
CT30A3370 Käyttöjärjestelmät ja systeemiohjelmointi
Project 5: Parallel zip

Authors: Eetu Knutars & Joona Lappalainen
Last modified: 11.7.2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <sys/sysinfo.h>

typedef struct thread_args {
    int index;
    int n_thread;
    char * file_name;
} thread_args;

// The function to be executed by all threads
void * parallelZip(void * args) { 

    thread_args * actual_arguments = args;

    size_t bufsize = 0;
    FILE * input = NULL;
    FILE * output = NULL;

    if((input = fopen(actual_arguments->file_name, "r")) == NULL) {
        fprintf(stderr, "error: cannot open file '%s'\n", actual_arguments->file_name);
        exit(1);
    }

    char output_name[20];
    sprintf(output_name, "./temp/temp%d.z", actual_arguments->index);
    if((output = fopen(output_name, "w")) == NULL) {
        fprintf(stderr, "error: cannot open file in parallelZip '%s'\n", output_name);
        exit(1);
    }

    fseek(input, 0, SEEK_END);
    int file_size = ftell(input);
    int seg_size = file_size / actual_arguments->n_thread;

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

int zip(char * input_name, char * output_name)
{
    int threads = get_nprocs_conf();
    struct stat st = {0};

    // Making a temporary file for storing partial zip-files
    if (stat("temp", &st) == -1) {
        mkdir("temp", 0700);
    }

    // Zipping part in threads
    pthread_t tid[threads];
    thread_args * args = NULL;
    for (int i = 0; i < threads; i++) {
        args = malloc(sizeof *args);
        args->index = i;
        args->n_thread = threads;
        args->file_name = input_name;
        pthread_create(&tid[i], NULL, parallelZip, args);
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(tid[i], NULL);  
    }

    // Combining the temporary files
    char c;
    FILE * zip_file = NULL;
    if((zip_file = fopen(output_name, "a")) == NULL) {
        fprintf(stderr, "error: cannot open file '%s'\n", output_name);
        exit(1);
    }

    FILE * temp_zip_file = NULL;
    for (int i = 0; i < threads; i++) {
        char file_name[20];
        sprintf(file_name, "./temp/temp%d.z", i);
        if((temp_zip_file = fopen(file_name, "r")) == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", file_name);
            exit(1);
        }
        while ((c = fgetc(temp_zip_file)) != EOF) {
            fputc(c, zip_file);
        }
        fclose(temp_zip_file);
        remove(file_name);
    }

    fclose(zip_file);
    remove("temp");

    return 0;
}

int main (int argc, char * argv[]) {
    
    /* Exit successfully if no input files are given */
    if(argc < 3)
    {
        fprintf(stderr, "my-zip: file1 [file2 ...]\n");
        exit(1); // exit(0) ?
    }

    remove(argv[argc-1]);

    for (int i = 1; i < argc - 1; i++) {
        zip(argv[i], argv[argc-1]);
    }

    return(0);
}
/* 
CT30A3370 Käyttöjärjestelmät ja systeemiohjelmointi
Project 5: Parallel Zip

File: pzip.c
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

// Struct for passing arguments from main level to threads
typedef struct thread_args {
    int index;
    int n_thread;
    char * file_name;
} thread_args;

// The zip function to be executed by all threads
// Each thread only zips a fixed partion of the input and writes it to a temporary file
void * parallelZip(void * args) { 

    thread_args * actual_arguments = args; // Getting the parametres defined in zip()

    FILE * input = NULL;
    FILE * output = NULL;

    // Opening the input file with error handler
    if((input = fopen(actual_arguments->file_name, "r")) == NULL) {
        fprintf(stderr, "error: cannot open file '%s'\n", actual_arguments->file_name);
        exit(1);
    }

    // Creating a temporary file 'temp<index>' for writing the zipped data
    char output_name[20];
    sprintf(output_name, "./temp/temp%d.z", actual_arguments->index);
    if((output = fopen(output_name, "w")) == NULL) {
        fprintf(stderr, "error: cannot open file in parallelZip '%s'\n", output_name);
        exit(1);
    }

    // Finding the file size for defining the segment size by using fseek() and ftell()
    fseek(input, 0, SEEK_END);
    int file_size = ftell(input);

    // Defining the segment size: we decided to use ceil() for rounding upwards, since
    // that makes sure that the last thread won't slow down the process by having significantly
    // more data to process
    int seg_size = ceil(file_size / actual_arguments->n_thread);

    // Setting the point in input stream that the thread starts to read by using fseek()
    if (actual_arguments->index == actual_arguments->n_thread - 1) {
        // For the last thread, adjust the segment size so that it reads input until EOF
        seg_size = file_size - ((actual_arguments->n_thread - 1) * seg_size);
        fseek(input, -seg_size, SEEK_END);
    } else {
        fseek(input, seg_size * actual_arguments->index, SEEK_SET);
    }

    // Actual zipping part of the function
    int current = -1;
    int temp;
    int counter;
    while(1) {
        // We are tracking a few values when reading the input:
        // 1) current char, 2) number of current chars and 3) newest char read

        temp = fgetc(input);

        // First iteration
        if (current == -1) {
            counter = 1;
            current = temp;
        }
        // If the newest char matches the current char, increase the counter
        else if (current == temp) {
            counter++;
        }
        // Char has changed, write the current status and continue with the newest char
        else {
            fwrite(&counter, sizeof(int), 1, output);
            fwrite(&current, sizeof(char), 1, output);
            counter = 1;
            current = temp;
        }

        // End of the segment reached, write the current status and end
        if  (ftell(input) == seg_size * (actual_arguments->index + 1)) {
            fwrite(&counter, sizeof(int), 1, output);
            fwrite(&current, sizeof(char), 1, output);
            break;
        }

        // End of file, end of reading
        if (feof(input)) {
            break;
        }
    }

    // Close the files
    fclose(input);
    fclose(output);

    return(0);
}

int zip(char * input_name, char * output_name)
{

    // Getting the thread count of the system for splitting segments correctly
    int threads = get_nprocs_conf();
    
    // Making a temporary file for storing partial zip-files
    struct stat st = {0};
    if (stat("temp", &st) == -1) {
        mkdir("temp", 0700);
    }

    // Zipping part in threads
    pthread_t tid[threads];
    thread_args * args = NULL;
    for (int i = 0; i < threads; i++) {
        // Each thread gets index, thread/segment count 
        // and the input file name as a parametre
        args = malloc(sizeof *args);
        args->index = i;
        args->n_thread = threads;
        args->file_name = input_name;
        pthread_create(&tid[i], NULL, parallelZip, args);
    }

    // Waiting for all the threads to finish before continuing
    for (int i = 0; i < threads; i++) {
        pthread_join(tid[i], NULL);  
    }

    // Creating a file for storing the output (append mode)
    char c;
    FILE * zip_file = NULL;
    if((zip_file = fopen(output_name, "a")) == NULL) {
        fprintf(stderr, "error: cannot open file '%s'\n", output_name);
        exit(1);
    }

    // Combining the temporary files in folder 'temp' and writing them to output
    FILE * temp_zip_file = NULL;
    for (int i = 0; i < threads; i++) {
        char file_name[20];
        sprintf(file_name, "./temp/temp%d.z", i);
        if((temp_zip_file = fopen(file_name, "r")) == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", file_name);
            exit(1);
        }
        
        // Reading and writing temp file to the output file until the end
        while ((c = fgetc(temp_zip_file)) != EOF) {
            fputc(c, zip_file);
        }

        // Closing the temp file and removing it after processing
        fclose(temp_zip_file);
        remove(file_name); 
    
    }

    // Closing file and removing 'temp' folder
    fclose(zip_file);
    remove("temp");

    return(0);
}

int main (int argc, char * argv[]) {
    
    /* Exit successfully if not enough arguments were are given */
    if(argc < 3)
    {
        fprintf(stderr, "pzip: file1 [file2 ...]\n");
        exit(0);
    }

    // Overwriting previous output file if it exists
    remove(argv[argc-1]);

    // Zipping each given input file
    for (int i = 1; i < argc - 1; i++) {
        zip(argv[i], argv[argc-1]);
    }

    return(0);
}
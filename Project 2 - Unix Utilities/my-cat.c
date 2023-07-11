/* 
CT30A3370 Käyttöjärjestelmät ja systeemiohjelmointi
Project 2: Unix Utilities

File: my-cat.c
Authors: Eetu Knutars & Joona Lappalainen
Last modified: 11.7.2023
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

size_t bufsize = 0;

// Reads the input file and writes it to the screen
int display_file (FILE * input) {

    // Defining and allocating memory for buffer, which used for
    // reading file with getline()
    char *buffer = NULL;
    if((buffer = (char *)malloc(bufsize * sizeof(char))) == NULL)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }

    // Reading the file
    while (1) {
        size_t characters = getline(&buffer, &bufsize, input);

        // Stop reading input at EOF
        if (characters == -1) {
            break;
        }
        // Write the line to stdout
        printf("%s", buffer);
    }
    
    free(buffer);

    return(0);
}

// Function for opening the file with error handler
FILE * open_file(char * file_name, char * mode) {
    FILE * file = NULL;
    if((file = fopen(file_name, mode)) == NULL) {
        fprintf(stderr, "my-cat: cannot open file");
        exit(1);
    }
    return file;
}

int main (int argc, char * argv[]) {
    
    // Exit successfully if no input files are given
    if(argc == 1)
    {
        exit(0);
    }

    // Display all the files that were given as arguments
    for(int i = 1; i < argc; i++)
    {
        FILE * input_file = open_file(argv[i], "r");
        display_file(input_file);
        fclose(input_file);
        printf("\n");
    }

    return(0);
}
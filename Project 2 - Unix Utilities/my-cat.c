/* 
CT30A3370 Käyttöjärjestelmät ja systeemiohjelmointi
Project 2: Unix Utilities

Authors: Eetu Knutars & Joona Lappalainen
Last modified: 3.7.2023
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

size_t bufsize = 0;

// Read the input file and writes it reversed to the output file
int read_file (FILE * input) {

    char *buffer;
    if((buffer = (char *)malloc(bufsize * sizeof(char))) == NULL)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }

    while (1) {
        size_t characters = getline(&buffer, &bufsize, input);

        // Stop reading input on file end
        if (characters == -1) {
            break;
        }
        fprintf(stdout, "%s", buffer);
    }
    return(0);
}

FILE * open_file(char * file_name, char * mode) {
    FILE * file = NULL;
    if((file = fopen(file_name, mode)) == NULL) {
        fprintf(stderr, "error: cannot open file '%s'\n", file_name);
        exit(1);
    }
    return file;
}

int main (int argc, char * argv[]) {
    
    if(argc == 1)
    {
        fprintf(stderr, "Needs at least one input file.\n");
        exit(1);
    }

    for(int i = 1; i < argc; i++)
    {
        FILE * input_file = open_file(argv[i], "r");
        read_file(input_file);
        fclose(input_file);
        printf("\n");
    }

    return(0);
}
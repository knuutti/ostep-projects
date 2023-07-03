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

// Reverse function: writes the input file and writes it reversed to the output file
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
        // If input is read from stdin, user can end the read by pressing Ctrl+D
        if (characters == -1) {
            break;
        }
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
        // Read from user (standard input), write to screen (standard output)
        reverse(stdin, stdout);
    }
    else if (argc == 2)
    {
        // Read from input file, write to screen (standard output)
        FILE * input_file = open_file(argv[1], "r");
        reverse(input_file, stdout);
        fclose(input_file);
    }
    else if (argc == 3)
    {
        // Read from input file, write to output file
        if (argv[1] == argv[2])
        {
            fprintf(stderr, "Input and output file must differ.\n");
            exit(1);
        }
        FILE * input_file = open_file(argv[1], "r");
        FILE * output_file = open_file(argv[2], "w");
        reverse(input_file, output_file);
        fclose(input_file);
        fclose(output_file);
    }
    else
    {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    return(0);
}
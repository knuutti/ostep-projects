/* 
CT30A3370 Käyttöjärjestelmät ja systeemiohjelmointi
Project 2: Unix Utilities

Authors: Eetu Knutars & Joona Lappalainen
Last modified: 3.7.2023
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1024

size_t bufsize = 0;

// Reads the input file and writes it compressed to the output file
int read_file (FILE * input, FILE * output) {

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
            //fprintf(output, "%d%c", counter, current + '\0');
            fwrite(&counter, sizeof(int), 1, output);
            fwrite(&current, sizeof(char), 1, output);
            counter = 1;
            current = temp;
        }
        if (feof(input)) {
            break;
        }
    }

    return(0);
}

FILE * open_file(char * file_name, char * mode) {
    FILE * file = NULL;
    if((file = fopen(file_name, mode)) == NULL) {
        fprintf(stderr, "my-cat: cannot open file");
        exit(1);
    }
    return file;
}

int main (int argc, char * argv[]) {
    
    /* Exit successfully if no input files are given */
    if(argc == 1)
    {
        exit(0);
    }

    FILE * input_file = open_file(argv[1], "r");
    FILE * output_file = open_file(argv[2], "w");
    read_file(input_file, output_file);
    fclose(input_file);
    fclose(output_file);
    printf("\n");

    return(0);
}
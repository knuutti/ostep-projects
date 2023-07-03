/* 
CT30A3370 Käyttöjärjestelmät ja systeemiohjelmointi
Project 2: Unix Utilities

Authors: Eetu Knutars & Joona Lappalainen
Last modified: 3.7.2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1024

size_t bufsize = 0;

// Reads the input file and writes it decompressed to the screen
int read_file (FILE * input) {

    char * current = NULL;
    
    current = (char *)malloc(sizeof(char));

    while(1) {
            fread(current, sizeof(char), 1, input);
            printf("%s", current);
        
        if (feof(input)) {
            break;
        }
    }

    return(0);
}

FILE * open_file(char * file_name, char * mode) {
    FILE * file = NULL;
    if((file = fopen(file_name, mode)) == NULL) {
        fprintf(stderr, "my-unzip cannot open file");
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
    read_file(input_file);
    fclose(input_file);
    printf("\n");

    return(0);
}
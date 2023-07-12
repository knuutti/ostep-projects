/* 
CT30A3370 Käyttöjärjestelmät ja systeemiohjelmointi
Project 2: Unix Utilities

File: my-unzip.c
Authors: Eetu Knutars & Joona Lappalainen
Last modified: 12.7.2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Reads the input file and writes it decompressed to the screen
int read_file (FILE * input) {

    int tmp;
    int chr;
    
    // Reading the number of characters (1st fread) and the character (2nd fread)
    while(fread(&tmp, sizeof(int), 1, input)) 
    {   
        fread(&chr, sizeof(char), 1, input);
        
        // Printing the character as many times as the number before it says
        for(int i = 0; i < tmp; i++)
        {
            printf("%c", chr);
        }
    }

    return(0);
}

// Function for opening a file with error handler
FILE * open_file(char * file_name, char * mode) {
    FILE * file = NULL;
    if((file = fopen(file_name, mode)) == NULL) {
        fprintf(stderr, "my-unzip: cannot open file");
        exit(1);
    }
    return file;
}

int main (int argc, char * argv[]) {
    
    // Exit successfully if no input files are given
    if(argc == 1)
    {
        fprintf(stderr, "my-unzip: file1 [file2 ...]\n");
        exit(0);
    }

    // Unzipping all the input files
    FILE * input_file = NULL;
    for (int i = 1; i < argc; i++) {
        input_file = open_file(argv[1], "r");
    read_file(input_file);
    fclose(input_file);
    printf("\n");
    }

    return(0);
}
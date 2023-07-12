/* 
CT30A3370 Käyttöjärjestelmät ja systeemiohjelmointi
Project 2: Unix Utilities

File: my-zip.c
Authors: Eetu Knutars & Joona Lappalainen
Last modified: 12.7.2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Reads the input file and writes it compressed to the output file
int read_file (FILE * input, FILE * output) {

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
        
        // End of file, end of reading
        if (feof(input)) {
            break;
        }
    }

    return(0);
}

// Function for opening file with error handler
FILE * open_file(char * file_name, char * mode) {
    FILE * file = NULL;
    if((file = fopen(file_name, mode)) == NULL) {
        fprintf(stderr, "my-zip: cannot open file");
        exit(1);
    }
    return file;
}

int main (int argc, char * argv[]) {
    
    // Exit successfully if no input files are given
    if(argc < 3)
    {
        fprintf(stderr, "my-zip: file1 [file2 ...]\n");
        exit(0);
    }

    // Defining files and opening the output
    FILE * output_file = open_file(argv[argc-1], "w");
    FILE * input_file = NULL;

    // Zipping all the input files as one output file
    for (int i = 1; i < argc - 1; i++) {
        input_file = open_file(argv[1], "r");
        read_file(input_file, output_file);
        fclose(input_file);
    }
    
    fclose(output_file);
    printf("\n");

    return(0);
}
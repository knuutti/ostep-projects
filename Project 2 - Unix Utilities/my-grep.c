/* 
CT30A3370 Käyttöjärjestelmät ja systeemiohjelmointi
Project 2: Unix Utilities

File: my-grep.c
Authors: Eetu Knutars & Joona Lappalainen
Last modified: 11.7.2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function for searching the user-specified term
int search_contents(char * search_word, FILE * file) {
    
    char * buffer = NULL;
    size_t bufsize = 0;
    size_t characters = 0;
    while(1) {

        // Reading the input line by line until EOF
        characters = getline(&buffer, &bufsize, file);
        if (characters == -1) {
            break;
        }

        // If the term was found, print the entire line
        if (strstr(buffer, search_word)) {
            printf("%s", buffer);
        }
    }
    
    return(0);
}

int main(int argc, char * argv[]) {
    
    // Not enought arguments
    if (argc < 2) {
        fprintf(stderr, "my-grep: searchterm [file ...]");
        exit(1);
    } 
    // Read input from stdin
    else if (argc == 2) {
        search_contents(argv[1], stdin);
    }
    // Read input from files
    else {
        FILE * file = NULL;
        for (int i = 2; i < argc; i++) {
            if ((file = fopen(argv[i], "r")) == NULL) {
                fprintf(stderr, "my-grep: cannot open file\n");
                exit(1);
            } else {
                search_contents(argv[1], file);
            }
            fclose(file);
        }
    }
    return(0);
}


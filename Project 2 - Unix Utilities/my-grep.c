#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int search_contents(char * search_word, FILE * file) {
    
    char * buffer = NULL;
    size_t bufsize = 0;
    size_t characters = 0;
    while(1) {
        characters = getline(&buffer, &bufsize, file);
        if (characters == -1) {
            break;
        }
        if (strstr(buffer, search_word)) {
            printf("%s", buffer);
        }
    }
    
    return(0);
}

int main(int argc, char * argv[]) {
    
    FILE * file = NULL;
    for (int i = 2; i < argc; i++) {
        if ((file = fopen(argv[i], "r")) == NULL) {
            fprintf(stderr, "Failed to open file.\n");
            exit(1);
        } else {
            search_contents(argv[1], file);
        }
        fclose(file);
    }

    return(0);
}

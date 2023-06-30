#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

typedef struct rows {
    char * row;
    struct rows * next;
    struct rows * prev;
    } ROWS;

ROWS *pStart = NULL, *pEnd = NULL;
ROWS *pNew, *ptr;

size_t bufsize = 0;

int reverse (FILE *input, FILE *output) {

    char * buffer;
    buffer = (char *)malloc(bufsize * sizeof(char));
    if(buffer == NULL)
    {
        fprintf(stderr, "Unable to allocate buffer.\n");
        exit(1);
    }

    while (1) {
        size_t characters = getline(&buffer, &bufsize, input);

        if (characters == 1) {
            break;
        }

        if ((pNew = (ROWS*)malloc(sizeof(ROWS))) == NULL ){
            perror("Muistin varaus epäonnistui");
            exit(1);
        }
        if ((pNew->row = malloc(UINT_MAX)) == NULL ){
            perror("Muistin varaus epäonnistui");
            exit(1);
        }

        strcpy(pNew->row, buffer); 
        pNew->next = NULL;

        if (pStart == NULL) { 
            pStart = pNew;
            pEnd = pNew;
        } 
        else { 
            pEnd->next = pNew;
            pNew->prev = pEnd;
            pEnd = pNew;
        }

    }

    ptr = pEnd;
    while (ptr != NULL) {
        fprintf(output, "%s", ptr->row);
        ptr = ptr->prev;
    }

    ptr = pStart;
    while (ptr != NULL) {
        pStart = ptr->next;
        free(ptr);
        ptr = pStart;
    }
    
    return(0);
}

int main (int argc, char * argv[]) {

    if(argc == 1)
    {
        /* Read screen and write screen. */
        reverse(stdin, stdout);
    }
    else if (argc == 2)
    {
        FILE *input_file = NULL;
        /* Read file and write screen. */
        if((input_file = fopen(argv[1], "r")) == NULL)
        {
            fprintf(stderr, "Failed to open %s\n", argv[1]);
            exit(1);
        }

        reverse(input_file, stdout);
        
        fclose(input_file);
    }
    else if (argc == 3)
    {

        FILE * input_file = NULL;
        FILE * output_file = NULL;

        /* Read file and write file. */
        if (argv[1] == argv[2])
        {
            fprintf(stderr, "Input and output file must differ.\n");
            exit(1);
        }
        
        if((input_file = fopen(argv[1], "r")) == NULL)
        {
            fprintf(stderr, "Failed to open %s\n", argv[1]);
            exit(1);
        }
        
        if((output_file = fopen(argv[2], "w")) == NULL)
        {
            fprintf(stderr, "Failed to open %s\n", argv[2]);
            exit(1);
        }

        reverse(input_file, output_file);

        fclose(input_file);
        fclose(output_file);
    }
    else
    {
        fprintf(stderr, "This program should be invoked in one of the following ways:\n"
            "prompt> ./reverse\n"
            "prompt> ./reverse input.txt\n"
            "prompt> ./reverse input.txt output.txt\n");
    }

    printf("Kiitos ohjelman käytöstä.\n");

    return(0);
}
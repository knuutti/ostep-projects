#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Dynaaminen muistinvaraus ja getline
https://stackoverflow.com/questions/63309517/allocate-memory-for-a-dynamic-string-of-char
*/

int main(int argc, char * argv[])
{
    char * buffer;
    size_t bufsize = 0;
    // size_t characters;
    FILE * tiedosto_r;
    FILE * tiedosto_w;

    /* Muistin varaus */
    buffer = (char *)malloc(bufsize * sizeof(char));
    if(buffer == NULL)
    {
        fprintf(stderr, "Unable to allocate buffer.\n");
        exit(1);
    }

    if(argc == 1)
    {
        /* Read screen and write screen. */

        printf("Type something: ");
        
        while (getline(&buffer,&bufsize,stdin) != -1)
        {
            /* Tallennus listaan? */    
        }    
        
        
        // printf("%zu characters were read.\n",characters);
        printf("You typed: '%s'\n",buffer);
    }
    else if (argc == 2)
    {
        /* Read file and write screen. */
        tiedosto_r = fopen(argv[1], "r");
        while (getline(&buffer,&bufsize,tiedosto_r) != -1)
        {
            printf("%s", buffer);
        }
        printf("\n");
        fclose(tiedosto_r);
    }
    else if (argc == 3)
    {
        /* Read file and write file. */
        if (argv[1] == argv[2])
        {
            fprintf(stderr, "Input and output file must differ.\n");
            exit(1);
        }
        
        tiedosto_r = fopen(argv[1], "r");
        fclose(tiedosto_r);

        tiedosto_w = fopen(argv[2], "w");
        fclose(tiedosto_w);
    }
    else
    {
        fprintf(stderr, "This program should be invoked in one of the following ways:\n"
            "prompt> ./reverse\n"
            "prompt> ./reverse input.txt\n"
            "prompt> ./reverse input.txt output.txt\n");
    }
    
    free(buffer);
    
    return 0;
}
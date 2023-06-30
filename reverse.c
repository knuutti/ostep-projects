#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* Creating list structure */
struct list
{
    char * line;
    struct list * next;
    struct list * prev;
};

int main(int argc, char * argv[])
{
    char * buffer;
    size_t bufsize = 0;
    FILE * tiedosto_r;
    FILE * tiedosto_w;

    /* List pointers */
    struct list * pStart = NULL, * pEnd = NULL, * ptr;

    /* Allocating memory for input buffer (pitää vissiin siirtää) */
    buffer = (char *)malloc(bufsize * sizeof(char));
    if(buffer == NULL)
    {
        fprintf(stderr, "Unable to allocate buffer.\n");
        exit(1);
    }

    if(argc == 1)
    {
        /* Read screen and write screen. */

        printf("Type something:\n");
        
        while (getline(&buffer,&bufsize,stdin) != -1)
        {   
            /* Allocating memory */
            if ((ptr = (struct list*)malloc(sizeof(struct list))) == NULL)
            {
                fprintf(stderr, "Unable to allocate buffer.\n");
                exit(1);
            }

            /* Allocating memory */
            if ((ptr->line = malloc(sizeof(UINT_MAX))) == NULL)
            {
                fprintf(stderr, "Unable to allocate buffer.\n");
                exit(1);
            }

            /* New node values */
            strcpy(ptr->line,buffer);
            ptr->next = NULL;

            /* Adding new node to the end of the list */
            if(pStart == NULL) /* If empty list */
            {
                pStart = ptr;
                pEnd = ptr;
            }
            else /* To end of the list */
            {
                pEnd->next = ptr;
                ptr->prev = pEnd;
                pEnd = ptr;
            }
        }    

        /* Printing list in reverse order */
        ptr = pEnd;
        printf("You typed:\n");
        while(ptr != NULL)
        {
            printf("%s", ptr->line);
            ptr = ptr->prev;
        }
        
        /* Free used memory */
        ptr = pStart;
        while (ptr != NULL) {
            pStart = ptr->next;
            free(ptr);
            ptr = pStart;
        }
    }
    else if (argc == 2)
    {
        /* Read file and write screen. */
        if((tiedosto_r = fopen(argv[1], "r")) == NULL)
        {
            fprintf(stderr, "Failed to open %s\n", argv[1]);
            exit(1);
        }

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
        
        if((tiedosto_r = fopen(argv[1], "r")) == NULL)
        {
            fprintf(stderr, "Failed to open %s\n", argv[1]);
            exit(1);
        }
        
        if((tiedosto_w = fopen(argv[2], "w")) == NULL)
        {
            fprintf(stderr, "Failed to open %s\n", argv[2]);
            exit(1);
        }

        while (getline(&buffer,&bufsize,tiedosto_r) != -1)
        {
            fprintf(tiedosto_w, "%s", buffer);
        }

        fclose(tiedosto_r);
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

    printf("Kiitos ohjelman käytöstä.\n");

    return 0;
}
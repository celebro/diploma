#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
void main(int argc, char ** argv)
{
    int rc, i, nodes;
    FILE *starfile;
    char name[100];
    
    if (argc != 2)
        {
            printf("USAGE: create_star <number of nodes>\n");
            exit(0);
        }

    nodes = atoi(argv[1]);
    sprintf(name, "unistar_%d.txt", nodes);
    
    starfile= fopen(name, "w");
    if (starfile == NULL)
        {
            printf("Could not open file name %s for writing\n", name);
            exit(0);  
        }

    /* Write node data to file */
    for (i=0; i<nodes; i++)
        {
            fprintf(starfile, "node a %d\n", i);
        }

    /* Write edge data to file */
    for (i=1; i<nodes; i++)
        {
            fprintf(starfile, "edge 0 %d edge\n", i);
        }

    fclose(starfile);
}

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
void main(int argc, char ** argv)
{
    int rc, i, j, nodes;
    FILE *starfile;
    char name[100];
    
    if (argc != 2)
        {
            printf("USAGE: create_clique <number of nodes>\n");
            exit(0);
        }

    nodes = atoi(argv[1]);
    sprintf(name, "uniclique_%d.txt", nodes);
    
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
    for (i=0; i<nodes; i++)
        {
            for (j=i+1; j<nodes; j++)
               fprintf(starfile, "edge %d %d edge\n", i, j);
        }

    fclose(starfile);
}

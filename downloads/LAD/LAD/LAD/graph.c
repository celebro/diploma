// This software has been written by Chrstine Solnon.
// It is distributed under the CeCILL-B FREE SOFTWARE LICENSE
// see http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html for more details

typedef struct{
	int nbVertices; // Number of vertices
	int* nbSucc;    // nbSucc[i] = number of successors of i
	int** succ;     // forall j in [0..nbSucc[i]-1], succ[i][j] = jth successor of i 
	bool** isEdge;   // isEdge[i][j] = true if (i,j) is an edge; false otherwise
} Tgraph;

void createGraph(char* fileName, Tgraph* graph){
	// reads data in fileName and create the corresponding graph
	
	FILE* fd;
	int i, j;
	
	if ((fd=fopen(fileName, "r"))==NULL){
		printf("ERROR: Cannot open ascii input file %s\n", fileName); 
		exit(1);
	}
	
	if (fscanf(fd,"%d",&(graph->nbVertices)) != 1){
		printf("ERROR while reading input file %s\n", fileName); 
		exit(1);
	}
	graph->nbSucc = (int*)calloc(graph->nbVertices,sizeof(int));
	graph->succ = (int**)calloc(graph->nbVertices,sizeof(int*));
	graph->isEdge = (bool**)calloc(graph->nbVertices,sizeof(bool*));
	
	for (i=0; i<graph->nbVertices; i++){
		graph->isEdge[i] = (bool*)calloc(graph->nbVertices,sizeof(bool));
		memset(graph->isEdge[i],false,graph->nbVertices);
		// reading degree and successors of vertex i
		if ((fscanf(fd,"%d",&(graph->nbSucc[i])) != 1) || (graph->nbSucc[i] < 0) || (graph->nbSucc[i]>=graph->nbVertices) || (feof(fd))) {
			printf("ERROR while reading input file %s\n", fileName); 
			exit(1);
		}
		graph->succ[i] = (int*)calloc(graph->nbSucc[i],sizeof(int));
		for (j=0; j<graph->nbSucc[i]; j++){
			if ((fscanf(fd,"%d",&(graph->succ[i][j])) != 1) || (graph->succ[i][j]<0) || (graph->succ[i][j]>=graph->nbVertices) || (feof(fd))){
				printf("ERROR while reading input file %s\n", fileName); 
				exit(1);
			}
			if (graph->isEdge[i][graph->succ[i][j]]){
				printf("ERROR while reading input file %s (the successors of a node should be all different)\n",fileName);
				exit(1);
			}
			graph->isEdge[i][graph->succ[i][j]] = true;
		}
	}
	fclose(fd);
}

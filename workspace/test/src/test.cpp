//============================================================================
// Name        : test.cpp
// Author      : Nejc Ramovs
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <argedit.h>
#include <argraph.h>
#include <vf2_sub_state.h>
#include <match.h>

using namespace std;

#define MAXNODES 200

Graph * get_full_graph(int count)
{
	ARGEdit ed;  // The object used to create the graph
	// Insert the three nodes
	for(int i=0; i<count; i++)
	  ed.InsertNode(NULL); // The inserted node will have index i.
						   // NULL stands for no semantic attribute.
	// Insert the edges
	for(int i=0; i<count; i++)
	  for(int j=0; j<count; j++)
		if (i!=j)
			  ed.InsertEdge(i, j, NULL); // NULL stands for no sem. attribute.

	return new Graph(&ed);
}

bool match_printer_visitor(int n, node_id ni1[], node_id ni2[], void *usr_data)
{
	cout << "Found a mathing with " << n << " nodes:" << endl;

	int i;
	for(i=0; i<n; i++) {
		cout << "\tNode " << ni1[i] << " of graph 1 is paired with node " << ni2[i] << " of graph 2" << endl;
	}

	// Return false to search for the next matching
	return false;

}

int main() {
	Graph *small_graph = get_full_graph(3);
	Graph *large_graph = get_full_graph(4);

	VF2SubState s0(small_graph, large_graph);

	match(&s0, match_printer_visitor, NULL);

	delete(small_graph);
	delete(large_graph);

	cout << "Finished!" << endl;

	return 0;
}


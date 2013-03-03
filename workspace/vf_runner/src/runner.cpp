//============================================================================
// Name        : runner.cpp
// Author      : 
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <time.h>
#include <cstdarg>

#include <argedit.h>
#include <argraph.h>
#include <vf2_sub_state.h>
#include <ull_sub_state.h>
#include <match.h>
using namespace std;

const int FLAG_ALG_VF = 1 << 0;

bool fileexists(const char *filename) {
  ifstream ifile(filename);
  return ifile;
}

inline unsigned int r(ifstream& in) {
	unsigned char a, b;
	a = (char)(in.get());
	b = (char)(in.get());
	return a | (b << 8);
}

Graph* readGraphBinary(const char* filename) {
	ARGEdit ed;

    ifstream in (filename, ios::in|ios::binary);
    int n = r(in);	// number of vertices
    for(int i = 0; i < n; i++)
    	  ed.InsertNode(NULL);

    for (int i = 0; i < n; i++) {
    	int cnt = r(in);
        for (int j = 0; j < cnt; j++) {
            int k = r(in);
            ed.InsertEdge(i, k, NULL);
        }
    }

    in.close();
    return new Graph(&ed);
}

bool match_printer_visitor(int n, node_id ni1[], node_id ni2[], void *usr_data)
{
	// Return false to search for the next matching
	return false;
}

timespec timespec_diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

int main(int argc, char* argv[]) {
	char* a;
	char* b;
	int flags = 0;
	timespec start_t;
	timespec end_t;

	a = "/home/celebro/data/fri/diploma/src/workspace/data/mivia/si2/rand/r001/100.A00";
	b = "/home/celebro/data/fri/diploma/src/workspace/data/mivia/si2/rand/r001/100.B00";
	flags = 1;

	if (argc >= 4) {
		a = argv[1];
		b = argv[2];
		flags = atoi(argv[3]);
	}

	if (!fileexists(a) || !fileexists(b)) {
		cerr << "One of the files not found." << endl;
		return 1;
	}

	cout.flush();

	Graph* graph_pattern = readGraphBinary(a);
	Graph* graph_target = readGraphBinary(b);

	State* state;

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_t);
	clock_t start = clock();

	if (flags && FLAG_ALG_VF) {
		state = new VF2SubState(graph_pattern, graph_target);
		//cout << "vf ";
	} else {
		state = new UllSubState(graph_pattern, graph_target);
		//cout << "ull ";
	}
	int cnt = match(state, match_printer_visitor, NULL);

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_t);
	timespec diff_t = timespec_diff(start_t, end_t);
	double time_t = (diff_t.tv_sec * 1000000 + diff_t.tv_nsec/1000)/1000.0;

	cout.precision(2);
	printf("%.2f", time_t);

	delete(graph_pattern);
	delete(graph_target);

	return 0;
}

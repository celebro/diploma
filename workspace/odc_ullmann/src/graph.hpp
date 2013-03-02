/*
 * graph.hpp
 *
 *  Created on: 20. avg. 2012
 *      Author: jure
 */

#ifndef GRAPH_HPP_
#define GRAPH_HPP_

#include <iostream>
#include <fstream>
#include <cstring>
#include "stack.hpp"


class Digraph {
private:
	ListStack* inn;
	ListStack* outn;
protected:
	int nodeCnt;
	bool* adjacency;
	int *indegs;
	int *outdegs;
	int **inneighs;
	int **outneighs;
public:
	Digraph(const char* filename): inn(), outn(), nodeCnt(0), adjacency(), indegs(), outdegs(), inneighs(), outneighs() {
	    readGraphBinary(filename);
	}

	~Digraph() {
		delete adjacency;
		delete indegs;
		delete outdegs;
		for (int i = 0; i < nodeCnt; i++) {
			delete inneighs[i];
			delete outneighs[i];
		}
		delete inneighs;
		delete outneighs;
	}

	void prepare(int size) {
		nodeCnt = size;
		adjacency = new bool[size * size];
		memset(adjacency, 0, sizeof(bool) * size * size);
		indegs = new int[size];
		memset(indegs, 0, sizeof(int) * size);
		outdegs = new int[size];
		memset(outdegs, 0, sizeof(int) * size);
		inn = new ListStack[size];
		outn = new ListStack[size];
	}

	void print() {
		std::cout << "Graph: " << nodeCnt << endl;
//        std::cout << "Matrix: " << rows << "x" << cols << " Ones: " << cnt1 << " Zeros: " << (size-cnt1) << endl;
	}

    void setEdge(int i, int j) {
        if (edge(i, j)) return;
        adjacency[i * size() + j] = true;
        outdegs[i]++;
        indegs[j]++;
        inn[j].push(i);
        outn[i].push(j);
    }

    void finish() {
    	inneighs = new int* [nodeCnt];
    	outneighs = new int* [nodeCnt];
    	for (int i = 0; i < nodeCnt; i++) {
    		inneighs[i] = inn[i].toArray();
    		outneighs[i] = outn[i].toArray();
    	}
    	delete[] inn;
    	delete[] outn;
    }

    inline unsigned int r(ifstream& in) {
    	unsigned char a, b;
    	a = (char)(in.get());
    	b = (char)(in.get());
    	return a | (b << 8);
    }

    void readGraphBinary(const char* filename) {
        ifstream in (filename, ios::in|ios::binary);
        int n = r(in);
//        cout << "n: " << n << endl;
        prepare(n);
        for (int i = 0; i < n; i++) {
        	int cnt = r(in);
//            cout << "  cnt: " << cnt << endl;
            for (int j = 0; j < cnt; j++) {
                int k = r(in);
                setEdge(i, k);
//                cout << i << " " << k << endl;
            }
        }
        finish();
    }

    inline int size() {
    	return nodeCnt;
    }

    inline bool node(int i) {
    	return i < size();
    }

    inline int degree(int i) {
    	return indegs[i] + outdegs[i];
    }

    inline int indegree(int i) {
    	return indegs[i];
    }

    inline int outdegree(int i) {
    	return outdegs[i];
    }

    inline int* inneigh(int i) {
    	return inneighs[i];
    }

    inline int* outneigh(int i) {
    	return outneighs[i];
    }

    inline bool edge(int i, int j) {
    	return adjacency[i * nodeCnt + j];
    }

    inline bool biedge(int i, int j) {
    	return adjacency[i * nodeCnt + j] || adjacency[j * nodeCnt + i];
    }

};


#endif /* GRAPH_HPP_ */

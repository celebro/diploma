/*
 * isoalg.hpp
 *
 *  Created on: 29. avg. 2012
 *      Author: jure
 */

#ifndef ISOALG_HPP_
#define ISOALG_HPP_

#include <cstring>
#include <ctime>
#include "map.hpp"


class O {
protected:
	Digraph &g;
	Digraph &h;
	int isoCount;
    Map* map;
    int* iso;
    bool* usedcols;
    int* order;

    // bydepthdeg
	int* sortnodes() {
		int* avail = new int[g.size()];
		memset(avail, 0, sizeof(int) * g.size());
		int* order = new int[g.size()];
		memset(avail, 0, sizeof(int) * g.size());
		int idx = 0;
		while (idx < g.size()) {
	        // find node with max degree
	    	int best = -1;
			for (int i = 0; i < g.size(); i++)
				if (avail[i] == 0 && (best < 0 || g.degree(i) > g.degree(best))) best = i;
			for (int *p = g.outneigh(best); *p >= 0; p++) avail[*p] = 1;
			for (int *p = g.inneigh(best); *p >= 0; p++) avail[*p] = 1;
			avail[best] = -1;
			order[idx] = best;
			idx++;
	        // process all accessible vertices
			best = 0;
			while (idx < g.size() && best >= 0) {
				best = -1;
				for (int i = 0; i < g.size(); i++)
					if (avail[i] == 1 && (best < 0 || g.degree(i) > g.degree(best))) best = i;
				if (best != -1) {
					for (int *p = g.outneigh(best); *p >= 0; p++) if (avail[*p] == 0) avail[*p] = 1;
					for (int *p = g.inneigh(best); *p >= 0; p++) if (avail[*p] == 0) avail[*p] = 1;
					avail[best] = -1;
					order[idx] = best;
					idx += 1;
				}
			}
		}
        delete[] avail;
		return order;
	}

	void init() {
		// possible mappings
		map = new Map(g.size(), h.size());
		for (int i = 0; i < g.size(); i++)
			for (int j = 0; j < h.size(); j++)
				(*map)(i, j) = h.indegree(j) >= g.indegree(i) && h.outdegree(j) >= g.outdegree(i);
//		map->print();
    	// iso mapping
		iso = new int[g.size()];
		for (int i = 0; i < g.size(); i++) iso[i] = -1;
		// used columns
		usedcols = new bool[h.size()];
		memset(usedcols, 0, sizeof(bool) * h.size());
		// order
	    order = sortnodes();
//    	cout << "Order: "; for (int i = 0; i < g.size(); i++) cout << order[i] << " "; cout << endl;
	    //
		refineMapFull(0);
	}

 	void done() {
    	delete[] order;
    	delete[] usedcols;
    	delete[] iso;
    	delete map;
 	}

	bool canMap(int row, int col) {
		for (int *i = g.outneigh(row); *i >= 0; i++) {
			if (iso[*i] >= 0) continue;
			bool all0 = true;
			for (int *j = h.outneigh(col); *j >= 0; j++)
				if ((*map)(*i, *j)) { all0 = false; break; }
			if (all0) return false;
		}
		for (int *i = g.inneigh(row); *i >= 0; i++) {
			if (iso[*i] >= 0) continue;
			bool all0 = true;
			for (int *j = h.inneigh(col); *j >= 0; j++)
				if ((*map)(*i, *j)) { all0 = false; break; }
			if (all0) return false;
		}
		return true;
	}

	void refineMapFull(int level) {
		do {
			map->reset();
			for (int i = 0; i < g.size(); i++) if (iso[i] < 0)
				for (int j = 0; j < h.size(); j++)
					if ((*map)(i, j) && !canMap(i, j)) map->annul(i, j);
		} while (map->changes() > level);
	}

	void refineMapPartial(int level, int row, int col) {
		do {
			map->reset();
	    	for (int *i = g.outneigh(row); *i >= 0; i++) if (iso[*i] < 0)
	    		for (int *j = h.outneigh(col); *j >= 0; j++)
					if ((*map)(*i, *j) && !canMap(*i, *j)) map->annul(*i, *j);
	    	for (int *i = g.inneigh(row); *i >= 0; i++) if (iso[*i] < 0)
	    		for (int *j = h.inneigh(col); *j >= 0; j++)
					if ((*map)(*i, *j) && !canMap(*i, *j)) map->annul(*i, *j);
		} while (map->changes() > level);
	}

	// odpullman refine
	bool refine(int depth, int row, int col) {
		// vrstica
		for (int j = 0; j < h.size(); j++) map->annul(row, j);
    	for (int *i = g.outneigh(row); *i >= 0; i++) if (iso[*i] < 0)
    		for (int j = 0; j < h.size(); j++) if (!usedcols[j] && !h.edge(col, j)) map->annul(*i, j);
    	for (int *i = g.inneigh(row); *i >= 0; i++) if (iso[*i] < 0)
    		for (int j = 0; j < h.size(); j++) if (!usedcols[j] && !h.edge(j, col)) map->annul(*i, j);
		// stolpec
/*	    for (int i = 0; i < g.size(); i++) map->annul(i, col);
    	for (int *j = h.outneigh(col); *j >= 0; j++) if (!usedcols[*j])
    	    for (int i = 0; i < g.size(); i++) if (iso[i] < 0 && !g.edge(row, i)) map->annul(i, *j);
    	for (int *j = h.inneigh(col); *j >= 0; j++) if (!usedcols[*j])
    	    for (int i = 0; i < g.size(); i++) if (iso[i] < 0 && !g.edge(i, row)) map->annul(i, *j); //*/
	    for (int i = 0; i < g.size(); i++) if (iso[i] < 0) {
		    map->annul(i, col);
		    // vsi, ki niso sosedje od row, se ne morejo mapirati na sosede of col
	        if (!g.edge(row, i))
		    	for (int *j = h.outneigh(col); *j >= 0; j++) map->annul(i, *j);
	        if (!g.edge(i, row))
		    	for (int *j = h.inneigh(col); *j >= 0; j++) map->annul(i, *j);
	    }//
/*    	if (depth <= 0)
    		refineMapFull(1);
    	else
*/    		if (depth <= g.size()/3)
    		refineMapPartial(depth, row, col);
		return canMap(row, col);
	}

	void algorithm(bool enumerate) {
    	int depth = 0;
        while (true) {
        	while (depth < g.size()) {
        	    int row = order[depth];									// get next node in order
        	    if (iso[row] >= 0) usedcols[iso[row]] = false;			// reset previous visit
	            // find next available column
	            int col = iso[row] + 1;
	            while (col < h.size() && !(*map)(row, col)) col += 1;
	            if (col < h.size()) {									// if column available
//	            	if (depth == 0) cout << row << " -> " << col << endl;	// any progress?
	            	iso[row] = col;										// map node -> col
	            	usedcols[col] = true;								// mark column as used
	            	map->snapshot();
	            	if (refine(depth, row, col)) depth += 1;
	            	else map->undo();
                // otherwise no column found
	            } else {
	                iso[row] = -1;			// backtrack iso
	                depth -= 1;				// backtrack
	                if (depth < 0) return;	// all paths tried?
	                map->undo();
	            }
        	}
        	// all rows mapped (row == g.size)
    	    isoCount++;
    	    if (!enumerate) return;
            map->undo();
        	depth -= 1;
        }
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

public:
	O(Digraph &g, Digraph &h): g(g), h(h), isoCount(0) {}

    void find(bool all) {
    	timespec start_t;
    	timespec end_t;

    	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_t);
		//clock_t start = clock();
		init();
        algorithm(all);
        done();
        //clock_t diff = (clock() - start);
        //int time = (int) (diff * 1000 / (double) CLOCKS_PER_SEC);
    	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_t);
    	timespec diff_t = timespec_diff(start_t, end_t);
    	double time_t = (diff_t.tv_sec * 1000000 + diff_t.tv_nsec/1000)/1000.0;

        //cout << isoCount << " " << time << endl;
    	cout.precision(2);
    	printf("%.2f", time_t);
    }
};


#endif /* ISOALG_HPP_ */

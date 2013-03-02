/*
 * map.hpp
 *
 *  Created on: 20. avg. 2012
 *      Author: jure
 */

#ifndef MAP_HPP_
#define MAP_HPP_

using namespace std;


class Map {
protected:
	int rows;
	int cols;
	int size;
	// current matrix
	bool* data;
	// history - stack of all changes
	int* history;
	int index;
	// shots - snapshosts in history
	int* shots;
	int shotidx;
	// no of changes after reset
	int changed;

public:
	Map(int rows, int cols) {
		this->rows = rows;
		this->cols = cols;
		size = rows * cols;
		data = new bool[size];
		memset(data, 0, sizeof(bool) * size);
		history = new int[size];
		memset(history, 0, sizeof(int) * size);
		index = -1;
		shots = new int[rows];
		memset(shots, 0, sizeof(int) * rows);
		shotidx = -1;
	}

	~Map() {
		delete[] shots;
		delete[] history;
		delete[] data;
	}

	inline bool& operator()(int row, int col) {
	    return data[row * cols + col];
	}

	inline const bool operator()(int row, int col) const {
	    return data[row * cols + col];
	}

	inline void snapshot() {
        //if (shotidx >= rows) throw new StackOverFlowException();
		shots[++shotidx] = index;
		changed = false;
	}

	inline void undo() {
        //if (shotidx == -1) throw new StackUnderFlowException();
		int stop = shots[shotidx--];
		while (index > stop)
			data[history[index--]] = true;
		changed = false;
	}

	inline void annul(int row, int col) {
		register int idx = row * cols + col;
		if (data[idx]) {
			data[idx] = false;
			history[++index] = idx;
			changed = true;
		}
	}

	inline void reset() {
		changed = 0;
	}

	inline int changes() {
		return changed;
	}

	void print() {
        int cnt1 = 0;
        for (int i = 0; i < size; i++)
			if (data[i]) cnt1++;
        std::cout << "Matrix: " << rows << "x" << cols << " Ones: " << cnt1 << " Zeros: " << (size-cnt1) << endl;
	}
};


#endif /* MAP_HPP_ */

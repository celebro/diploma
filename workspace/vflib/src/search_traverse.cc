/*
 * search_traverse.cc
 *
 *  Created on: Mar 6, 2013
 *      Author: celebro
 */

#include <algorithm>
#include <vector>
#include <cstdio>
#include <climits>
#include <cstring>
#include <iostream>
#include "argraph.h"

// This class calculates order as described in Subsea algorithm - depth
// first search and choose best with breadth first search
class SearchTraverse {
private:
	int n;
	node_id* order;
	bool *processed;
	bool *estimateProcessed;
	int index;

	Graph *g;

	struct NodeEstimate {
		node_id id;
		int steps;
		int size;
	};

	// a < b
	bool nodeEstimateCompare(NodeEstimate a, NodeEstimate b) {
		if (a.steps == b.steps) {
			return a.size < b.size;
		} else {
			return a.steps < b.steps;
		}
	}

	// Breadth first search
	NodeEstimate getNodeEstimate(node_id start, node_id from) {
		memset(estimateProcessed, 0, sizeof(bool) * n);

		NodeEstimate e;
		e.id = start;
		e.steps = 1;
		e.size = 0;
		int total = 0;

		std::vector<node_id> s;
		std::vector<node_id> n;
		s.push_back(start);

		do {
			int p = 0;
			bool skippedEdge = false;
			for (unsigned int idx = 0; idx < s.size(); idx++) {				// Loop visited nodes
				for (int i = 0; i < g->InEdgeCount(s.at(idx)); i++) {		// Loop node neighbors
					node_id n1 = g->GetInEdge(s.at(idx), i);
					if (s.at(idx) == start && n1 == from && skippedEdge == false) {	// Do not use edge start->from
						skippedEdge = true;		// Only skip one edge
						continue;
					}
					if (!estimateProcessed[n1]) {							// Add to visited
						n.push_back(n1);
						estimateProcessed[n1] = true;
						if (processed[n1])
							p++;											// Count, if node is in goal
					}
				}
				for (int i = 0; i < g->OutEdgeCount(s.at(idx)); i++) {
					node_id n1 = g->GetOutEdge(s.at(idx), i);
					if (s.at(idx) == start && n1 == from && skippedEdge == false) {	// Do not use edge start->from
						skippedEdge = true;
						continue;
					}
					if (!estimateProcessed[n1]) {
						n.push_back(n1);
						estimateProcessed[n1] = true;
						if (processed[n1])
							p++;
					}
				}
			}
			if (p > 0) {
				e.size = -p;
				return e;
			} else {
				e.steps++;
				total += n.size();
				s.clear();
				s.swap(n);
			}

		} while (s.size() > 0);

		e.steps = 99999999;
		e.size = total;

		return e;
	}

	// Depth first visit, in best-first order
	void Visit(node_id node) {
		order[index++] = node;
		processed[node] = true;

		// Find all non-processed neighbors of added node
		std::vector<node_id> neighbors;
		for (int i = 0; i < g->InEdgeCount(node); i++) {
			node_id n1 = g->GetInEdge(node, i);
			if (!processed[n1]) {
				neighbors.push_back(n1);
			}
		}
		for (int i = 0; i < g->OutEdgeCount(node); i++) {
			node_id n1 = g->GetOutEdge(node, i);
			if (!processed[n1]) {
				neighbors.push_back(n1);
			}
		}

		// Visit all neighbors in best-first order
		while (neighbors.size() > 0) {
			int bestNodeIdx = -1;
			node_id bestNode = NULL_NODE;
			NodeEstimate bestEstimate;

			// Find best neighbor
			for (unsigned int i = 0; i < neighbors.size(); i++) {
				node_id n1 = neighbors.at(i);
				if (!processed[n1]) {
					NodeEstimate nEstiname = getNodeEstimate(n1, node);
					if (bestNode == NULL_NODE || nodeEstimateCompare(nEstiname, bestEstimate)) {
						bestNodeIdx = i;
						bestNode = n1;
						bestEstimate = nEstiname;
					}
				}
			}

			if (bestNode != NULL_NODE) {
				Visit(bestNode);
				neighbors[bestNodeIdx] = neighbors[neighbors.size()-1];	// Remove visited neighbor
				neighbors.pop_back();
			} else {
				break;	// No more unvisited neighbors
			}
		}

	}

	void printGraph() {
		for (int node = 0; node < g->NodeCount(); node++) {
			std::cout << node << " -- {";
			for (int i = 0; i < g->InEdgeCount(node); i++) {
				node_id n1 = g->GetInEdge(node, i);
				std::cout << n1 << " ";
			}
			std::cout << "}" << std::endl;
		}
	}

	void printOrder() {
		for (int i = 0; i < n; i++) {
			std::cout << order[i] << " ";
		}
		std::cout << std::endl;
	}

public:
	node_id * SortNodesBySearchTraverse(Graph *g) {
		n = g->NodeCount();
		order = new node_id[n];
		processed = new bool[n];
		estimateProcessed = new bool[n];
		index = 0;
		this->g = g;
		for (int i = 0; i < n; i++) {
			order[i] = NULL_NODE;
			processed[i] = false;
		}
		// Find first node - the one with highest degree (not part of Subsea)
		// Loop if graph is not connected
		while (index < n) {
			int first = -1;
			int firstEdgeCnt = -1;
			for (int i = 0; i < n; i++) {
				if (!processed[i] && g->EdgeCount(i) > firstEdgeCnt) {
					first = i;
					firstEdgeCnt = g->EdgeCount(i);
				}
			}
			Visit(first);
		}

//		printGraph();
//		printOrder();

		delete[] processed;
		delete[] estimateProcessed;
		return order;
	}
};







// This class calculates order similar as described in Subsea algorithm,
// but it doesn't go dept first: after every selected node, it takes into
// consideration every reachable nonvisited node
class SearchTraverse2 {
private:
	int n;
	node_id* order;
	bool *processed;
	bool *estimateProcessed;

	Graph *g;

	struct NodeEstimate {
		node_id id;
		int steps;
		int size;
	};

	// a < b
	bool nodeEstimateCompare(NodeEstimate a, NodeEstimate b) {
		if (a.steps == b.steps) {
			return a.size < b.size;
		} else {
			return a.steps < b.steps;
		}
	}

	// Breadth first search
	NodeEstimate getNodeEstimate(node_id start) {
		memset(estimateProcessed, 0, sizeof(bool) * n);

		NodeEstimate e;
		e.id = start;
		e.steps = 1;
		e.size = 0;
		int total = 0;

		std::vector<node_id> s;
		std::vector<node_id> n;
		s.push_back(start);

		do {
			int p = 0;
			bool skippedEdge = false;
			for (unsigned int idx = 0; idx < s.size(); idx++) {				// Loop visited nodes
				for (int i = 0; i < g->InEdgeCount(s.at(idx)); i++) {		// Loop node neighbors
					node_id n1 = g->GetInEdge(s.at(idx), i);
					if (s.at(idx) == start && processed[n1] && !skippedEdge) {	// Do not use edge start->from
						skippedEdge = true;		// Only skip one edge
						continue;
					}
					if (!estimateProcessed[n1]) {							// Add to visited
						n.push_back(n1);
						estimateProcessed[n1] = true;
						if (processed[n1])
							p++;											// Count, if node is in goal
					}
				}
				for (int i = 0; i < g->OutEdgeCount(s.at(idx)); i++) {
					node_id n1 = g->GetOutEdge(s.at(idx), i);
					if (s.at(idx) == start && processed[n1] && !skippedEdge) {	// Do not use edge start->from
						skippedEdge = true;
						continue;
					}
					if (!estimateProcessed[n1]) {
						n.push_back(n1);
						estimateProcessed[n1] = true;
						if (processed[n1])
							p++;
					}
				}
			}
			if (p > 0) {
				e.size = -p;
				return e;
			} else {
				e.steps++;
				total += n.size();
				s.clear();
				s.swap(n);
			}

		} while (s.size() > 0);

		e.steps = 99999999;
		e.size = total;

		return e;
	}

	void printGraph() {
		for (int node = 0; node < g->NodeCount(); node++) {
			std::cout << node << " -- {";
			for (int i = 0; i < g->InEdgeCount(node); i++) {
				node_id n1 = g->GetInEdge(node, i);
				std::cout << n1 << " ";
			}
			std::cout << "}" << std::endl;
		}
	}

	void printOrder() {
		for (int i = 0; i < n; i++) {
			std::cout << order[i] << " ";
		}
		std::cout << std::endl;
	}

public:
	node_id * SortNodesBySearchTraverse(Graph *g) {
		n = g->NodeCount();
		order = new node_id[n];
		processed = new bool[n];
		estimateProcessed = new bool[n];
		bool *isReachable = new bool[n];

		this->g = g;
		for (int i = 0; i < n; i++) {
			order[i] = NULL_NODE;
			processed[i] = false;
			isReachable[i] = false;
		}

		std::vector<node_id> reachable;
		for (int index = 0; index < n; index++) {
			// Start with highest degree - on index == 0 or disconnected graph
			if (reachable.size() == 0) {
				int best = -1;
				int bestEdgeCnt = -1;
				for (int i = 0; i < n; i++) {
					if (!processed[i] && g->EdgeCount(i) > bestEdgeCnt) {
						best = i;
						bestEdgeCnt = g->EdgeCount(i);
					}
				}
				reachable.push_back(best);
				isReachable[best] = true;
			}

			// Find best reachable unprocessed node
			int bestNodeIdx = -1;
			node_id bestNode = NULL_NODE;
			NodeEstimate bestEstimate;
			for (unsigned int i = 0; i < reachable.size(); i++) {
				node_id n1 = reachable.at(i);
				NodeEstimate nEstiname = getNodeEstimate(n1);
				if (bestNode == NULL_NODE || nodeEstimateCompare(nEstiname, bestEstimate)) {
					bestNodeIdx = i;
					bestNode = n1;
					bestEstimate = nEstiname;
				}
			}
			order[index] = bestNode;
			processed[bestNode] = true;
			reachable[bestNodeIdx] = reachable[reachable.size()-1];	// Remove added node from reachable
			reachable.pop_back();

			for (int i = 0; i < g->InEdgeCount(bestNode); i++) {
				node_id n1 = g->GetInEdge(bestNode, i);
				if (!processed[n1] && !isReachable[n1]) {
					reachable.push_back(n1);
					isReachable[n1] = true;
				}
			}
			for (int i = 0; i < g->OutEdgeCount(bestNode); i++) {
				node_id n1 = g->GetOutEdge(bestNode, i);
				if (!processed[n1] && !isReachable[n1]) {
					reachable.push_back(n1);
					isReachable[n1] = true;
				}
			}

		}

//		printGraph();
//		printOrder();

		delete[] processed;
		delete[] estimateProcessed;
		return order;
	}

};


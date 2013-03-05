/*------------------------------------------------------------------
 * vf2_sub_state.cc
 * Implementation of the class VF2SubState
 *
 * Author: P. Foggia
 *-----------------------------------------------------------------*/

/*-----------------------------------------------------------------
 * NOTE: 
 *   The attribute compatibility check (methods CompatibleNode
 *   and CompatibleEdge of ARGraph) is always performed
 *   applying the method to g1, and passing the attribute of
 *   g1 as first argument, and the attribute of g2 as second
 *   argument. This may be important if the compatibility
 *   criterion is not symmetric.
 -----------------------------------------------------------------*/

/*---------------------------------------------------------
 *   IMPLEMENTATION NOTES:
 * The six vectors core_1, core_2, in_1, in_2, out_1, out_2, 
 * are shared among the instances of this class; they are
 * owned by the instance with core_len==0 (the root of the
 * SSR).
 * In the vectors in_* and out_* there is a value indicating 
 * the level at which the corresponding node became a member
 * of the core or of Tin (for in_*) or Tout (for out_*),
 * or 0 if the node does not belong to the set.
 * This information is used for backtracking.
 * The fields t1out_len etc. also count the nodes in core.
 * The true t1out_len is t1out_len-core_len!
 ---------------------------------------------------------*/

#include <stddef.h>

#include "vf2_sub_state.h"
#include "sortnodes.h"

#include "error.h"

/*----------------------------------------------------------
 * Methods of the class VF2SubState
 ---------------------------------------------------------*/
#include <algorithm>
#include <vector>
#include <cstdio>
#include <climits>
#include <cstring>
#include <iostream>

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

		for (int node = 0; node < g->NodeCount(); node++) {
			std::cout << node << " -- {";
			for (int i = 0; i < g->InEdgeCount(node); i++) {
				node_id n1 = g->GetInEdge(node, i);
				std::cout << n1 << " ";
			}
			std::cout << "}" << std::endl;
		}

		for (int i = 0; i < n; i++) {
			std::cout << order[i] << " ";
		}
		std::cout << std::endl;

		delete[] processed;
		delete[] estimateProcessed;
		return order;
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



	//std::sort(neighbors.begin(), neighbors.end(), nodeEstimateCompare);
};


/*----------------------------------------------------------
 * VF2SubState::VF2SubState(g1, g2, sortNodes)
 * Constructor. Makes an empty state.
 * If sortNodes is true, computes an initial ordering
 * for the nodes based on the frequency of their valence.
 ---------------------------------------------------------*/
VF2SubState::VF2SubState(Graph *ag1, Graph *ag2, int sortHevristic) {
	g1 = ag1;
	g2 = ag2;
	n1 = g1->NodeCount();
	n2 = g2->NodeCount();

	if (sortHevristic == 0) {
		order = NULL;
	}
	else if (sortHevristic == 1) {
		order = SortNodesByFrequency(ag1);
	}
	else if (sortHevristic == 2) {
		SearchTraverse st;
		hevristicOrder = st.SortNodesBySearchTraverse(ag1);
	}


	core_len = orig_core_len = 0;
	t1both_len = t1in_len = t1out_len = 0;
	t2both_len = t2in_len = t2out_len = 0;

	added_node1 = NULL_NODE;

	core_1 = new node_id[n1];
	core_2 = new node_id[n2];
	in_1 = new node_id[n1];
	in_2 = new node_id[n2];
	out_1 = new node_id[n1];
	out_2 = new node_id[n2];
	share_count = new long;
	if (!core_1 || !core_2 || !in_1 || !in_2 || !out_1 || !out_2 || !share_count)
		error("Out of memory");

	int i;
	for (i = 0; i < n1; i++) {
		core_1[i] = NULL_NODE;
		in_1[i] = 0;
		out_1[i] = 0;
	}
	for (i = 0; i < n2; i++) {
		core_2[i] = NULL_NODE;
		in_2[i] = 0;
		out_2[i] = 0;
	}

	*share_count = 1;

}

/*----------------------------------------------------------
 * VF2SubState::VF2SubState(state)
 * Copy constructor. 
 ---------------------------------------------------------*/
VF2SubState::VF2SubState(const VF2SubState &state) {
	g1 = state.g1;
	g2 = state.g2;
	n1 = state.n1;
	n2 = state.n2;

	order = state.order;

	core_len = orig_core_len = state.core_len;
	t1in_len = state.t1in_len;
	t1out_len = state.t1out_len;
	t1both_len = state.t1both_len;
	t2in_len = state.t2in_len;
	t2out_len = state.t2out_len;
	t2both_len = state.t2both_len;

	added_node1 = NULL_NODE;

	core_1 = state.core_1;
	core_2 = state.core_2;
	in_1 = state.in_1;
	in_2 = state.in_2;
	out_1 = state.out_1;
	out_2 = state.out_2;
	share_count = state.share_count;

	++*share_count;

	hevristicOrder = state.hevristicOrder;
}

/*---------------------------------------------------------------
 * VF2SubState::~VF2SubState()
 * Destructor.
 --------------------------------------------------------------*/
VF2SubState::~VF2SubState() {
	if (--*share_count == 0) {
		delete[] core_1;
		delete[] core_2;
		delete[] in_1;
		delete[] out_1;
		delete[] in_2;
		delete[] out_2;
		delete share_count;
		delete[] order;
		delete[] hevristicOrder;
	}
}

/*--------------------------------------------------------------------------
 * bool VF2SubState::NextPair(pn1, pn2, prev_n1, prev_n2)
 * Puts in *pn1, *pn2 the next pair of nodes to be tried.
 * prev_n1 and prev_n2 must be the last nodes, or NULL_NODE (default)
 * to start from the first pair.
 * Returns false if no more pairs are available.
 -------------------------------------------------------------------------*/
bool VF2SubState::NextPair(node_id *pn1, node_id *pn2, node_id prev_n1, node_id prev_n2) {
	if (prev_n1 == NULL_NODE)
		prev_n1 = 0;
	if (prev_n2 == NULL_NODE)
		prev_n2 = 0;
	else
		prev_n2++;

	if (t1both_len > core_len && t2both_len > core_len) {
		while (prev_n1 < n1 && (core_1[prev_n1] != NULL_NODE || out_1[prev_n1] == 0 || in_1[prev_n1] == 0)) {
			prev_n1++;
			prev_n2 = 0;
		}
	} else if (t1out_len > core_len && t2out_len > core_len) {
		while (prev_n1 < n1 && (core_1[prev_n1] != NULL_NODE || out_1[prev_n1] == 0)) {
			prev_n1++;
			prev_n2 = 0;
		}
	} else if (t1in_len > core_len && t2in_len > core_len) {
		while (prev_n1 < n1 && (core_1[prev_n1] != NULL_NODE || in_1[prev_n1] == 0)) {
			prev_n1++;
			prev_n2 = 0;
		}
	} else if (prev_n1 == 0 && order != NULL) {
		int i = 0;
		while (i < n1 && core_1[prev_n1 = order[i]] != NULL_NODE)
			i++;
		if (i == n1)
			prev_n1 = n1;
	} else {
		while (prev_n1 < n1 && core_1[prev_n1] != NULL_NODE) {
			prev_n1++;
			prev_n2 = 0;
		}
	}

	if (t1both_len > core_len && t2both_len > core_len) {
		while (prev_n2 < n2 && (core_2[prev_n2] != NULL_NODE || out_2[prev_n2] == 0 || in_2[prev_n2] == 0)) {
			prev_n2++;
		}
	} else if (t1out_len > core_len && t2out_len > core_len) {
		while (prev_n2 < n2 && (core_2[prev_n2] != NULL_NODE || out_2[prev_n2] == 0)) {
			prev_n2++;
		}
	} else if (t1in_len > core_len && t2in_len > core_len) {
		while (prev_n2 < n2 && (core_2[prev_n2] != NULL_NODE || in_2[prev_n2] == 0)) {
			prev_n2++;
		}
	} else {
		while (prev_n2 < n2 && core_2[prev_n2] != NULL_NODE) {
			prev_n2++;
		}
	}

	if (prev_n1 < n1 && prev_n2 < n2) {
		*pn1 = prev_n1;
		*pn2 = prev_n2;
		return true;
	}

	return false;
}

/*---------------------------------------------------------------
 * bool VF2SubState::IsFeasiblePair(node1, node2)
 * Returns true if (node1, node2) can be added to the state
 * NOTE: 
 *   The attribute compatibility check (methods CompatibleNode
 *   and CompatibleEdge of ARGraph) is always performed
 *   applying the method to g1, and passing the attribute of
 *   g1 as first argument, and the attribute of g2 as second
 *   argument. This may be important if the compatibility
 *   criterion is not symmetric.
 --------------------------------------------------------------*/
bool VF2SubState::IsFeasiblePair(node_id node1, node_id node2) {
	assert(node1 < n1);
	assert(node2 < n2);
	assert(core_1[node1] == NULL_NODE);
	assert(core_2[node2] == NULL_NODE);

	if (!g1->CompatibleNode(g1->GetNodeAttr(node1), g2->GetNodeAttr(node2)))
		return false;

	int i, other1, other2;
	void *attr1;
	int termout1 = 0, termout2 = 0, termin1 = 0, termin2 = 0, new1 = 0, new2 = 0;

	// Check the 'out' edges of node1
	for (i = 0; i < g1->OutEdgeCount(node1); i++) {
		other1 = g1->GetOutEdge(node1, i, &attr1);
		if (core_1[other1] != NULL_NODE) {
			other2 = core_1[other1];
			if (!g2->HasEdge(node2, other2) || !g1->CompatibleEdge(attr1, g2->GetEdgeAttr(node2, other2)))
				return false;
		} else {
			if (in_1[other1])
				termin1++;
			if (out_1[other1])
				termout1++;
			if (!in_1[other1] && !out_1[other1])
				new1++;
		}
	}

	// Check the 'in' edges of node1
	for (i = 0; i < g1->InEdgeCount(node1); i++) {
		other1 = g1->GetInEdge(node1, i, &attr1);
		if (core_1[other1] != NULL_NODE) {
			other2 = core_1[other1];
			if (!g2->HasEdge(other2, node2) || !g1->CompatibleEdge(attr1, g2->GetEdgeAttr(other2, node2)))
				return false;
		} else {
			if (in_1[other1])
				termin1++;
			if (out_1[other1])
				termout1++;
			if (!in_1[other1] && !out_1[other1])
				new1++;
		}
	}

	// Check the 'out' edges of node2
	for (i = 0; i < g2->OutEdgeCount(node2); i++) {
		other2 = g2->GetOutEdge(node2, i);
		if (core_2[other2] != NULL_NODE) {
			other1 = core_2[other2];
			if (!g1->HasEdge(node1, other1))
				return false;
		} else {
			if (in_2[other2])
				termin2++;
			if (out_2[other2])
				termout2++;
			if (!in_2[other2] && !out_2[other2])
				new2++;
		}
	}

	// Check the 'in' edges of node2
	for (i = 0; i < g2->InEdgeCount(node2); i++) {
		other2 = g2->GetInEdge(node2, i);
		if (core_2[other2] != NULL_NODE) {
			other1 = core_2[other2];
			if (!g1->HasEdge(other1, node1))
				return false;
		} else {
			if (in_2[other2])
				termin2++;
			if (out_2[other2])
				termout2++;
			if (!in_2[other2] && !out_2[other2])
				new2++;
		}
	}

	return termin1 <= termin2 && termout1 <= termout2 && new1 <= new2;
}

/*--------------------------------------------------------------
 * void VF2SubState::AddPair(node1, node2)
 * Adds a pair to the Core set of the state.
 * Precondition: the pair must be feasible
 -------------------------------------------------------------*/
void VF2SubState::AddPair(node_id node1, node_id node2) {
	assert(node1 < n1);
	assert(node2 < n2);
	assert(core_len < n1);
	assert(core_len < n2);

	core_len++;
	added_node1 = node1;

	if (!in_1[node1]) {
		in_1[node1] = core_len;
		t1in_len++;
		if (out_1[node1])
			t1both_len++;
	}
	if (!out_1[node1]) {
		out_1[node1] = core_len;
		t1out_len++;
		if (in_1[node1])
			t1both_len++;
	}

	if (!in_2[node2]) {
		in_2[node2] = core_len;
		t2in_len++;
		if (out_2[node2])
			t2both_len++;
	}
	if (!out_2[node2]) {
		out_2[node2] = core_len;
		t2out_len++;
		if (in_2[node2])
			t2both_len++;
	}

	core_1[node1] = node2;
	core_2[node2] = node1;

	int i, other;
	for (i = 0; i < g1->InEdgeCount(node1); i++) {
		other = g1->GetInEdge(node1, i);
		if (!in_1[other]) {
			in_1[other] = core_len;
			t1in_len++;
			if (out_1[other])
				t1both_len++;
		}
	}

	for (i = 0; i < g1->OutEdgeCount(node1); i++) {
		other = g1->GetOutEdge(node1, i);
		if (!out_1[other]) {
			out_1[other] = core_len;
			t1out_len++;
			if (in_1[other])
				t1both_len++;
		}
	}

	for (i = 0; i < g2->InEdgeCount(node2); i++) {
		other = g2->GetInEdge(node2, i);
		if (!in_2[other]) {
			in_2[other] = core_len;
			t2in_len++;
			if (out_2[other])
				t2both_len++;
		}
	}

	for (i = 0; i < g2->OutEdgeCount(node2); i++) {
		other = g2->GetOutEdge(node2, i);
		if (!out_2[other]) {
			out_2[other] = core_len;
			t2out_len++;
			if (in_2[other])
				t2both_len++;
		}
	}

}

/*--------------------------------------------------------------
 * void VF2SubState::GetCoreSet(c1, c2)
 * Reads the core set of the state into the arrays c1 and c2.
 * The i-th pair of the mapping is (c1[i], c2[i])
 --------------------------------------------------------------*/
void VF2SubState::GetCoreSet(node_id c1[], node_id c2[]) {
	int i, j;
	for (i = 0, j = 0; i < n1; i++)
		if (core_1[i] != NULL_NODE) {
			c1[j] = i;
			c2[j] = core_1[i];
			j++;
		}
}

/*----------------------------------------------------------------
 * Clones a VF2SubState, allocating with new the clone.
 --------------------------------------------------------------*/
State* VF2SubState::Clone() {
	return new VF2SubState(*this);
}

/*----------------------------------------------------------------
 * Undoes the changes to the shared vectors made by the 
 * current state. Assumes that at most one AddPair has been
 * performed.
 ----------------------------------------------------------------*/
void VF2SubState::BackTrack() {
	assert(core_len - orig_core_len <= 1);
	assert(added_node1 != NULL_NODE);

	if (orig_core_len < core_len) {
		int i, node2;

		if (in_1[added_node1] == core_len)
			in_1[added_node1] = 0;
		for (i = 0; i < g1->InEdgeCount(added_node1); i++) {
			int other = g1->GetInEdge(added_node1, i);
			if (in_1[other] == core_len)
				in_1[other] = 0;
		}

		if (out_1[added_node1] == core_len)
			out_1[added_node1] = 0;
		for (i = 0; i < g1->OutEdgeCount(added_node1); i++) {
			int other = g1->GetOutEdge(added_node1, i);
			if (out_1[other] == core_len)
				out_1[other] = 0;
		}

		node2 = core_1[added_node1];

		if (in_2[node2] == core_len)
			in_2[node2] = 0;
		for (i = 0; i < g2->InEdgeCount(node2); i++) {
			int other = g2->GetInEdge(node2, i);
			if (in_2[other] == core_len)
				in_2[other] = 0;
		}

		if (out_2[node2] == core_len)
			out_2[node2] = 0;
		for (i = 0; i < g2->OutEdgeCount(node2); i++) {
			int other = g2->GetOutEdge(node2, i);
			if (out_2[other] == core_len)
				out_2[other] = 0;
		}

		core_1[added_node1] = NULL_NODE;
		core_2[node2] = NULL_NODE;

		core_len = orig_core_len;
		added_node1 = NULL_NODE;
	}

}

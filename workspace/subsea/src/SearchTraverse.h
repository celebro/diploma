//This file include all the functions relatet to part 5 Search Travers - in the paper.
//The main goal of this functions is to find all the all the isomorphism subgraph drived from 
//all the cross edgs ( all the edges crossing the cut (C1,C2)).

#ifndef _SEARCHTRAVERS_H_
#define _SEARCHTRAVERS_H_


#include "Graph.h"
#include "Node.h"
#include "IsoSubGraph.h"

class CTraverseHistList;
typedef pair<vector<vector<int>*>*, vector<CNode*>*> TravePair;
///////////////////////////////////////////////////////////////////////////////
class CSearchTraverse
{
public:
	CSearchTraverse(bool bRetSubGraph = false); // should be false
	~CSearchTraverse();

	bool Init(CGraph* pLGraph);
	int SearchTraverse(Edge* pCrossEdge,TravePair* pTravHistPair);
	void Reset();
	vector<CIsoSubGraph*>* GetIsoSubGraph();//MIS

private:
	//members
	CGraph* m_pG;
	Edge* m_pCrossEdge;
	TravePair* m_pTravHistPair;
	vector<CNode*>* m_pvecNumberdNodes;
	bool m_bRetSubGraph;//return the sub graph that found by the Search

	//functions
	int SearchVisit(int vtime);
	bool IsContained(vector<int>* pVec1,vector<int>* pVec2);
	void VecIntersect(vector<CNode*>* pvecC1,vector<CNode*>*  pvecC2); 

	//Build the IsoSubGraphs for the MIS
	void BuildIsoSubGraph();

    //Print IsoSubGraph for the MIS
	void PrintIsoSubGraph();
    void PrintSubGraphTraversal();
    void PrintSmallGraphTraversal();

	//MIS
	vector<CIsoSubGraph*>* m_pvecMISIsoGraphs;
	

};


#endif //_SEARCHTRAVERS_H_


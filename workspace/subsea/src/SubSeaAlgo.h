#ifndef _SUBSEAALGO_H_
#define _SUBSEAALGO_H_


#include <vector>
#include "Graph.h"
#include "SearchTraverse.h"
#include "TraverseHistList.h"
#include "MISFileCreator.h"

//#include "Node.h"

class CSubSeaAlgo
{
public:

	CSubSeaAlgo();
	~CSubSeaAlgo();
	
	bool Init(string strSmallGraphFilename, string strLargeGraphFilename = "");
	pair<int,int> Run();
	int RunBruteForce(); //compete the subsea algo with brute force

	int GetNumOfNodes(bool bSmallGraph)
	{
		if (bSmallGraph)
			return m_pGsmall->GetNumOfNodes();
		else
			return m_pGlarge->GetNumOfNodes();
	}

	int GetNumOfEdges(bool bSmallGraph)
	{
		if (bSmallGraph)
			return m_pGsmall->GetNumOfEdges();
		else
			return m_pGlarge->GetNumOfEdges();
	}


private:
	//members
	CGraph* m_pGlarge; //The Large Graph
	CGraph* m_pGsmall; //Small patern Graph
	ITraverseHistory* m_pAllTravList; // Travers History Object . Original Version.
	vector<TravePair>* m_pvecTravHist;//The vector of all the Traverses
	CSearchTraverse  m_SearchTraverse;//Search Object
	vector<int>* m_pvecTravMultipaleCout;//Travers History  Multipale cout value.
	
	//functions
	int ProduceSubIso(CSubGraph& L_Graph);
	void GetCrossEdges(CSubGraph* pLeftCut, CSubGraph* pRightCut, vector<Edge*>* pvecCrossEdges);

	//MIS
	CMISFileCreator m_MISFC;

};




#endif //_SUBSEAALGO_H_
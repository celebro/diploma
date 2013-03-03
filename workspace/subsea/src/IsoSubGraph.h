#ifndef _ISOSUBGRAPH_H_
#define _ISOSUBGRAPH_H_

#include "Node.h"
#include <vector>

class CIsoSubGraph
{
public:
	CIsoSubGraph();
	~CIsoSubGraph();
	void AddEdge(CNode* V1, CNode* V2);
	bool IsIntersect(CIsoSubGraph* G);
	bool operator==(const CIsoSubGraph& other); 
	void Print();

private:
	vector<LightEdge*>* m_pvecLightEdge;
};

#endif //_ISOSUBGRAPH_H_

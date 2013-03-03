
#ifndef _GRAPH_H_
#define _GRAPH_H_


#include "Node.h"
#include <vector>

#ifdef WIN32
	#include <hash_map>
#else
	#include <ext/hash_map>
#endif

using namespace std;

#ifdef WIN32
	using namespace stdext;
#else
	using namespace __gnu_cxx;
#endif

typedef pair<int, CNode*> HashPair;

///////////////////////////////////////////////////////////////////////////////
enum EDGE_TYPE
{
	EDGE_TYPE_BLACK,
	EDGE_TYPE_WHITE,
	EDGE_TYPE_NO_EDGE
};

///////////////////////////////////////////////////////////////////////////////
struct TextList
{
	char *str;
	struct TextList *next;
};

///////////////////////////////////////////////////////////////////////////////
class CGraph
{
private:
	vector<CNode*> m_vecNodes;
	hash_map</*key*/ int, /*Data*/ CNode*> m_mapNodes;

	bool AddToGraph(TextList* T);
	string CutCarReturn(string str);

	// Text parsing
	void add_to_list(TextList **l, char *element);
	TextList* Parse(char *str, char *delimiters, char *forbidden);

public:

	~CGraph();

	// Init the graph from file.
	bool InitFromFile(const string& strInitFilename);

    unsigned short int read_word(FILE *in, bool *end);
    bool ReadWholeGraph(FILE *in);

	// Given a node, get its adjacent nodes.
	bool GetAdjNodes(int nNodeId, vector<AdjNode*>& rAdjNodes);

	// Set all traverse value to zero.
	bool ResetTravNodes();
	
	int GetNumOfNodes();
	bool RemoveEdge(Edge* pEdge);
	vector<CNode*>* GetAllNodes();
	void PrintNodes();
	void ResetBFSColor();
	int GetNumOfEdges();

	EDGE_TYPE GetEdgeType(CNode* pV1, CNode* pV2);
	void Reset();
};

///////////////////////////////////////////////////////////////////////////////
struct CSubGraph
{
	vector<CNode*>* m_pvecNodes;

	CSubGraph()
	{
		m_pvecNodes = NULL;
	}

	bool Init(vector<CNode*>* pvecNodes)
	{
		m_pvecNodes = new vector<CNode*>(pvecNodes->begin(), pvecNodes->end());
		return true;
	}

	~CSubGraph()
	{
		if (m_pvecNodes)
			delete m_pvecNodes;
	}

	vector<CNode*>* GetAllNodes()
	{
		vector<CNode*>* pvecRetVal = new vector<CNode*>(*m_pvecNodes);
		return pvecRetVal;
	}

	int GetNumOfNodes() const
	{
		return (int)m_pvecNodes->size();
	}

};

#endif // _GRAPH_H_

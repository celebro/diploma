
#ifndef _NODE_H_
#define _NODE_H_


#define BFS_COLOR_BLACK 2
#define BFS_COLOR_GRAY  1
#define BFS_COLOR_WHITE 0

#include <string>
#include <vector>
using namespace std;

class CNode;

///////////////////////////////////////////////////////////////////////////////
struct AdjNode
{
	CNode* pNode;
	string strEdgeName;
};

///////////////////////////////////////////////////////////////////////////////
class CNode
{
public:

	int m_nId;
	int m_nTravNum; // its the "d" in the article , can also be "g".
	string m_strName;

	vector<AdjNode*> m_vecAdj; // list of adjecent nodes
	vector<AdjNode*> m_vecBlackAdj; // list of adjecent nodes

public:
	~CNode();
	CNode(const string& strName, int nID);
	bool IsConnected(CNode* pNode);
	bool IsConnected(AdjNode* pAdjNode);
	bool operator== (const CNode& obj);
    bool operator> (const CNode& obj);
    bool operator< (const CNode& obj);
	void AddAdjNode(CNode* pNode, string name);
	void AddAdjNode(AdjNode* pAdj);
	int GetNumOfAdjNodes();
	int GetID() const;
    void Print();
	
	char m_BfsColor;

	vector<AdjNode*> GetAdjNodes();
	vector<AdjNode*>& GetAdjNodesRef();
	vector<CNode*>* GetAdjNodesOnlyNodes() const;
	vector<CNode*>* GetAllAdjNodesOnlyNodes() const;
    vector<int>* getAdjHistoriesWhite() const;
    vector<int>* getAdjHistoriesBlackAndWhite() const;

	bool RemoveAdjNode(CNode* pNode);
	void Reset();
};

///////////////////////////////////////////////////////////////////////////////
struct Edge
{
	CNode* v1;
	CNode* v2;
	string strName;
	
	Edge(CNode* _v1, CNode* _v2, string name)
	{
		v1 = _v1;
		v2 = _v2;
		strName = name;
	}

	bool operator< (const Edge& e)
	{
		if(v1->GetID() < e.v1->GetID())
			return true;
		if((v1->GetID() == e.v1->GetID()) && (v2->GetID() < e.v2->GetID()))
			return true;

		return false;
	}
};

///////////////////////////////////////////////////////////////////////////////
struct LightEdge
{
	CNode* v1;
	CNode* v2;
		
	LightEdge(CNode* _v1, CNode* _v2)
	{
		v1 = _v1;
		v2 = _v2;
	}

	bool operator== (const LightEdge& e)
	{
		if((v1->GetID() == e.v1->GetID())&& (v2->GetID() == e.v2->GetID()))
			return true;

		if((v1->GetID() == e.v2->GetID())&& (v2->GetID() == e.v1->GetID()))
			return true;

		return false;
	}
};

#endif // _NODE_H_

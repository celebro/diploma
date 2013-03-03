
#ifndef _BLACK_HOLE_TRAV_LIST_
#define _BLACK_HOLE_TRAV_LIST_

class CGraph;
class CNode;


#include <vector>
using namespace std;
typedef pair<vector<vector<int>*>*, vector<CNode*>*> TravePair;
#include "SearchTraverse.h"


///////////////////////////////////////////////////////////////////////////////
class ITraverseHistory
{
public:
	ITraverseHistory(){}; 
	virtual ~ITraverseHistory(){};

	virtual bool ProduceTravHistList(CGraph* pG) = 0;
    virtual bool ProduceTravHistListFast(CGraph* pG) = 0;
	virtual vector<TravePair>& ToVector() = 0;
	virtual vector<int>& GetMultiapeVec() = 0; 
};

///////////////////////////////////////////////////////////////////////////////
class CBlackHoleTraverseList : public ITraverseHistory
{
public:
	CBlackHoleTraverseList();
	virtual ~CBlackHoleTraverseList();

	virtual bool ProduceTravHistList(CGraph* pG);
    virtual bool ProduceTravHistListFast(CGraph* pG);
	virtual vector<TravePair>& ToVector();
	virtual vector<int>& GetMultiapeVec(){return *m_pvecTravMultiapl;}
	

private:
	CSearchTraverse m_SearchTraverse;
	vector<TravePair>* m_pvecTrav;
	vector<int>* m_pvecTravMultiapl;
	CGraph* m_pG;
	int m_nCurrNum;
	int m_nNumOfActivationsPerTrav;

	void CopyTravers(TravePair& Tsource, TravePair& Tdest);
	TravePair ProduceTravListOnPair(CNode* v1, CNode* v2);
	bool AddToHoleAndVisit(vector<CNode*>* pvecUnassignedNodes, vector<CNode*>* pvecAssignedNodes, vector<vector<int>*>* pvecNumbering);
	vector<CNode*>* GetAllAdjNodes(vector<CNode*>& vecNodeToSplit, vector<CNode*>* pvecCurrCut);
	bool IsRedundant(const TravePair& HistList);
	bool IsRedundant_Version2(TravePair& HistList);
	void PrintDebug(vector<vector<int>*>* pvecNumbering);
	int CalcIntegrity(vector<vector<int>*>* pvecNumbering);
	void PrintTrav();//debug function
	void PrintTrav(const vector<TravePair>& T);
	void PrintDebug(vector<CNode*>* vecNodes);
	void DeleteTravPairContent(TravePair& T);
};

///////////////////////////////////////////////////////////////////////////////
class CTraverseHistList : public ITraverseHistory
{
public:
	CTraverseHistList();
	virtual ~CTraverseHistList();

	virtual bool ProduceTravHistList(CGraph* pG);
    virtual bool ProduceTravHistListFast(CGraph* pG);
	virtual vector<TravePair>& ToVector();
	virtual vector<int>& GetMultiapeVec() {return *m_pvecTravMultiapl;}

private:
	vector<TravePair>* m_pvecTrav;
	vector<int>* m_pvecTravMultiapl;
	CGraph* m_pG;
	int m_nCurrNum;
	CSearchTraverse m_SearchTraverse;

	bool IsRedundant(const TravePair& HistList);
	bool IsRedundant_Version2(TravePair& HistList);
	TravePair ProduceTravListOnPair(CNode* v1, CNode* v2);
	CNode* EstimateNext(CNode* V, vector<CNode*>* pvecAdjNodesToVNotEval);
	pair<int,int> EstimateNext(CNode* v1, CNode* v2);
	void Visit(CNode* V,vector<CNode*>* pvecUnassignedNodes,
						vector<CNode*>* pvecAssignedNodes,
						vector<vector<int>*>* pvecNumbering,
						bool FirstTime , CNode* v2);
	void PrintTrav();//debug function
};
#endif // _BLACK_HOLE_TRAV_LIST_

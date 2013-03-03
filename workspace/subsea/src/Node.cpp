#include "Node.h"
#include <iostream>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
CNode::CNode(const string& strName, int nID)
{
	m_nId = nID;
	m_nTravNum = 0;
	m_strName = strName;
}

///////////////////////////////////////////////////////////////////////////////
CNode::~CNode()
{
	vector<AdjNode*>::iterator iter_adj;
	for (iter_adj = m_vecAdj.begin(); iter_adj != m_vecAdj.end(); iter_adj++)
		if (*iter_adj)
			delete *iter_adj;

	for (iter_adj = m_vecBlackAdj.begin(); iter_adj != m_vecBlackAdj.end(); iter_adj++)
		if (*iter_adj)
			delete *iter_adj;
}
///////////////////////////////////////////////////////////////////////////////
void CNode::Print()
{
    cout<<"Node ("<<m_nId<<",t="<<m_nTravNum<<", l="<<m_strName<<")"<<endl;
    cout<<" adjacent to";
    for (vector<AdjNode*>::iterator iter = m_vecAdj.begin(); iter != m_vecAdj.end(); iter++)
        cout<<" ("<<(**iter).pNode->m_nId<<",t="<<(**iter).pNode->m_nTravNum<<")";
    cout<<endl;
    cout<<" clean trav ";
    for (vector<AdjNode*>::iterator iter = m_vecAdj.begin(); iter != m_vecAdj.end(); iter++)
        if ((**iter).pNode->m_nTravNum < m_nTravNum && (**iter).pNode->m_nTravNum>0)
           cout<<" ("<<(**iter).pNode->m_nId<<",t="<<(**iter).pNode->m_nTravNum<<")"; 
    cout<<endl;
}

///////////////////////////////////////////////////////////////////////////////
bool CNode::IsConnected(CNode* pNode)
{
	for (vector<AdjNode*>::iterator iter = m_vecAdj.begin(); iter != m_vecAdj.end(); iter++)
		if ((**iter).pNode->GetID() == pNode->GetID())
			return true;

	return false;
}

///////////////////////////////////////////////////////////////////////////////
bool CNode::IsConnected(AdjNode* pAdjNode)
{
	for (vector<AdjNode*>::iterator iter = m_vecAdj.begin(); iter != m_vecAdj.end(); iter++)
		if ((**iter).pNode == pAdjNode->pNode)
			return true;

	return false;
}

///////////////////////////////////////////////////////////////////////////////
bool CNode::operator== (const CNode& obj)
{
	return (GetID() == obj.GetID());
}

///////////////////////////////////////////////////////////////////////////////
bool CNode::operator< (const CNode& obj)
{
	return (GetID() < obj.GetID());
}

///////////////////////////////////////////////////////////////////////////////
bool CNode::operator> (const CNode& obj)
{
	return (GetID() > obj.GetID());
}

///////////////////////////////////////////////////////////////////////////////
vector<AdjNode*> CNode::GetAdjNodes()
{
	return m_vecAdj;
}

///////////////////////////////////////////////////////////////////////////////
vector<AdjNode*>& CNode::GetAdjNodesRef()
{
	return m_vecAdj;
}

///////////////////////////////////////////////////////////////////////////////
void CNode::AddAdjNode(CNode* pNode, string name)
{
	AdjNode* pAdj = new AdjNode();
	pAdj->pNode = pNode;
	pAdj->strEdgeName = name;

	m_vecAdj.push_back(pAdj);
}

///////////////////////////////////////////////////////////////////////////////
void CNode::AddAdjNode(AdjNode* pAdj)
{
	m_vecAdj.push_back(pAdj);
}

///////////////////////////////////////////////////////////////////////////////
int CNode::GetNumOfAdjNodes()
{
	return (int)m_vecAdj.size();
}


///////////////////////////////////////////////////////////////////////////////
//Added by lior 27.8.06 12:00
vector<CNode*>* CNode::GetAdjNodesOnlyNodes() const
{
	vector<CNode*>* pvecNodes = new vector<CNode*>();
	for(vector<AdjNode*>::const_iterator Iter = m_vecAdj.begin(); Iter != m_vecAdj.end(); Iter++)
	{
		pvecNodes ->push_back((*Iter)->pNode);
	}

	return pvecNodes;
}

///////////////////////////////////////////////////////////////////////////////
// Natalia: get adj nodes with non-zero trav. history
vector<int>* CNode::getAdjHistoriesWhite() const
{
	vector<int>* pvecNodes = new vector<int>();
	for(vector<AdjNode*>::const_iterator Iter = m_vecAdj.begin(); Iter != m_vecAdj.end(); Iter++)
	{
        if ((*Iter)->pNode->m_nTravNum > 0)      
		    pvecNodes ->push_back((*Iter)->pNode->m_nTravNum);
	}
    sort(pvecNodes->begin(),pvecNodes->end());

	return pvecNodes;
}

///////////////////////////////////////////////////////////////////////////////
vector<CNode*>* CNode::GetAllAdjNodesOnlyNodes() const
{
	vector<AdjNode*>::const_iterator iter;
	vector<CNode*>* pvecNodes = new vector<CNode*>();

	for (iter = m_vecAdj.begin(); iter != m_vecAdj.end(); iter++)
		pvecNodes->push_back((*iter)->pNode);

	for (iter = m_vecBlackAdj.begin(); iter != m_vecBlackAdj.end(); iter++)
		pvecNodes->push_back((*iter)->pNode);

	return pvecNodes;
}

///////////////////////////////////////////////////////////////////////////////
// Natalia: get adj nodes with non-zero trav. history
vector<int>* CNode::getAdjHistoriesBlackAndWhite() const
{
	vector<AdjNode*>::const_iterator iter;
	vector<int>* pvecNodes = new vector<int>();

	for (iter = m_vecAdj.begin(); iter != m_vecAdj.end(); iter++)
       if ((*iter)->pNode->m_nTravNum > 0) 
		pvecNodes->push_back((*iter)->pNode->m_nTravNum);

	for (iter = m_vecBlackAdj.begin(); iter != m_vecBlackAdj.end(); iter++)
       if ((*iter)->pNode->m_nTravNum > 0) 
		pvecNodes->push_back((*iter)->pNode->m_nTravNum);

    sort(pvecNodes->begin(),pvecNodes->end());

	return pvecNodes;
}

///////////////////////////////////////////////////////////////////////////////
int CNode::GetID() const
{
	return m_nId;
}

///////////////////////////////////////////////////////////////////////////////
bool CNode::RemoveAdjNode(CNode* pNode)
{
	bool bRetVal = false;
	string strName;

	for (vector<AdjNode*>::iterator iter = m_vecAdj.begin(); iter != m_vecAdj.end(); iter++)
		if (*(*iter)->pNode == *pNode)
		{
			strName = (*iter)->strEdgeName;

			delete *iter;
			m_vecAdj.erase(iter);
			
			bRetVal = true;
			break;
		}

	if (bRetVal)
	{
		AdjNode* pAdj = new AdjNode();
		pAdj->pNode = pNode;
		pAdj->strEdgeName = strName;

		m_vecBlackAdj.push_back(pAdj);
	}

	return bRetVal;
}

///////////////////////////////////////////////////////////////////////////////
void CNode::Reset()
{
	m_vecAdj.insert(m_vecAdj.end(), m_vecBlackAdj.begin(), m_vecBlackAdj.end());
	m_vecBlackAdj.clear();
}

#include "Graph.h"
#include "Node.h"
#include "TraverseHistList.h"
#include "BiSection.h"
#include "Def.h"
#include <iostream>
#include <algorithm>
using namespace std;


///////////////////////////////////////////////////////////////////////////////
CBlackHoleTraverseList::CBlackHoleTraverseList()
{
	m_nNumOfActivationsPerTrav = NUM_OF_ACTIVATIONS_FOR_SINGLE_BLACK_HOLE;
	m_pvecTrav = new vector<TravePair>();
	m_pvecTravMultiapl  = new vector<int>();
}

///////////////////////////////////////////////////////////////////////////////
CBlackHoleTraverseList::~CBlackHoleTraverseList()
{
	if (m_pvecTrav)
		for(vector<TravePair>::iterator IterTrav = m_pvecTrav->begin();IterTrav != m_pvecTrav->end(); IterTrav++)
		{
			if((*IterTrav).second)
				delete (*IterTrav).second;
			for(vector<vector<int>*>::iterator IterFirst = (*IterTrav).first->begin();IterFirst != (*IterTrav).first->end();IterFirst++)
			{
				if(*IterFirst)
					delete (*IterFirst);
			}

			if((*IterTrav).first)
				delete (*IterTrav).first;
		}

	if(m_pvecTravMultiapl)//The vector of "int" that represent the Multipale value of travers
		delete m_pvecTravMultiapl;
}

///////////////////////////////////////////////////////////////////////////////
bool CBlackHoleTraverseList::ProduceTravHistListFast(CGraph* pG) { return ProduceTravHistList(pG); }

///////////////////////////////////////////////////////////////////////////////
bool CBlackHoleTraverseList::ProduceTravHistList(CGraph* pG)
{
	int nCurrIntegVal = 0;
	int nLargestIntegVal = -1;
	m_pG = pG;
	m_SearchTraverse.Init(m_pG);
	TravePair HistList, BestHistList;
	Edge* pE;
	
	HistList.first = NULL;//added by lior
	HistList.second = NULL;//added by lior
	BestHistList.first= NULL;//added by lior
	BestHistList.second = NULL;//added by lior

	vector<CNode*>* pvecNodes = m_pG->GetAllNodes();

	for (vector<CNode*>::iterator iter1 = pvecNodes->begin(); iter1 != pvecNodes->end(); iter1++)
		for (vector<CNode*>::iterator iter2 = pvecNodes->begin(); iter2 != pvecNodes->end(); iter2++)
		{
			HistList.first = NULL;//added by lior
			HistList.second = NULL;//added by lior
			BestHistList.first= NULL;//added by lior
			BestHistList.second = NULL;//added by lior
			if ((*iter1)->IsConnected(*iter2))
			{
				nLargestIntegVal = -1;
				for (int i=0; i<m_nNumOfActivationsPerTrav; i++)
				{
					DeleteTravPairContent(HistList);//added by lior
					HistList = ProduceTravListOnPair(*iter1, *iter2);
					nCurrIntegVal = CalcIntegrity(HistList.first);

					if (nCurrIntegVal > nLargestIntegVal)
					{
						nLargestIntegVal = nCurrIntegVal;
						DeleteTravPairContent(BestHistList);//added by lior
						//BestHistList = HistList;
						CopyTravers(HistList,BestHistList);//added by lior

						
					}
				}

				//if (!IsRedundant(BestHistList))
				if (!IsRedundant_Version2(BestHistList))//added by lior version 2
				{
					m_pvecTrav->push_back(BestHistList);
					//Lior Change 17.9.06 , sovle the multipale count of subgraphs
					pE = new Edge(*iter1,*iter2,"YYY");
					m_pvecTravMultiapl->push_back(m_SearchTraverse.SearchTraverse(pE, &HistList));
					
					if(pE)
						delete pE;
					pE = NULL;
				}

				else
					DeleteTravPairContent(BestHistList);//added by lior
				
			}
		}

	#ifdef _DEBUG_MODE_
		std::cout << "DEBUG_MODE BEGIN TRAVERS HISTORY BLACKHOLE" << endl;
		PrintTrav();
		std::cout << "DEBUG_MODE END TRAVERS HISTORY BLACKHOLE" << endl << endl;
	#endif

	delete pvecNodes;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
vector<TravePair>& CBlackHoleTraverseList::ToVector()
{
	return *m_pvecTrav;
}

///////////////////////////////////////////////////////////////////////////////
int CBlackHoleTraverseList::CalcIntegrity(vector<vector<int>*>* pvecNumbering)
{
	int RetVal = 0;
	int nIdx = 1;
	int nNumOfNodes = (int)pvecNumbering->size();

	for (vector<vector<int>*>::iterator iter1 = pvecNumbering->begin(); iter1 != pvecNumbering->end(); iter1++)
	{
		RetVal += (nNumOfNodes + 1 - nIdx)*((int)(*iter1)->size());
		nIdx++;
	}

	return RetVal;
}

///////////////////////////////////////////////////////////////////////////////
TravePair CBlackHoleTraverseList::ProduceTravListOnPair(CNode* v1, CNode* v2)
{
	//
	// AKA: Hole integration
	//

	vector<CNode*>* pvecUnassignedNodes = m_pG->GetAllNodes();		// V
	vector<CNode*>* pvecAssignedNodes = new vector<CNode*>();		// B
	vector<vector<int>*>* pvecNumbering = new vector<vector<int>*>();

	m_pG->ResetTravNodes();
	m_nCurrNum = 0;
	v1->m_nTravNum = ++m_nCurrNum;
	v2->m_nTravNum = ++m_nCurrNum;

	for (vector<CNode*>::iterator un_iter = pvecUnassignedNodes->begin();un_iter != pvecUnassignedNodes->end(); un_iter++)
		if(*(*un_iter)== *v1)
		{
			pvecUnassignedNodes->erase(un_iter);
			break;
		}

	for (vector<CNode*>::iterator un_iter = pvecUnassignedNodes->begin();un_iter != pvecUnassignedNodes->end(); un_iter++)
		if(*(*un_iter)== *v2)
		{
			pvecUnassignedNodes->erase(un_iter);
			break;
		}

	pvecAssignedNodes->push_back(v1);
	pvecNumbering->push_back(new vector<int>());

	pvecAssignedNodes->push_back(v2);
	vector<int>* pTempVec = new vector<int>();
	pTempVec->push_back(1);
	pvecNumbering->push_back(pTempVec);

	while (!pvecUnassignedNodes->empty())
		AddToHoleAndVisit(pvecUnassignedNodes, pvecAssignedNodes, pvecNumbering);

	if(pvecUnassignedNodes)
		delete pvecUnassignedNodes;
	return TravePair(pvecNumbering, pvecAssignedNodes);
}

///////////////////////////////////////////////////////////////////////////////
bool CBlackHoleTraverseList::AddToHoleAndVisit(vector<CNode*>* pvecUnassignedNodes, 
											   vector<CNode*>* pvecAssignedNodes,
											   vector<vector<int>*>* pvecNumbering)
{
	vector<CNode*>* vecCrossEdges = GetAllAdjNodes(*pvecUnassignedNodes, pvecAssignedNodes);
	
	CNode*& N = vecCrossEdges->at(RandNum((int)vecCrossEdges->size()-1));
	N->m_nTravNum = ++m_nCurrNum;

	pvecAssignedNodes->push_back(N);
	for (vector<CNode*>::iterator un_iter = pvecUnassignedNodes->begin(); 
		un_iter != pvecUnassignedNodes->end(); un_iter++)
	{
		if ((*un_iter)->m_strName == N->m_strName)
		{
			pvecUnassignedNodes->erase(un_iter);
			break;
		}
	}

	vector<int>* pvecNums = new vector<int>();
	vector<AdjNode*> vecAdjNodes = N->GetAdjNodes();
	for (vector<AdjNode*>::iterator iter = vecAdjNodes.begin(); iter != vecAdjNodes.end(); iter++)
	{
		if ((*iter)->pNode->m_nTravNum != 0)
			if (N->m_nTravNum > (*iter)->pNode->m_nTravNum)
				pvecNums->push_back((*iter)->pNode->m_nTravNum);
	}

	sort(pvecNums->begin(), pvecNums->end());
	pvecNumbering->push_back(pvecNums);

	delete vecCrossEdges;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
//Verify that this Travers History is not Redundant i.e isomorphic to other Travers
bool CBlackHoleTraverseList::IsRedundant(const TravePair& HistList)
{
	int i;
	bool bStop = true;
	if(m_pvecTrav->size() == 0)
		return false;

	for(vector<TravePair>::iterator Iter = m_pvecTrav->begin();Iter != m_pvecTrav->end(); Iter++)
	{
		bStop = true;
		if((*Iter).first->size() != HistList.first->size())
			return false;//should not happen
		
		for(i=0; i < (int)HistList.first->size() ; i++)
		{
			if( (!(*(*HistList.first)[i] == *(*(*Iter).first)[i])) && bStop)
				bStop = false;
		}

		if(bStop)
			return true;
	}
	return false;
}
///////////////////////////////////////////////////////////////////////////////
//Build a vector(vecAdjNodes) of all the nodes in vecNodeToSplit that are adj to vecNodeToSplit
//It might contain duplicated Nodes, acording to the number of Edges entering to the node.
vector<CNode*>* CBlackHoleTraverseList::GetAllAdjNodes(vector<CNode*>& vecNodeToSplit, vector<CNode*>* pvecCurrCut)
{
	vector<CNode*>*  vecAdjNodes  = new vector<CNode*>();
	vector<CNode*>::iterator Iter1,Iter2;
	
	if(pvecCurrCut->size() == 0)
        return vecAdjNodes;
	
	
	for (Iter1 = pvecCurrCut->begin(); Iter1 != pvecCurrCut->end(); Iter1++ )
		for (Iter2 = vecNodeToSplit.begin(); Iter2 != vecNodeToSplit.end(); Iter2++ )
			if((*Iter1)->IsConnected(*Iter2))
				vecAdjNodes->push_back(*Iter2);

	return vecAdjNodes;
}

///////////////////////////////////////////////////////////////////////////////
void CBlackHoleTraverseList::PrintTrav(const vector<TravePair>& T)
{
	for(vector<TravePair>::const_iterator Iter = T.begin();Iter != T.end(); Iter++)
		for(vector<vector<int>*>::iterator Iter2 = (*Iter).first->begin();Iter2 != (*Iter).first->end(); Iter2++)
		{
			for(vector<int>::iterator Iter3 = (*Iter2)->begin();Iter3 != (*Iter2)->end(); Iter3++)
				std::cout << " " << (*Iter3);
			std::cout << endl;   
		}

}

///////////////////////////////////////////////////////////////////////////////
void CBlackHoleTraverseList::PrintDebug(vector<CNode*>* vecNodes)
{
	for (vector<CNode*>::iterator iter = vecNodes->begin(); iter != vecNodes->end(); iter++)
		printf("%s ", (*iter)->m_strName.c_str());

	printf("\n");
}

///////////////////////////////////////////////////////////////////////////////
void CBlackHoleTraverseList::PrintDebug(vector<vector<int>*>* pvecNumbering)
{
	for (vector<vector<int>*>::iterator iter1 = pvecNumbering->begin(); iter1 != pvecNumbering->end(); iter1++)
	{
		for (vector<int>::iterator iter2 = (*iter1)->begin(); iter2 != (*iter1)->end(); iter2++)
		{
			printf("%d ", *iter2);
		}
		printf("\n");
	}
}
///////////////////////////////////////////////////////////////////////////////
//DEBUG function 
//Added by Lior 28.7.06 
void CBlackHoleTraverseList::PrintTrav()
{
	static int i = 0;
	for(vector<TravePair>::const_iterator Iter = m_pvecTrav->begin();Iter != m_pvecTrav->end(); Iter++)
	{
		std::cout << endl;
		std::cout << "Travers Number: " << i++ <<endl;
		std::cout << "vertex Order : ";
		for(vector<CNode*>::iterator Iter4 = (*Iter).second->begin();Iter4 != (*Iter).second->end(); Iter4++)
			std::cout<< "(" <<  (*Iter4)->m_nId <<", "<<(*Iter4)->m_strName<< ") ";

		std::cout << endl;
		for(vector<vector<int>*>::iterator Iter2 = (*Iter).first->begin();Iter2 != (*Iter).first->end(); Iter2++)
		{
			std::cout<< "{";
			for(vector<int>::iterator Iter3 = (*Iter2)->begin();Iter3 != (*Iter2)->end(); Iter3++)
				std::cout << (*Iter3)<< " ";
			std::cout<< "}";
			std::cout << endl;   
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//Added by Lior 30.8.06 12:48
//Free the memory of TravPair ,dose not delete the Travers Pair only its content.
void CBlackHoleTraverseList::DeleteTravPairContent(TravePair& T)
{

	//delete the travers
	if(T.second)
		delete T.second;
	T.second = NULL;

	
	if(T.first)
	{
		for(vector<vector<int>*>::iterator IterFirst = T.first->begin();IterFirst != T.first->end();IterFirst++)
			if(*IterFirst)
			{
				delete (*IterFirst);
				*IterFirst = NULL;

			}

		delete T.first;
		T.first = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
//Verify that this Travers History is not Redundant i.e isomorphic to other Travers
//version 2 , Add by lior 6.9.06 
//work same as the paper, using SearchTravers 
bool CBlackHoleTraverseList::IsRedundant_Version2(TravePair& HistList)
{
	
	int counter = 0;

	CNode*& V1 = HistList.second->at(0);
	CNode*& V2 = HistList.second->at(1);
	Edge* pE = new Edge(V1,V2,"XXX");
	Edge* pEtemp; 



	vector<CNode*>* pvecAllNodes = m_pG->GetAllNodes(); //delete
	for(vector<CNode*>::iterator Iterv1 = pvecAllNodes->begin(); Iterv1 != pvecAllNodes->end(); Iterv1++)
		for(vector<CNode*>::iterator Iterv2 = pvecAllNodes->begin(); Iterv2 != pvecAllNodes->end(); Iterv2++)
		{

			if(!(**Iterv1 == **Iterv2) && (*Iterv1)->IsConnected(*Iterv2)) 
			{
				pEtemp = new Edge(*Iterv1,*Iterv2,"YYY");
				if(*pEtemp  < *pE )
				{
					counter = 0;
					counter = m_SearchTraverse.SearchTraverse(pEtemp, &HistList);
					if (counter > 0)
					{
						if(pEtemp)
							delete pEtemp;
						if(pE)
							delete pE;
						if(pvecAllNodes)
							delete pvecAllNodes;

						return true;
					}
				}

				if(pEtemp)
				{
					delete pEtemp;
					pEtemp = NULL;
				}
			}
		}


	if(pvecAllNodes)
		delete pvecAllNodes;
	if(pE)
		delete pE;

	return false;
}

///////////////////////////////////////////////////////////////////////////////
// copy the given travers values to a new one
//the dest TravPair should be empty!!
void CBlackHoleTraverseList::CopyTravers(TravePair& Tsource, TravePair& Tdest)
{
	Tdest.second = new vector<CNode*>();
	for(vector<CNode*>::iterator Iter = Tsource.second->begin();Iter != Tsource.second->end();Iter++)
		Tdest.second->push_back(*Iter);

	Tdest.first = new vector<vector<int>*>;

	for (vector<vector<int>*>::iterator iter1 = Tsource.first->begin(); iter1 != Tsource.first->end(); iter1++)
	{
		vector<int>* pvecInt = new vector<int>();
		for (vector<int>::iterator iter2 = (*iter1)->begin(); iter2 != (*iter1)->end(); iter2++)
		{
			pvecInt->push_back(*iter2);
		}
		Tdest.first->push_back(pvecInt);
	}
}

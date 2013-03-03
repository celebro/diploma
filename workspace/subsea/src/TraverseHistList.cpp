#include "TraverseHistList.h"
#include "Graph.h"
#include "Node.h"
#include <queue>
#include <iostream>
#include <climits>
#include <algorithm>


///////////////////////////////////////////////////////////////////////////////
CTraverseHistList::CTraverseHistList()
{
	m_pvecTrav			= new vector<TravePair>();
	m_pvecTravMultiapl  = new vector<int>();
}

///////////////////////////////////////////////////////////////////////////////
CTraverseHistList::~CTraverseHistList()
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

	if(m_pvecTrav)
	{
		delete m_pvecTrav;
		m_pvecTrav = NULL;
	}


	if(m_pvecTravMultiapl)//The vector of "int" that represent the Multipale value of travers
		delete m_pvecTravMultiapl;
}

///////////////////////////////////////////////////////////////////////////////
//return a vector that represent the Travers of the graph.
vector<TravePair>& CTraverseHistList::ToVector()
{
	m_nCurrNum = 0;
	return *m_pvecTrav;
}

///////////////////////////////////////////////////////////////////////////////
//for each (v1,v2) that Adj make a Travers History 
//it will be saved in a member m_pvecTrav
bool CTraverseHistList::ProduceTravHistList(CGraph *pG)
{
	m_pG = pG;
	m_SearchTraverse.Init(m_pG);
	TravePair HistList;
	vector<CNode*>* pvecNodes = m_pG->GetAllNodes();
	Edge* pE;


	for (vector<CNode*>::iterator iter1 = pvecNodes->begin(); iter1 != pvecNodes->end(); iter1++)
		for (vector<CNode*>::iterator iter2 = pvecNodes->begin(); iter2 != pvecNodes->end(); iter2++)
		{

			if(!(**iter1  ==  **iter2) && (*iter1)->IsConnected(*iter2))//sharing edge  && not the same node
			{
				m_pG->ResetTravNodes();
				HistList = ProduceTravListOnPair(*iter1, *iter2);
				//if (!IsRedundant(HistList))
				if (!IsRedundant_Version2(HistList))//Version2
				{
					m_pvecTrav->push_back(HistList);
					
					//Lior Change 17.9.06 , solve the multiple count of subgraphs
					pE = new Edge(*iter1,*iter2,"YYY");
					m_pvecTravMultiapl->push_back(m_SearchTraverse.SearchTraverse(pE, &HistList));
					
					if(pE)
						delete pE;
					pE = NULL;
				}
				else
				{//delete the traverse
					if(HistList.second)
						delete HistList.second;

					for(vector<vector<int>*>::iterator IterFirst = HistList.first->begin();IterFirst != HistList.first->end();IterFirst++)
						if(*IterFirst)
							delete (*IterFirst);

					if(HistList.first)
						delete HistList.first;
				}
			}
			
		}
	delete pvecNodes;
	#ifdef _DEBUG_MODE_
		std::cout << "DEBUG_MODE BEGIN TRAVERSE HISTORY " << endl;
		PrintTrav();
		std::cout << "DEBUG_MODE END TRAVERSE HISTORY " << endl << endl;
	#endif
	return true;
}

///////////////////////////////////////////////////////////////////////////////
//for each (v1,v2) that Adj make a Travers History 
//it will be saved in a member m_pvecTrav; Natalia: faster version
bool CTraverseHistList::ProduceTravHistListFast(CGraph *pG)
{
	m_pG = pG;
	m_SearchTraverse.Init(m_pG);
	TravePair HistList;
	vector<CNode*>* pvecNodes = m_pG->GetAllNodes();
	Edge* pE;


	for (vector<CNode*>::iterator iter1 = pvecNodes->begin(); iter1 != pvecNodes->end(); iter1++)
    {
        vector<AdjNode*> adjNodes = (*iter1)->GetAdjNodes();
        
		for (vector<AdjNode*>::iterator iter2 = adjNodes.begin(); iter2 != adjNodes.end(); iter2++)
		{		
   		    	m_pG->ResetTravNodes();
				HistList = ProduceTravListOnPair(*iter1, (*iter2)->pNode);
				
                //	if (!IsRedundant_Version2(HistList))//Version2
				//{
					m_pvecTrav->push_back(HistList);
					
					//Lior Change 17.9.06 , solve the multiple count of subgraphs
					pE = new Edge(*iter1,(*iter2)->pNode,"YYY");
					m_pvecTravMultiapl->push_back(m_SearchTraverse.SearchTraverse(pE, &HistList));
					
					if(pE)
						delete pE;
					pE = NULL;
 
                    //}
        }
		}
	delete pvecNodes;
	#ifdef _DEBUG_MODE_
		std::cout << "DEBUG_MODE BEGIN TRAVERSE HISTORY " << endl;
		PrintTrav();
		std::cout << "DEBUG_MODE END TRAVERSE HISTORY " << endl << endl;
	#endif
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// build a Travers History for the edge(v1,v2)
TravePair CTraverseHistList::ProduceTravListOnPair(CNode* v1, CNode* v2)
{
	m_pG->ResetTravNodes();
	

	vector<CNode*>* pvecUnassignedNodes = m_pG->GetAllNodes();							
	vector<CNode*>* pvecAssignedNodes = new vector<CNode*>(); 
	vector<vector<int>*>* pvecNumbering = new vector<vector<int>*>();

	m_nCurrNum = 1;
	Visit(v1,pvecUnassignedNodes,pvecAssignedNodes,pvecNumbering,true,v2);

	delete pvecUnassignedNodes;
	return TravePair(pvecNumbering, pvecAssignedNodes);
}

///////////////////////////////////////////////////////////////////////////////
//Recursive function, This is the main function that will build the travers for the nodes.
void CTraverseHistList::Visit(CNode* V,
							  vector<CNode*>* pvecUnassignedNodes,
							  vector<CNode*>* pvecAssignedNodes,
							  vector<vector<int>*>* pvecNumbering,
							  bool FirstTime , CNode* v2)
{
	//Update the data according to the Node
	V->m_nTravNum = m_nCurrNum++;
	
	//Build H[m_nCurrNum] ans save it. H<{},... , {d(u)>0 : u is Adj to V} >
	vector<int>* pTempVec = new vector<int>();
	for (vector<CNode*>::iterator as_iter = pvecAssignedNodes->begin();as_iter != pvecAssignedNodes->end(); as_iter++)
		if(V->IsConnected(*as_iter))
			pTempVec->push_back((*as_iter)->m_nTravNum);
	pvecNumbering->push_back(pTempVec);

	//erase V from the UnassignedNodes Vector.
	//And Update AssignedNodes.
	pvecAssignedNodes->push_back(V);
    for(vector<CNode*>::iterator un_iter = pvecUnassignedNodes->begin();un_iter != pvecUnassignedNodes->end(); un_iter++)
		if (**un_iter == *V)
		{
			pvecUnassignedNodes->erase(un_iter);
			break;
            }

	//Diffrent Rec Call in the first time.
	if(FirstTime)
		Visit(v2,pvecUnassignedNodes,pvecAssignedNodes,pvecNumbering,false,NULL);//Rec

	//Build a vector of all the Nodes Adj to V that d(u) == 0 (not evaluated yet).
	vector<CNode*>* pvecAdjNodesToVNotEval = new vector<CNode*>(); //in the pare N(0)
	for(vector<CNode*>::iterator un_iter = pvecUnassignedNodes->begin();un_iter != pvecUnassignedNodes->end(); un_iter++)
		if(V->IsConnected(*un_iter))
			pvecAdjNodesToVNotEval ->push_back(*un_iter);

	CNode* pNextVisitNode;
	while(!pvecAdjNodesToVNotEval ->empty())
	{
		pNextVisitNode = EstimateNext(V,pvecAdjNodesToVNotEval);//choose the next Vertex
		if(pNextVisitNode->m_nTravNum == 0)
			Visit(pNextVisitNode,pvecUnassignedNodes,pvecAssignedNodes,pvecNumbering,false,NULL);//Rec

		//Erase the pNextVisitNode from pvecAdjNodesToVNotEval
		for(vector<CNode*>::iterator Iter = pvecAdjNodesToVNotEval->begin();Iter != pvecAdjNodesToVNotEval->end(); Iter++)
			if (**Iter == *pNextVisitNode )
			{
				pvecAdjNodesToVNotEval->erase(Iter);
				break;
			}
	}//end while
	if(pvecAdjNodesToVNotEval)
		delete pvecAdjNodesToVNotEval;

}

///////////////////////////////////////////////////////////////////////////////
//From all nodes in pvecAdjNodesToVNotEval (not evaluated yet) choose the next one 
//that will use in the traves.
//It will compare Lexicographic the pair<int,int> that each Node will derive .
CNode* CTraverseHistList::EstimateNext(CNode* V, vector<CNode*>* pvecAdjNodesToVNotEval)
{
	CNode* pBestNode;
	pair<int,int> BestNodeEval, TempEval;
	BestNodeEval = pair<int,int>(INT_MAX,INT_MAX);
	for(vector<CNode*>::iterator Iter = pvecAdjNodesToVNotEval->begin(); Iter != pvecAdjNodesToVNotEval->end();Iter++)
	{
		m_pG->ResetBFSColor();
		TempEval = EstimateNext(V,*Iter);
		if(TempEval < BestNodeEval)
		{
			pBestNode =  *Iter;
			BestNodeEval = TempEval;
		}
	}

	return pBestNode;
	//return (*pvecAdjNodesToVNotEval)[0];
}

///////////////////////////////////////////////////////////////////////////////
//Compute a pair<int,int> that represent the "value" 
//choosing this node to be the next in the Travers.
pair<int,int> CTraverseHistList::EstimateNext(CNode* v1, CNode* v2)
{
	int nBFSNextSliceSize = 0;
	int nLen = 1;
	int nNumbedrdNodes=0;//"p" in the paper
	bool bFirst;;

	CNode* pNodeTemp;
	vector<CNode*>* pvecTempAdjNodes;
	queue<CNode*> qBFSNextSlice;
	queue<CNode*> qBFSTempSlice;

	qBFSTempSlice.push(v2);
	while(!qBFSTempSlice.empty())
	{

		//Get the next "Slice", All the nodes that Adj to qBFSTempSlice and White color
		while(!qBFSTempSlice.empty())
		{
			pNodeTemp = qBFSTempSlice.front();
			qBFSTempSlice.pop();
			pvecTempAdjNodes = pNodeTemp->GetAdjNodesOnlyNodes();
			for(vector<CNode*>::iterator Iter = pvecTempAdjNodes->begin();Iter != pvecTempAdjNodes->end(); Iter++)
				if( !((*(*Iter) == *v1) && (*pNodeTemp == *v2)))
					if( ((*Iter)->m_BfsColor) == BFS_COLOR_WHITE )
					{
						qBFSNextSlice.push(*Iter);
						(*Iter)->m_BfsColor = BFS_COLOR_BLACK;//color the node to be black.
					}
			delete pvecTempAdjNodes;
		}

		//count the number of nodes in BFSTempSilce that have "number" in the travers
		//and move the elements to qBFSTempSlice.
		nNumbedrdNodes = 0;
		bFirst = true;
		while(!qBFSNextSlice.empty())
		{
			if(bFirst)
			{
				nBFSNextSliceSize = (int)qBFSNextSlice.size();
				bFirst = false;
			}
			pNodeTemp = qBFSNextSlice.front();
			qBFSNextSlice.pop();
			qBFSTempSlice.push(pNodeTemp);

			if(pNodeTemp->m_nTravNum != 0)
				nNumbedrdNodes++;
		}

		if(nNumbedrdNodes>0)
			return  pair<int,int>(nLen,-nNumbedrdNodes);

		nLen++; //inc the distance by 1 (Length).
	}

	return pair<int,int>(INT_MAX,-nBFSNextSliceSize);
}

///////////////////////////////////////////////////////////////////////////////
//Verify that this Travers History is not Redundant i.e isomorphic to other Travers
bool CTraverseHistList::IsRedundant(const TravePair& HistList)
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
//Global operator > for pair<int,int> 
//it compare the pairs lexicographical.
bool operator>(const pair<int,int>& pLeft ,const pair<int,int>& pRight)
{
	if(pLeft.first > pRight.first)
		return true;
	if(pLeft.first == pRight.first)
		if(pLeft.second > pRight.second)
			return true;
	return false;
}

///////////////////////////////////////////////////////////////////////////////
//DEBUG function
void CTraverseHistList::PrintTrav()
{
	static int i = 0;
	for(vector<TravePair>::const_iterator Iter = m_pvecTrav->begin();Iter != m_pvecTrav->end(); Iter++)
	{
		std::cout << endl;
		std::cout << "Traverse Number: " << i++ <<endl;
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
//Verify that this Travers History is not Redundant i.e isomorphic to other Travers
//version 2 , Add by lior 6.9.06 
//work same as the paper, using SearchTravers 
bool CTraverseHistList::IsRedundant_Version2(TravePair& HistList)
{
	//m_SearchTraverse.Init(m_pG);
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

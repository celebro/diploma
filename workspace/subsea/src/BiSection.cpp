#include "BiSection.h"
#include "Graph.h"
#include "Node.h"
#include "Def.h"
#include <vector>
#include <time.h>
#include <algorithm>
#include <climits>
#include <cstdio>
#ifdef _DEBUG_MODE_
	#include <iostream>
#endif

extern int gMaxCutFound,gTotalCutWeight;

///////////////////////////////////////////////////////////////////////////////
//pre declaration 
void Add2Hole(vector<CNode*>& vecNodeToSplit,vector<CNode*>* pvecCurrCut);
void ChooseRandNodeNoEdge(vector<CNode*>& vecNodeToSplit, vector<CNode*>* pvecCurrCut);
void ChooseRandNodeFromEdges(vector<CNode*>& vecAdjNodes, vector<CNode*>& vecNodeToSplit, vector<CNode*>* pvecCurrCut);
int RandNum(int nUpperBound);
vector<CNode*>* GetAllAdjNodes(vector<CNode*>& vecNodeToSplit, vector<CNode*>* pvecCurrCut);
bool GreedyBisectionImprove(vector<CNode*>* pvecCut1,vector<CNode*>* pvecCut2);
vector<CNode*>* GetAllAdjNodesNoRepeat(vector<CNode*>& vecNodeToSplit, vector<CNode*>* pvecCurrCut);

//version 2
//void CreateBiSection_Version2(CSubGraph& L_Graph ,CSubGraph* pLeftCut,CSubGraph* pRightCut);
void Add2Hole_Version2(vector<CNode*>& vecNodeToSplit,vector<CNode*>* pvecCurrCut, 
					   vector<CNode*>* pvecAdjNodesCurrCut,vector<CNode*>* pvecAdjNodesOtherCut);
void ChooseRandNodeFromEdges_Version2(vector<CNode*>& vecAdjNodes, vector<CNode*>& vecNodeToSplit
									  , vector<CNode*>* pvecCurrCut,vector<CNode*>* pvecAdjNodesOtherCut);
void ChooseRandNodeNoEdge_Version2(vector<CNode*>& vecNodeToSplit, vector<CNode*>* pvecCurrCut,
								   vector<CNode*>* pvecAdjNodesCurrCut,vector<CNode*>* pvecAdjNodesOtherCut);
///////////////////////////////////////////////////////////////////////////////
void CreateBiSection(CSubGraph& L_Graph ,CSubGraph* pLeftCut,CSubGraph* pRightCut)
{
	vector<CNode*>*	pvecNodeToSplit;
	vector<CNode*>*  pvecLeftCut  = new vector<CNode*>();
	vector<CNode*>*  pvecRightCut = new vector<CNode*>();
	pvecNodeToSplit = L_Graph.GetAllNodes();

	

	if(BISECTION_CUT_MODE==0)//random cut
	{
		int dividor=0;
		for(vector<CNode*>::iterator Iter = pvecNodeToSplit->begin();Iter != pvecNodeToSplit->end();Iter++,dividor++)
		{
			if(dividor%2 == 0)
				pvecLeftCut->push_back(*Iter);
			else
				pvecRightCut->push_back(*Iter); 
		}
	}


	if(BISECTION_CUT_MODE==1)//regular cut
	{
		while(!pvecNodeToSplit->empty())
		{
			Add2Hole(*pvecNodeToSplit,pvecLeftCut);
			if(!pvecNodeToSplit->empty())
				Add2Hole(*pvecNodeToSplit,pvecRightCut);
		}
	}

	//Step 2 - "Simple Greedy BiSection Method"
	int i;
	for(i=0; i < BISECTION_GREEDY_IMPROV_DEPTH; i++)
		if(GreedyBisectionImprove(pvecLeftCut,pvecRightCut) == false)
			break;

	pLeftCut->Init(pvecLeftCut);
	pRightCut->Init(pvecRightCut);

	#ifdef _DEBUG_MODE_
		std::cout<< "DEBUG BEGIN BISECTION" << endl ;
		std::cout << "current cut: "<< endl;
		for (vector<CNode*>::iterator Iter = pvecLeftCut->begin( ) ; Iter != pvecLeftCut->end( ) ; Iter++ )
			std::cout << " " << (*Iter)->m_strName;
		std::cout << endl;

		for (vector<CNode*>::iterator Iter = pvecRightCut->begin( ) ; Iter != pvecRightCut->end( ) ; Iter++ )
			std::cout << " " << (*Iter)->m_strName;
		std::cout << endl;

		vector<CNode*>* pvecAdjNodes = GetAllAdjNodes(*pvecLeftCut,pvecRightCut);
		std::cout << "cut weight (number of cross edges) : "  << (int)pvecAdjNodes->size() << endl;
		delete pvecAdjNodes;
		std::cout <<"number of gready improvment done: " << i << endl;

		std::cout<< "DEBUG END BISECTION" << endl << endl ;
	#endif 

	delete pvecLeftCut;
	delete pvecRightCut;
	delete pvecNodeToSplit;
}


///////////////////////////////////////////////////////////////////////////////
void Add2Hole(vector<CNode*>& vecNodeToSplit,vector<CNode*>* pvecCurrCut)
{
	vector<CNode*>* vecAdjNodes;
		
	vecAdjNodes = GetAllAdjNodes(vecNodeToSplit, pvecCurrCut);
	
	if(vecAdjNodes->empty())
		//choose a random node from vecNodeToSplit, and update the vectors.
		ChooseRandNodeNoEdge(vecNodeToSplit, pvecCurrCut);
	else
		//choose a rand node from vecCurrNodeToSplit, and update the vectors.
		ChooseRandNodeFromEdges(*vecAdjNodes, vecNodeToSplit, pvecCurrCut);
	
	delete vecAdjNodes;
}

///////////////////////////////////////////////////////////////////////////////
//choose a random node from vecNodeToSplit, and update the vectors.
void ChooseRandNodeNoEdge(vector<CNode*>& vecNodeToSplit, vector<CNode*>* pvecCurrCut)
{
	int nRand = RandNum((int)vecNodeToSplit.size()-1);

	if(nRand > (int)vecNodeToSplit.size() -1)
		printf("Error: BiSection , ChooseRandNodeNoEdge , element out of target %d \n",nRand);

	CNode*& pRemoveNode = vecNodeToSplit[nRand];
	pvecCurrCut->push_back(pRemoveNode); //Add the new CNode to the CurrentVector

	//erase the CNode from the vecNodeToSplit
	vecNodeToSplit.erase(vecNodeToSplit.begin()+ nRand);
}

///////////////////////////////////////////////////////////////////////////////
//choose a rand node from vecCurrNodeToSplit, and update the vectors.
void ChooseRandNodeFromEdges(vector<CNode*>& vecAdjNodes, vector<CNode*>& vecNodeToSplit, vector<CNode*>* pvecCurrCut)
{
	int nRand = RandNum((int)vecAdjNodes.size()-1);
	

	if(nRand > (int)vecAdjNodes.size() -1)
		printf("Error: BiSection , ChooseRandNodeFromEdges , element out of target %d \n",nRand);

	CNode*& pRemoveNode = vecAdjNodes[nRand];
	pvecCurrCut->push_back(pRemoveNode); //Add the new Node to the CurrentVector

	//erase the Node from the vecNodeToSplit
	vector<CNode*>::iterator Iter;

    for (Iter = vecNodeToSplit.begin(); Iter != vecNodeToSplit.end(); Iter++ )
        if(**Iter == *pRemoveNode)
		{
            vecNodeToSplit.erase(Iter);
			break;
		}

	//vecNodeToSplit.erase(pRemoveNode);
}


///////////////////////////////////////////////////////////////////////////////
//return a random num in range  0....nUpperBound (include)
int RandNum(int nUpperBound)
{
	static int nsRandomCounter = 0;
	int num;

	if (nsRandomCounter % RANDOM_COUNTER_MOD == 0)
		srand( (unsigned)time( NULL ));
	nsRandomCounter++;

	num = rand();
	return  num%(nUpperBound+1);
	//return (int)(((float)num/RAND_MAX)*(nUpperBound));
}

///////////////////////////////////////////////////////////////////////////////
//Build a vector(vecAdjNodes) of all the nodes in vecNodeToSplit that are adj to vecNodeToSplit
//It might contain duplicated Nodes, acording to the number of Edges entering to the node.
vector<CNode*>* GetAllAdjNodes(vector<CNode*>& vecNodeToSplit, vector<CNode*>* pvecCurrCut)
{
	vector<CNode*>*  vecAdjNodes  = new vector<CNode*>();
	//vector<CNode*>*  vecAdjMerge	 = new vector<CNode*>();
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
//Step 2 - "Simple Greedy BiSection Method" Improving the MinCut
//return the improve cut inside the args pvecCut1 pvecCut2.
bool GreedyBisectionImprove(vector<CNode*>* pvecCut1,vector<CNode*>* pvecCut2)
{
	vector<int>*  pvecC1Gain = new vector<int>();
	vector<int>*  pvecC2Gain= new vector<int>();
	vector<CNode*>* pvecAdjNodes;
	vector<CNode*>::iterator Iter1,Iter2;
	vector<int>::iterator iIter1,iIter2;
	int nExternal;
		
	//calculate the Internal and External for each node in C1
	pvecAdjNodes = GetAllAdjNodesNoRepeat(*pvecCut2 ,pvecCut1); //Nodes from Cut2 that are Adj to Cut1
	for (Iter1 = pvecCut1->begin(); Iter1 != pvecCut1->end(); Iter1++ )
	{
		nExternal = 0;
		for (Iter2 = pvecAdjNodes ->begin(); Iter2 != pvecAdjNodes ->end(); Iter2++ )
		{
			if((*Iter1)->IsConnected(*Iter2))
				nExternal++;
		}

		pvecC1Gain->push_back(2*nExternal - (*Iter1)->GetNumOfAdjNodes());
		//std::cout << (*Iter1)->GetNumOfAdjNodes();
		
	}
	if(pvecAdjNodes)
	{
		delete pvecAdjNodes;
		pvecAdjNodes = NULL;
	}

	
	//calculate the Internal and External for each node in C2
	//pvecAdjNodes->clear();
	pvecAdjNodes = GetAllAdjNodesNoRepeat(*pvecCut1 ,pvecCut2); //Nodes from Cut1 that are Adj to Cut2
	for (Iter1 = pvecCut2->begin(); Iter1 != pvecCut2->end(); Iter1++ )
	{
		nExternal = 0;
		for (Iter2 = pvecAdjNodes ->begin(); Iter2 != pvecAdjNodes ->end(); Iter2++ )
		{
			if((*Iter1)->IsConnected(*Iter2))
				nExternal++;
		}
		pvecC2Gain->push_back(2*nExternal - (*Iter1)->GetNumOfAdjNodes());
		
	}

	//claculate the Best Gain of  node from C1 X C2.
	int nC1Max = INT_MIN;
	int nC1Pos;
	int nC2Max = INT_MIN;
	int nC2Pos;
	int i;

	i = 0;
	for (iIter1 = pvecC1Gain->begin(); iIter1 != pvecC1Gain->end(); iIter1++)
	{
		if(*iIter1 > nC1Max)
		{
			nC1Max = *iIter1;
			nC1Pos = i;
		}
		i++;
	}

	i = 0;
	for (iIter1 = pvecC2Gain->begin(); iIter1 != pvecC2Gain->end(); iIter1++ )
	{
		if(*iIter1 > nC2Max)
		{
			nC2Max = *iIter1;
			nC2Pos = i;
		}
		i++;
	}
		
	//Swap the two elements.
	if((int)pvecCut2->size()>nC2Pos  && (int)pvecCut1->size()>nC1Pos )
	{
		if(((*pvecC1Gain)[nC1Pos]+ (*pvecC2Gain)[nC2Pos] - 2) > 0) 
		{
			CNode*& Temp1Node = (*pvecCut1)[nC1Pos];
			CNode*  Temp2Node  = Temp1Node;
			(*pvecCut1)[nC1Pos] = (*pvecCut2)[nC2Pos];
			(*pvecCut2)[nC2Pos] = Temp2Node;
		}
		else
		{
			delete pvecC1Gain;
			delete pvecC2Gain;
			delete pvecAdjNodes;
			return false;
		}
	}
	else
		printf("Error: BiSection , GreedyBisectionImprove , element out of target %d or %d \n",nC1Pos ,nC2Pos);
	

	delete pvecC1Gain;
	delete pvecC2Gain;
	delete pvecAdjNodes;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
//Build a vector(vecAdjNodes) of all the nodes in vecNodeToSplit that are adj to vecNodeToSplit
//Eatch Node will appear only once.
vector<CNode*>* GetAllAdjNodesNoRepeat(vector<CNode*>& vecNodeToSplit, vector<CNode*>* pvecCurrCut)
{
	vector<CNode*>*  vecAdjNodes  = new vector<CNode*>();
	vector<CNode*>*  vecAdjMerge	 = new vector<CNode*>();
	vector<CNode*>::iterator Iter1,Iter2;
	
	if(pvecCurrCut->size() == 0)
        return vecAdjNodes;
	
	
	for (Iter1 = pvecCurrCut->begin(); Iter1 != pvecCurrCut->end(); Iter1++ )
		for (Iter2 = vecNodeToSplit.begin(); Iter2 != vecNodeToSplit.end(); Iter2++ )
			if((*Iter1)->IsConnected(*Iter2))
				vecAdjNodes->push_back(*Iter2);


	//Clean duplicated Nodes
	sort(vecAdjNodes->begin(),vecAdjNodes->end());

	if(vecAdjNodes->size() != 0)
	{
		vecAdjMerge->push_back(*vecAdjNodes->begin());
		for (Iter1 = vecAdjNodes->begin(); Iter1 != vecAdjNodes->end(); Iter1++ )
		{
			if(!((**Iter1) ==  *(vecAdjMerge->back())))
				vecAdjMerge->push_back(*Iter1);

		}
	}
	delete vecAdjNodes;
	return vecAdjMerge;
}

///////////////////////////////////////////////////////////////////////////////
//Added by Lior: 5.9.06 - Improvment, only update the cross edges and avoid 
//calling GetAllAdjNodes repeatedly.
///////////////////////////////////////////////////////////////////////////////
void Add2Hole_Version2(vector<CNode*>& vecNodeToSplit,vector<CNode*>* pvecCurrCut, 
					   vector<CNode*>* pvecAdjNodesCurrCut,vector<CNode*>* pvecAdjNodesOtherCut)
{
	//vecAdjNodes = GetAllAdjNodes(vecNodeToSplit, pvecCurrCut);
	
	if(pvecAdjNodesCurrCut->empty())
		//choose a random node from vecNodeToSplit, and update the vectors.
		ChooseRandNodeNoEdge_Version2(vecNodeToSplit, pvecCurrCut, pvecAdjNodesCurrCut, pvecAdjNodesOtherCut);
	else
		//choose a rand node from vecCurrNodeToSplit, and update the vectors.
		ChooseRandNodeFromEdges_Version2(*pvecAdjNodesCurrCut, vecNodeToSplit, pvecCurrCut,pvecAdjNodesOtherCut);
	
	//delete vecAdjNodes;
}

///////////////////////////////////////////////////////////////////////////////
//choose a random node from vecNodeToSplit, and update the vectors.
void ChooseRandNodeNoEdge_Version2(vector<CNode*>& vecNodeToSplit, vector<CNode*>* pvecCurrCut,
								   vector<CNode*>* pvecAdjNodesCurrCut,vector<CNode*>* pvecAdjNodesOtherCut)
{
	int nRand = RandNum((int)vecNodeToSplit.size()-1);

	if(nRand > (int)vecNodeToSplit.size() -1)
		printf("Error: BiSection , ChooseRandNodeNoEdge , element out of target %d \n",nRand);

	CNode*& pRemoveNode = vecNodeToSplit[nRand];
	CNode* pRemoveNodeCopy = pRemoveNode;
	pvecCurrCut->push_back(pRemoveNode); //Add the new CNode to the CurrentVector

	//erase the CNode from the vecNodeToSplit
	vecNodeToSplit.erase(vecNodeToSplit.begin()+ nRand);

	//second vertion changes
	//update the current AdjNodes
	vector<CNode*>::iterator Iter;
	for(Iter = vecNodeToSplit.begin(); Iter != vecNodeToSplit.end(); Iter++)
		if(pRemoveNodeCopy->IsConnected(*Iter))
			pvecAdjNodesCurrCut->push_back(*Iter);

	//update other AdjNodes by removing the selected node.
	bool bStop = false;
	while(!bStop)
	{
		bStop = true;
		for(Iter = pvecAdjNodesOtherCut->begin(); Iter != pvecAdjNodesOtherCut->end(); Iter++)
		{
			if((**Iter)==(*pRemoveNodeCopy))
			{
				pvecAdjNodesOtherCut->erase(Iter);
				bStop = false;
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//choose a rand node from vecCurrNodeToSplit, and update the vectors.
void ChooseRandNodeFromEdges_Version2(vector<CNode*>& vecAdjNodes, vector<CNode*>& vecNodeToSplit
									  , vector<CNode*>* pvecCurrCut,vector<CNode*>* pvecAdjNodesOtherCut)
{
	int nRand = RandNum((int)vecAdjNodes.size()-1);
	

	if(nRand > (int)vecAdjNodes.size() -1)
		printf("Error: BiSection , ChooseRandNodeFromEdges , element out of target %d \n",nRand);

	CNode*& pRemoveNode = vecAdjNodes[nRand];
	CNode* pRemoveNodeCopy = pRemoveNode;
	pvecCurrCut->push_back(pRemoveNode); //Add the new Node to the CurrentVector

	//erase the Node from the vecNodeToSplit
	vector<CNode*>::iterator Iter;

    for (Iter = vecNodeToSplit.begin(); Iter != vecNodeToSplit.end(); Iter++ )
	{
        if(**Iter == *pRemoveNodeCopy)
		{
            vecNodeToSplit.erase(Iter);
			break;
		}
	}

	//version 2 update.

	//remove the node from the currCut (vecAdjNodes)
	bool bStop = false;
	while(!bStop)
	{
		bStop = true;
		for(Iter = vecAdjNodes.begin(); Iter != vecAdjNodes.end(); Iter++)
		{
			if((**Iter)==(*pRemoveNodeCopy))
			{
				vecAdjNodes.erase(Iter);
				bStop = false;
				break;
			}
		}
	}

	//add the new nodes to the currcut
	for(Iter = vecNodeToSplit.begin(); Iter != vecNodeToSplit.end(); Iter++)
		if((*Iter)->IsConnected(pRemoveNodeCopy))
			vecAdjNodes.push_back(*Iter);
			


	//remove the node from pvecAdjNodesOtherCut
	bStop = false;
	while(!bStop)
	{
		bStop = true;
		for(Iter = pvecAdjNodesOtherCut->begin(); Iter != pvecAdjNodesOtherCut->end(); Iter++)
		{
			if((**Iter)==(*pRemoveNodeCopy))
			{
				pvecAdjNodesOtherCut->erase(Iter);
				bStop = false;
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//version 2
void CreateBiSection_Version2(CSubGraph& L_Graph ,CSubGraph* pLeftCut,CSubGraph* pRightCut)
{
	vector<CNode*>*	pvecNodeToSplit;
	vector<CNode*>*  pvecLeftCut  = new vector<CNode*>();
	vector<CNode*>*  pvecRightCut = new vector<CNode*>();
	pvecNodeToSplit = L_Graph.GetAllNodes();

	vector<CNode*>*  pvecAdjNodesLeftCut  = new vector<CNode*>();
	vector<CNode*>*  pvecAdjNodesRightCut = new vector<CNode*>();

	////////////////////////////////////////////////random cut///
	if(BISECTION_CUT_MODE==0)//random cut
	{
		int dividor=0;
		for(vector<CNode*>::iterator Iter = pvecNodeToSplit->begin();Iter != pvecNodeToSplit->end();Iter++,dividor++)
		{
			if(dividor%2 == 0)
				pvecLeftCut->push_back(*Iter);
			else
				pvecRightCut->push_back(*Iter); 
		}


	}

	////////////////////////////////////////////////regular cut///
	if(BISECTION_CUT_MODE==1)//regular cut
	{
		while(!pvecNodeToSplit->empty())
		{
			Add2Hole_Version2(*pvecNodeToSplit,pvecLeftCut,pvecAdjNodesLeftCut ,pvecAdjNodesRightCut);
			if(!pvecNodeToSplit->empty())
				Add2Hole_Version2(*pvecNodeToSplit,pvecRightCut,pvecAdjNodesRightCut,pvecAdjNodesLeftCut);
		}
	}

	//Step 2 - "Simple Greedy BiSection Method"
	int i;
	for(i=0; i < BISECTION_GREEDY_IMPROV_DEPTH; i++)
		if(GreedyBisectionImprove(pvecLeftCut,pvecRightCut) == false)
			break;

	pLeftCut->Init(pvecLeftCut);
	pRightCut->Init(pvecRightCut);

	#ifdef _DEBUG_MODE_
		std::cout<< "DEBUG BEGIN BISECTION" << endl ;
		std::cout << "current cut: "<< endl;
		for (vector<CNode*>::iterator Iter = pvecLeftCut->begin( ) ; Iter != pvecLeftCut->end( ) ; Iter++ )
			std::cout << " " << (*Iter)->m_strName;
		std::cout << endl;

		for (vector<CNode*>::iterator Iter = pvecRightCut->begin( ) ; Iter != pvecRightCut->end( ) ; Iter++ )
			std::cout << " " << (*Iter)->m_strName;
		std::cout << endl;

		vector<CNode*>* pvecAdjNodes = GetAllAdjNodes(*pvecLeftCut,pvecRightCut);
		std::cout << "cut weight (number of cross edges) : "  << (int)pvecAdjNodes->size() << endl;
		delete pvecAdjNodes;
		std::cout <<"number of gready improvment done: " << i << endl;

		std::cout<< "DEBUG END BISECTION" << endl << endl ;
	#endif 

	if(STATISTIC_INFO)
	{
		vector<CNode*>* pvecAdjNodes = GetAllAdjNodesNoRepeat(*pvecLeftCut,pvecRightCut);
		int CutWeight = (int)pvecAdjNodes ->size();

		if(gMaxCutFound < CutWeight )
			gMaxCutFound = CutWeight;

		gTotalCutWeight += CutWeight ;//global statistic counter
		delete pvecAdjNodes;
	}

	delete pvecLeftCut;
	delete pvecRightCut;
	delete pvecNodeToSplit;
	delete pvecAdjNodesLeftCut;
	delete pvecAdjNodesRightCut;

}

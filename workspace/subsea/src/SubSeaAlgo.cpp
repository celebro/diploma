#include "SubSeaAlgo.h"
#include "Graph.h"
#include "BiSection.h"
#include "TraverseHistList.h"
#include "Def.h"
#include "wclique1.h"
#include <ctime>

#ifdef _DEBUG_MODE_
	#include <iostream>
#endif

#include <iostream>

bool algorithmRunning = false;

extern clock_t TimeToFindAllOcc,TimeBeginMIS,TimeEndMIS; //Global statistic Timer
extern int gNumOfBisectionCut;

#ifndef WIN32
	#include "sys/times.h"
	#include "time.h"
	extern struct tms TMS;
	extern clock_t tu_end,ts_end;
	extern clock_t tu_begin,ts_begin;
#endif


///////////////////////////////////////////////////////////////////////////////
//Main Algo 
//(recursive funciotn) it will be called on the leftCut and RightCut
int CSubSeaAlgo::ProduceSubIso(CSubGraph& L_Graph)
{
	#ifdef _DEBUG_MODE_
		std::cout << "DEBUG_MODE BEGIN CSubSeaAlgo::ProduceSubIso " << endl;
		std::cout<<"The Graph that enters the Rec."<< endl;
		std::cout<<"Graph: ";
		for(vector<CNode*>::iterator Iter = L_Graph.m_pvecNodes->begin(); Iter != L_Graph.m_pvecNodes->end(); Iter++)
			std::cout<< "(" <<  (*Iter)->m_nId <<", "<<(*Iter)->m_strName<< ") ";
		std::cout<<endl;
		std::cout << "DEBUG_MODE   END CSubSeaAlgo::ProduceSubIso " << endl << endl;
     #endif

	int nNumOfIsoFound = 0;
	
	//The recursive STOP condition
	if ((int)((*m_pvecTravHist)[0]).first->size() > L_Graph.GetNumOfNodes())
		return 0;

	//Allocate two new graphs LeftCut and RightCut, for the BiSection
	CSubGraph* pLeftCut = new CSubGraph();
	CSubGraph* pRightCut = new CSubGraph();

	vector<Edge*>* pvecCrossEdges = new vector<Edge*>();

	//CreateBiSection(L_Graph, pLeftCut, pRightCut);//Find Min Cut
	CreateBiSection_Version2(L_Graph, pLeftCut, pRightCut);//Find Min Cut
	gNumOfBisectionCut++; //global statistic counter

	GetCrossEdges(pLeftCut, pRightCut, pvecCrossEdges);//Get All the CrossEdges

	#ifdef _DEBUG_MODE_
		std::cout << "DEBUG_MODE BEGIN ProduceSubIso The MinCut Edges & Num of iso found" << endl;
		std::cout<<"Min Cut edges: ";
		for (vector<Edge*>::iterator edge_iter = pvecCrossEdges->begin(); edge_iter != pvecCrossEdges->end(); edge_iter++)
		{
			std::cout<< "(" <<  (*edge_iter )->strName <<", ";
			std::cout<< "[" <<  (*edge_iter )->v1->m_nId <<", "<<(*edge_iter )->v1->m_strName<< "] ,";
			std::cout<< "[" <<  (*edge_iter )->v2->m_nId <<", "<<(*edge_iter )->v2->m_strName<< "] )";
		}
		std::cout<<endl;
	#endif

	//Main loop, run over all the cross edges
	//for each edge check all traverse histories
	int nMulCounterHelper = 0;
	int nStepCounter = 0;

    //cout<<"Exploring cross-edges "<<endl;

	for (vector<Edge*>::iterator edge_iter = pvecCrossEdges->begin(); edge_iter != pvecCrossEdges->end(); edge_iter++)
	{
	#ifdef _DEBUG_MODE_            
        cout<<"1 Using cross edge ("<<(*edge_iter )->v1->m_nId<<", "<< (*edge_iter )->v2->m_nId<<")"<<endl;
	#endif
            
		nStepCounter = 0;
		for (vector<TravePair>::iterator trav_iter = m_pvecTravHist->begin(); trav_iter != m_pvecTravHist->end(); trav_iter++)
		{
            // *edge_iter comes from large graph                       
			nMulCounterHelper = m_SearchTraverse.SearchTraverse(*edge_iter, &(*trav_iter));
			
			//Lior Change 17.9.06 , sovle the multipale count of subgraphs	
			if( nMulCounterHelper % m_pvecTravMultipaleCout->at(nStepCounter) != 0 ) 
			{
				std::cout<<"Num of subgraphs found isnt divisable in func ProduceSubIso() " << nMulCounterHelper 
				<< ", " << m_pvecTravMultipaleCout->at(nStepCounter) << std::endl;
			}
			//DEBUG
			//cout << nMulCounterHelper  << " ";
			//cout << m_pvecTravMultipaleCout->at(nStepCounter)  << " ";
			//cout << nNumOfIsoFound  << "\n";
			//DEBUG

			nMulCounterHelper = nMulCounterHelper / m_pvecTravMultipaleCout->at(nStepCounter);
			nNumOfIsoFound += nMulCounterHelper;
			nStepCounter++;
		}
	
		
		m_pGlarge->RemoveEdge(*edge_iter);
		if(*edge_iter)
			delete *edge_iter;
		
	}
	

	#ifdef _DEBUG_MODE_
		std::cout <<"Number of IsoMorphism found ""ON"" this cut: " << nNumOfIsoFound << endl;
		std::cout << "DEBUG_MODE   END ProduceSubIso The MinCut Edges & Num of iso found" << endl << endl;
	#endif



	delete pvecCrossEdges;
	//recursive call on the Left and Right Cut
	int n1 = ProduceSubIso(*pLeftCut);
	delete pLeftCut;

	int n2 = ProduceSubIso(*pRightCut);
	delete pRightCut;
	return nNumOfIsoFound + n1 + n2;
}

///////////////////////////////////////////////////////////////////////////////
pair<int,int> CSubSeaAlgo::Run()
{

	#ifndef WIN32  //Take the sysmet & user time
		times(&TMS);
		tu_begin  = TMS.tms_utime;
		ts_begin  = TMS.tms_stime;
	#endif

    #ifdef _DEBUG_MODE_    
      cout << "IN RUN()"<<endl;
    #endif  

      algorithmRunning = true;  
      
	//call the main Algo
	CSubGraph* pSG_large = new CSubGraph();
	vector<CNode*>*  pvecLargeGraphNodes;
	pvecLargeGraphNodes = m_pGlarge->GetAllNodes();
	pSG_large->Init(pvecLargeGraphNodes);//convert it to SubGraph
	delete(pvecLargeGraphNodes);

	int nNumOfIso = ProduceSubIso(*pSG_large);
	TimeToFindAllOcc = clock(); //statistic Timer
	
	#ifdef _DEBUG_MODE_
		int nEdges = 0;
		vector<CNode*>* pvecNodesLeftEdges = m_pGlarge->GetAllNodes();
		for(vector<CNode*>::iterator Iter = pvecNodesLeftEdges->begin(); Iter != pvecNodesLeftEdges->end();Iter++)
			nEdges += (*Iter)->GetNumOfAdjNodes();
		
		std::cout << "*****************************************" << endl;
		std::cout << "Number of edges left after the algo: " << nEdges/2 << endl;
	#endif

	int nMisSize = -1;
	
	#ifndef WIN32 //Take the sysmet & user time
			times(&TMS);
			tu_end  = TMS.tms_utime;
			ts_end  = TMS.tms_stime;
	#endif
	if(PRODUCE_MIS)
	{
		std::cout << "Starting to generate MIS" <<std::endl;
		TimeBeginMIS = clock();
		vector<CIsoSubGraph*>* p_vecIsoSubGraph;
        	p_vecIsoSubGraph = m_SearchTraverse.GetIsoSubGraph();

		string strFilename = "MisGraph.txt";
		if(m_MISFC.WriteToFile(p_vecIsoSubGraph, strFilename))
			nMisSize= RunMis((char*)strFilename.c_str());

		TimeEndMIS = clock();
	}

	//free the memory
	delete pSG_large;
		
	//return nNumOfIso;
	return std::pair<int,int>(nNumOfIso,nMisSize);

}

///////////////////////////////////////////////////////////////////////////////
//Init the small grap and large graph from the files
//Buid all the Travers History
bool CSubSeaAlgo::Init(string strSmallGraphFilename, string strLargeGraphFilename /* = "" */)
{
	bool bRetVal = false;

	if (strLargeGraphFilename.empty() && !m_pGlarge)
		return false;

	if (m_pGsmall)
		delete m_pGsmall;

	m_pGsmall = new CGraph();
	
	if (strLargeGraphFilename.empty())
	{
		m_pGlarge->Reset();
		bRetVal = m_pGsmall->InitFromFile(strSmallGraphFilename);
	}
	else
	{
		m_pGlarge = new CGraph();

		//Init the small and large graph from a file.
		if (m_pGlarge->InitFromFile(strLargeGraphFilename))
			bRetVal = m_pGsmall->InitFromFile(strSmallGraphFilename);

	}
	
	if (bRetVal)
	{
		//Build all the traverse histories of the small graph
		//cout<<"Building all trav histories of the small graph..."<<endl;
		if (m_pAllTravList)
		{
			delete m_pAllTravList;
			m_pAllTravList = NULL;
		}

		if (USE_BLACK_HOLE_TRAVERS_HISTORY)
			m_pAllTravList = new CBlackHoleTraverseList();//Travers Option 2 BlackHole (Arie)
		else
			m_pAllTravList = new CTraverseHistList();	  //Travers Option 1 (lior)

		//bRetVal = m_pAllTravList->ProduceTravHistList(m_pGsmall);
        bRetVal = m_pAllTravList->ProduceTravHistListFast(m_pGsmall);
        
		m_pvecTravHist = &m_pAllTravList->ToVector();
		m_pvecTravMultipaleCout = &m_pAllTravList->GetMultiapeVec();
        //cout<<"Finished"<<endl;
	}

	m_SearchTraverse.Reset();
	m_SearchTraverse.Init(m_pGlarge);
	
	return bRetVal;
}

///////////////////////////////////////////////////////////////////////////////
//Get All the CrossEdges
void CSubSeaAlgo::GetCrossEdges(CSubGraph* pLeftCut, CSubGraph* pRightCut,vector<Edge*>* pvecCrossEdges)
{
	vector<CNode*>* pNodesLeft;
	vector<CNode*>* pNodesRight;

	if (!pLeftCut || !pRightCut)
	{
		pvecCrossEdges->clear();
		return;
	}

	pNodesLeft  = pLeftCut->m_pvecNodes;
	pNodesRight = pRightCut->m_pvecNodes;

	for (vector<CNode*>::iterator left_iter = pNodesLeft->begin(); left_iter != pNodesLeft->end(); left_iter++)
	{
		for (vector<CNode*>::iterator right_iter = pNodesRight->begin(); right_iter != pNodesRight->end(); right_iter++)
		{
			if ((*left_iter)->IsConnected(*right_iter))
			{

				if ((*left_iter)->GetNumOfAdjNodes() < (*right_iter)->GetNumOfAdjNodes())
				{
					 vector<AdjNode*>& vecAdjNodes = (*left_iter)->GetAdjNodesRef();
					 for (vector<AdjNode*>::iterator iter_adj = vecAdjNodes.begin(); iter_adj != vecAdjNodes.end(); iter_adj++)
					 {
						 if ((*(*iter_adj)->pNode) == *(*right_iter))
						 {
							 pvecCrossEdges->push_back(new Edge(*left_iter, *right_iter, (*iter_adj)->strEdgeName));
							 break;
						 }
					 }
				}

				else
				{
					 vector<AdjNode*>& vecAdjNodes = (*right_iter)->GetAdjNodesRef();
					 for (vector<AdjNode*>::iterator iter_adj = vecAdjNodes.begin(); iter_adj != vecAdjNodes.end(); iter_adj++)
					 {
						 if ((*(*iter_adj)->pNode) == *(*left_iter))
						 {
							 pvecCrossEdges->push_back(new Edge(*right_iter, *left_iter, (*iter_adj)->strEdgeName));
							 break;
						 }
					 }
				}
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
CSubSeaAlgo::CSubSeaAlgo(): m_SearchTraverse(true)
{
	m_pGlarge = NULL; //The Large Graph
	m_pGsmall = NULL; //Small patern Graph
	m_pAllTravList = NULL; // Travers History Object . Original Version.
	m_pvecTravHist = NULL;//The vector of all the Traverses
	
}

///////////////////////////////////////////////////////////////////////////////
CSubSeaAlgo::~CSubSeaAlgo()
{
	if(m_pGlarge)
		delete m_pGlarge; //The Large Graph
	if(m_pGsmall)
		delete m_pGsmall; //Small patern Graph
	if(m_pAllTravList)
		delete m_pAllTravList; // Travers History Object . Original Version.
	//if(m_pvecTravHist)
	//	delete m_pvecTravHist;//The vector of all the Traverses
	
}


///////////////////////////////////////////////////////////////////////////////
//compete the subsea algo with brute force
int CSubSeaAlgo::RunBruteForce()
{
    //cout<<"In RunBruteForce"<<endl;
	int ans = 0;
	int StepCounter = 0;
	vector<CNode*> *pVec1,*pVec2;
	pVec1 = m_pGlarge->GetAllNodes();
	pVec2 = m_pGlarge->GetAllNodes();
	Edge E(NULL,NULL,"");

	for (vector<CNode*>::iterator iter1 = pVec1->begin(); iter1 != pVec1->end(); iter1++)
		for (vector<CNode*>::iterator iter2 = pVec2->begin(); iter2 != pVec2->end(); iter2++)
		{
			if((*iter1)->IsConnected(*iter2))
			{
				StepCounter = 0;
				for (vector<TravePair>::iterator trav_iter = m_pvecTravHist->begin(); trav_iter != m_pvecTravHist->end(); trav_iter++)
				{
					
					E.v1 = *iter1;
					E.v2 = *iter2;
					E.strName = "ABC";
					ans += (m_SearchTraverse.SearchTraverse(&E, &(*trav_iter))) /  m_pvecTravMultipaleCout->at(StepCounter);
					StepCounter++;
				}
				m_pGlarge->RemoveEdge(&E);
			}
		}
	return ans;
}

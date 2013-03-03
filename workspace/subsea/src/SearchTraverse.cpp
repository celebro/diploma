#include "SearchTraverse.h"
#include "TraverseHistList.h"
#include "BiSection.h"
#include <algorithm>
#include "Def.h"
#include <ctime>
#include <iostream>

#ifdef _DEBUG_MODE_
	#include <iostream>
#endif


extern clock_t TimeToFindFirstOcc; //statistic Timer
extern bool bFirstOcc;
extern bool algorithmRunning;


///////////////////////////////////////////////////////////////////////////////
CSearchTraverse::CSearchTraverse(bool bRetSubGraph)
{
	m_bRetSubGraph = bRetSubGraph;
	m_pvecNumberdNodes = NULL;
	m_pvecMISIsoGraphs  = NULL;

}

///////////////////////////////////////////////////////////////////////////////
CSearchTraverse::~CSearchTraverse()
{
	if (m_pvecNumberdNodes)
	{
		delete m_pvecNumberdNodes;
		m_pvecNumberdNodes = NULL;
	}

	if(m_pvecMISIsoGraphs)
	{
		for(vector<CIsoSubGraph*>::iterator iter = m_pvecMISIsoGraphs->begin(); iter != m_pvecMISIsoGraphs->end(); iter++)
		{
			if(*iter)
			{
				delete *iter;
				*iter = NULL;
			}
		}

		delete m_pvecMISIsoGraphs;
		m_pvecMISIsoGraphs = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
void CSearchTraverse::Reset()
{
	if (m_pvecNumberdNodes)
	{
		delete m_pvecNumberdNodes;
		m_pvecNumberdNodes = NULL;
	}

	if(m_pvecMISIsoGraphs)
	{
		for(vector<CIsoSubGraph*>::iterator iter = m_pvecMISIsoGraphs->begin(); iter != m_pvecMISIsoGraphs->end(); iter++)
		{
			if(*iter)
			{
				delete *iter;
				*iter = NULL;
			}
		}
		delete m_pvecMISIsoGraphs;
	}
}

///////////////////////////////////////////////////////////////////////////////
int CSearchTraverse::SearchTraverse(Edge* pCrossEdge,TravePair* pTravHistPair)
{

    if (PRINT_TH)
        cout<<"Using cross-edge ("<<pCrossEdge->v1->GetID()<<","<<pCrossEdge->v2->GetID()<<")"<<endl;
    
	if(USE_NODES_NAME)//using nodes names
	{
		if((pCrossEdge->v1->m_strName ==  pTravHistPair->second->at(0)->m_strName) )
		{
			if((pCrossEdge->v2->m_strName ==  pTravHistPair->second->at(1)->m_strName))
			{
				//save the members
				m_pCrossEdge	= pCrossEdge;
				m_pTravHistPair = pTravHistPair;

				//reset graph Traverse
				m_pG->ResetTravNodes();
				m_pvecNumberdNodes->clear();
				pCrossEdge->v1->m_nTravNum = 1;
				m_pvecNumberdNodes->push_back(pCrossEdge->v1);
				pCrossEdge->v2->m_nTravNum = 2;
				m_pvecNumberdNodes->push_back(pCrossEdge->v2);

				return SearchVisit(2);
			}

		}
	}
	else//NOT using nodes names
	{
		//save the members
		m_pCrossEdge	= pCrossEdge;
		m_pTravHistPair = pTravHistPair;

		//reset graph Traverse
		m_pG->ResetTravNodes();
		m_pvecNumberdNodes->clear();
		pCrossEdge->v1->m_nTravNum = 1;
		m_pvecNumberdNodes->push_back(pCrossEdge->v1);
		pCrossEdge->v2->m_nTravNum = 2;
		m_pvecNumberdNodes->push_back(pCrossEdge->v2);

		return SearchVisit(2);

	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int CSearchTraverse::SearchVisit(int vtime)
{
	int nAnsCounter = 0;
	CNode* pV = m_pvecNumberdNodes->at(vtime-1);
	vector<CNode*>* pvecAdjNodes;
    int i;
	
	/*if(INDUCED_SUB_GRAPH)
		pvecAdjNodes = pV->GetAllAdjNodesOnlyNodes();
	else
		pvecAdjNodes = pV->GetAdjNodesOnlyNodes();

	vector<int>* pvecNodesNotZero = new vector<int>();
    vector<int>* testList = new vector<int>();
	for(vector<CNode*>::iterator Iter = pvecAdjNodes->begin();Iter != pvecAdjNodes->end(); Iter++) //Line 3
		if((*Iter)->m_nTravNum > 0)
        {
            pvecNodesNotZero->push_back((*Iter)->m_nTravNum);
            testList->push_back((*Iter)->m_nId);
        }

	delete pvecAdjNodes;
	pvecAdjNodes = NULL;

	sort(pvecNodesNotZero->begin(),pvecNodesNotZero->end());*/

    vector<int>* pvecNodesNotZero = new vector<int>();
    if(INDUCED_SUB_GRAPH)
		pvecNodesNotZero = pV->getAdjHistoriesBlackAndWhite(); /*black edges also*/
	else
		pvecNodesNotZero = pV->getAdjHistoriesWhite();


    // Print traverse history of a large graph subgraph
    if (PRINT_TH)
    {
       pV->Print(); 
    }

    if(!includes(pvecNodesNotZero->begin(),  //Natalia: replaced IsContained with includes() from <algorithm> package
                 pvecNodesNotZero->end(),
                 m_pTravHistPair->first->at(vtime-1)->begin(),
                 m_pTravHistPair->first->at(vtime-1)->end())) //Line 3
    
        //if(!IsContained(m_pTravHistPair->first->at(vtime-1),pvecNodesNotZero)) //Line 3
	{
		if(pvecNodesNotZero)
			delete pvecNodesNotZero;
		return 0;
	}

	//Line 4 , induced Sub Graph.
	if(INDUCED_SUB_GRAPH  && pvecNodesNotZero->size() != m_pTravHistPair->first->at(vtime-1)->size())
		return 0;

	if(pvecNodesNotZero)
			delete pvecNodesNotZero;

	//check if we have a full match
	if(vtime == (m_pTravHistPair->first->size())) //Line 6
	{             
#ifdef _DEBUG_MODE_
        if (algorithmRunning) {
            cout<<"vtime="<<vtime<<" TH size ="<<m_pTravHistPair->first->size()<<endl;
        // Prints current traverse history of the small graph;
        // Looks like prints it correctly
			cout<<"****************** match !! *******************************"<<endl;
			cout<< "cross edge: [" <<  m_pCrossEdge->v1->m_nId <<", "<<m_pCrossEdge->v1->m_strName<< "] ,";
			cout<< "[" <<  m_pCrossEdge->v2->m_nId <<", "<<m_pCrossEdge->v2->m_strName<< "]" << endl;

            cout<<"current Traverse History" << endl;
            i=1; int e=0;
            
            vector<CNode*>::iterator IterNode = m_pTravHistPair->second->begin();
            
            for(vector<vector<int>*>::iterator Iter2 = m_pTravHistPair->first->begin();
                Iter2 != m_pTravHistPair->first->end(); Iter2++, IterNode++)
				{
                    cout<<"Node "<<(*IterNode)->m_nId;
                    cout <<" N_" << i<<"=";
					for(vector<int>::iterator Iter3 =  (*Iter2)->begin();Iter3 != (*Iter2)->end(); Iter3++)
                    { cout << " " << (*Iter3); e++; }
                    i++;
					cout << endl;   
				}
            cout << "Total "<<e<<" edges"<<endl;
			cout<<"***********************************************************"<<endl;
        }
#endif

		//deal with the new subgraph found.
        if(m_bRetSubGraph && PRODUCE_MIS )
           BuildIsoSubGraph();

        //if (algorithmRunning)   
        // {
        //     PrintSubGraphTraversal();
        //     PrintSmallGraphTraversal();
        // }

		
		if(m_bRetSubGraph && !bFirstOcc)
		{
			TimeToFindFirstOcc = clock();
			bFirstOcc = true;
		}

		return 1; 
	}

	//choose a Node from H[vtime+1] FROM THE SUBGRAPH !
	//try to intersect all the Nodes adj to the Nodes in H[vtime+1] 
	//in order to get min group pvecAdjNodes.
	//changed by lior 10.9.06 
	vector<CNode*> *pvecIntersectAdjNodes,*pvecTempAdjNodes;
	CNode* pNode = m_pvecNumberdNodes->at(m_pTravHistPair->first->at(vtime)->at(0) - 1);
	pvecIntersectAdjNodes = pNode->GetAdjNodesOnlyNodes();

    if (algorithmRunning && PRINT_TH)
    {
        cout<<"large node "<<pNode->GetID()<<" has adj ";
        for(vector<CNode*>::iterator Iter = pvecIntersectAdjNodes->begin();Iter != pvecIntersectAdjNodes->end(); Iter++)
            cout<<(*Iter)->GetID()<<" ";
        cout<<endl;
    }

	int nCurrTravSize = (int)(m_pTravHistPair->first->at(vtime))->size();
	for(int i=1; i < nCurrTravSize ; i++)
	{
		pNode = m_pvecNumberdNodes->at(m_pTravHistPair->first->at(vtime)->at(i) - 1);
		pvecTempAdjNodes = pNode ->GetAdjNodesOnlyNodes();
		
		//pvecIntersectAdjNodes <= pvecIntersectAdjNodes INTERSECT pvecTempAdjNodes
		VecIntersect(pvecIntersectAdjNodes,pvecTempAdjNodes); 

		if(pvecTempAdjNodes)
			delete pvecTempAdjNodes;
	}
	pvecAdjNodes = new vector<CNode*>(pvecIntersectAdjNodes->begin(), pvecIntersectAdjNodes->end());
	
	if(pvecIntersectAdjNodes)
		delete pvecIntersectAdjNodes;

	//for each Node w such that w is Adj to pV2 and g(w)==0 do
	for(vector<CNode*>::iterator Iter = pvecAdjNodes->begin();Iter != pvecAdjNodes->end(); Iter++)
	{
		if((*Iter)->m_nTravNum == 0)
		{
			if( (INDUCED_SUB_GRAPH &&
                 (m_pG->GetEdgeType(*Iter,pNode) == EDGE_TYPE_WHITE))  || (!INDUCED_SUB_GRAPH) )//Induced sub graph 
			{
				if(USE_NODES_NAME) //using node names
				{
					if( ((*Iter)->m_strName ==  m_pTravHistPair->second->at(vtime)->m_strName)) 
						//(m_pTravHistPair->second->at(vtime)->m_strName == NODE_NAME_EVERY_POS_NANE) ||
						//((*Iter)->m_strName == NODE_NAME_EVERY_POS_NANE ))
					{
						//set g(w) = vtime+1
						(*Iter)->m_nTravNum = vtime+1; 
						m_pvecNumberdNodes->push_back(*Iter);

						//Rec call
						nAnsCounter += SearchVisit(vtime+1);

						//set g(w) = 0
						(*Iter)->m_nTravNum = 0; 
						m_pvecNumberdNodes->pop_back();
					}
				}
				else//NOT using nodes names
				{

					//set g(w) = vtime+1
					(*Iter)->m_nTravNum = vtime+1; 
					m_pvecNumberdNodes->push_back(*Iter);

					//Rec call
					nAnsCounter += SearchVisit(vtime+1);

					//set g(w) = 0
					(*Iter)->m_nTravNum = 0; 
					m_pvecNumberdNodes->pop_back();
				}
			}

			 
			
		}
	}

	delete pvecAdjNodes;
	pvecAdjNodes = NULL;
	return nAnsCounter;
}

///////////////////////////////////////////////////////////////////////////////
// the funtion will return true iff (pVec1 C pVec2). ( C == Contained);
// vectors are sorted
bool CSearchTraverse::IsContained(vector<int>* pVec1,vector<int>* pVec2)
{
      if (algorithmRunning && PRINT_TH)
        {
          cout<<"vec1 ";
          for(vector<int>::iterator Iter = pVec1->begin();
                   Iter != pVec1->end(); Iter++)
			   cout << " " <<*Iter; 
          cout <<"   ";

          cout<<"vec2 ";
          for(vector<int>::iterator Iter = pVec2->begin();
                   Iter != pVec2->end(); Iter++)
			   cout << " " <<*Iter; 
          }
    
	vector<int>::iterator IterV1 = pVec1->begin();
	for(vector<int>::iterator IterV2 = pVec2->begin();IterV2 != pVec2->end(); IterV2++)
	{        
		if(IterV1 == pVec1->end())
        {
            return true;
        }

		if(*IterV1 < *IterV2 )
        {
            return false;
        }

		if(*IterV1 == *IterV2 )
			IterV1++;

	}

	if(IterV1 == pVec1->end()) //size of v1 == v2
    {
        return true;
    }

	return false;
}
///////////////////////////////////////////////////////////////////////////////
bool CSearchTraverse::Init(CGraph* pLGraph)
{
	m_pG = pLGraph;
	m_pvecNumberdNodes = new vector<CNode*>();
	
	if(PRODUCE_MIS &&  m_bRetSubGraph) 
		m_pvecMISIsoGraphs = new vector<CIsoSubGraph*>();
	

	return true;
}

///////////////////////////////////////////////////////////////////////////////
//pvecC1 <= pvecC1 INTERSECTED pvecC2.
void CSearchTraverse::VecIntersect(vector<CNode*>* pvecC1,vector<CNode*>*  pvecC2)
{
	vector<CNode*> vecIntersect;
	for(vector<CNode*>::iterator IterC1=pvecC1->begin(); IterC1!=pvecC1->end(); IterC1++)
	{
		for(vector<CNode*>::iterator IterC2=pvecC2->begin(); IterC2!=pvecC2->end(); IterC2++)
		{
			if(**IterC1 == **IterC2)
			{
				vecIntersect.push_back(*IterC1);
				break;
			}
		}
	}
	
	pvecC1->assign(vecIntersect.begin(),vecIntersect.end());
}

///////////////////////////////////////////////////////////////////////////////
//Build the IsoSubGraphs for the MIS
//add by Lior 18.9.06
void CSearchTraverse::BuildIsoSubGraph()
{
	CIsoSubGraph* pIsoGrapg = new CIsoSubGraph();
	int nSourceNode = 0;


    //cout<<"BuildIsoSubGraph 1"<<endl;
	for(vector<vector<int>*>::iterator TravIter = m_pTravHistPair->first->begin();TravIter != m_pTravHistPair->first->end(); TravIter++)
	{
		for(vector<int>::iterator IntTravIter = (*TravIter)->begin();IntTravIter != (*TravIter)->end(); IntTravIter++)
		{
			pIsoGrapg->AddEdge(m_pvecNumberdNodes->at(nSourceNode),m_pvecNumberdNodes->at(*IntTravIter-1));
		}
		nSourceNode++;

	}

    //cout<<"BuildIsoSubGraph 2"<<endl;

	bool bFound = false;
	for(vector<CIsoSubGraph*>::iterator iter = m_pvecMISIsoGraphs->begin();iter != m_pvecMISIsoGraphs->end(); iter++)
	{
		if(*pIsoGrapg == **iter)
		{
			bFound = true;
			break;
		}
	}
    
    //cout<<"BuildIsoSubGraph 3 bFound="<<bFound<<endl;

	if(bFound == false)
		m_pvecMISIsoGraphs->push_back(pIsoGrapg);
	else
		if(pIsoGrapg)
		{
			delete pIsoGrapg;
			pIsoGrapg = NULL;
		}
    //cout<<"BuildIsoSubGraph 4"<<endl;
    pIsoGrapg->Print();
}

///////////////////////////////////////////////////////////////////////////////
// Print isomorphic subgraph found: Natalia, for debugging
void CSearchTraverse::PrintIsoSubGraph()
{
	CIsoSubGraph* pIsoGrapg = new CIsoSubGraph();
	int nSourceNode = 0;


    for(vector<vector<int>*>::iterator TravIter = m_pTravHistPair->first->begin();TravIter != m_pTravHistPair->first->end(); TravIter++)
	{
		for(vector<int>::iterator IntTravIter = (*TravIter)->begin();IntTravIter != (*TravIter)->end(); IntTravIter++)
		{
			pIsoGrapg->AddEdge(m_pvecNumberdNodes->at(nSourceNode),m_pvecNumberdNodes->at(*IntTravIter-1));
		}
		nSourceNode++;

	}
    pIsoGrapg->Print();
}

///////////////////////////////////////////////////////////////////////////////
// Print isomorphic subgraph traversal: Natalia, for debugging
void CSearchTraverse::PrintSubGraphTraversal()
{
	CIsoSubGraph* pIsoGrapg = new CIsoSubGraph();
	int nSourceNode = 0;

    cout<<"***************** Subgraph traversal: *****************"<<endl;
    for(vector<vector<int>*>::iterator TravIter = m_pTravHistPair->first->begin();TravIter != m_pTravHistPair->first->end(); TravIter++)
	{
        cout<<"LN_"<<nSourceNode+1<<"("<<m_pvecNumberdNodes->at(nSourceNode)->GetID()<<")=";
        
		for(vector<int>::iterator IntTravIter = (*TravIter)->begin();IntTravIter != (*TravIter)->end(); IntTravIter++)
		{
            cout<<" "<<m_pvecNumberdNodes->at(*IntTravIter-1)->m_nTravNum<<"("<<m_pvecNumberdNodes->at(*IntTravIter-1)->GetID()<<")";
		}
		nSourceNode++;
        cout<<endl;

	}
}

///////////////////////////////////////////////////////////////////////////////
// Print traversal of the small graph: Natalia, for debugging
void CSearchTraverse::PrintSmallGraphTraversal()
{
    cout<<"##################### Small graph traversal: #####################"<<endl;
            int i=1; int e=0;
            
            vector<CNode*>::iterator IterNode = m_pTravHistPair->second->begin();
            
            for(vector<vector<int>*>::iterator Iter2 = m_pTravHistPair->first->begin();
                Iter2 != m_pTravHistPair->first->end(); Iter2++, IterNode++)
				{
                    cout<<"Node "<<(*IterNode)->m_nId;
                    cout <<" SN_" << i<<"=";
					for(vector<int>::iterator Iter3 =  (*Iter2)->begin();Iter3 != (*Iter2)->end(); Iter3++)
                    { cout << " " << (*Iter3); e++; }
                    i++;
					cout << endl;   
				}
           
	cout<<"###################################################################"<<endl;
}

///////////////////////////////////////////////////////////////////////////////
vector<CIsoSubGraph*>* CSearchTraverse::GetIsoSubGraph()
{
	return m_pvecMISIsoGraphs;
}

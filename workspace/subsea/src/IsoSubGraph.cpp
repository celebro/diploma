#include "IsoSubGraph.h"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
CIsoSubGraph::CIsoSubGraph()
{
	m_pvecLightEdge = new vector<LightEdge*>();
}


///////////////////////////////////////////////////////////////////////////////
CIsoSubGraph::~CIsoSubGraph()
{
	if(m_pvecLightEdge)
	{
		for(vector<LightEdge*>::iterator iter = m_pvecLightEdge->begin(); iter != m_pvecLightEdge->end(); iter++)
		{
			if(*iter)
			{
				delete *iter;
				*iter = NULL;
			}
		}
		
		delete m_pvecLightEdge; 
		m_pvecLightEdge = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
void CIsoSubGraph::AddEdge(CNode* V1, CNode* V2)
{
	LightEdge* pNewEdge = new LightEdge(V1,V2);

	bool bUnique = true;
	for(vector<LightEdge*>::iterator iter = m_pvecLightEdge->begin(); iter != m_pvecLightEdge->end(); iter++)
	{
		if(*pNewEdge  == **iter )
		{
			bUnique = false;
			break;
		}
	}

	if(bUnique)
		m_pvecLightEdge->push_back(pNewEdge);
}

///////////////////////////////////////////////////////////////////////////////
void CIsoSubGraph::Print()
{
    cout<<"***************** Subgraph found: *****************"<<endl;
	for(vector<LightEdge*>::iterator iter = m_pvecLightEdge->begin(); iter != m_pvecLightEdge->end(); iter++)
	{
        LightEdge l = **iter;
		cout<<"("<<l.v1->GetID()<<","<<l.v2->GetID()<<")";
        if (iter+1 != m_pvecLightEdge->end())
            cout<<", ";
	}
    cout<<endl;

}

///////////////////////////////////////////////////////////////////////////////
bool CIsoSubGraph::IsIntersect(CIsoSubGraph* other)
{
	for(vector<LightEdge*>::iterator this_iter = m_pvecLightEdge->begin(); this_iter  != m_pvecLightEdge->end(); this_iter ++)
	{
		for(vector<LightEdge*>::iterator other_iter = other->m_pvecLightEdge->begin(); other_iter != other->m_pvecLightEdge->end(); other_iter++)
		{
			if(**this_iter == **other_iter)
			{
				return true;
			}
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
bool CIsoSubGraph::operator==(const CIsoSubGraph& other)
{
	bool bFound = false;

	for(vector<LightEdge*>::iterator this_iter = m_pvecLightEdge->begin(); this_iter  != m_pvecLightEdge->end(); this_iter ++)
	{
		bFound = false;
		for(vector<LightEdge*>::iterator other_iter = other.m_pvecLightEdge->begin(); other_iter != other.m_pvecLightEdge->end(); other_iter++)
		{
			if(**this_iter == **other_iter)
			{
				bFound = true;
				break;
			}
		}

		if(bFound == false)
			return false;
	}

	for(vector<LightEdge*>::iterator other_iter = other.m_pvecLightEdge->begin(); other_iter != other.m_pvecLightEdge->end(); other_iter++)
	{
		bFound = false;
		for(vector<LightEdge*>::iterator this_iter = m_pvecLightEdge->begin(); this_iter  != m_pvecLightEdge->end(); this_iter ++)
		{
			if(**this_iter == **other_iter)
			{
				bFound = true;
				break;
			}
		}

		if(bFound == false)
			return false;
	}

	return true;


}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


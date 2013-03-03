#include "Graph.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

extern bool ourFormat;
extern int  NumberOfLabels;

///////////////////////////////////////////////////////////////////////////////
CGraph::~CGraph()
{
	for (vector<CNode*>::iterator iter = m_vecNodes.begin(); iter != m_vecNodes.end(); iter++)
		if (*iter)
			delete *iter;

	
	m_mapNodes.clear();
}

///////////////////////////////////////////////////////////////////////////////
void CGraph::add_to_list(TextList **l, char *element)
{
	TextList *e;
	if (*l == NULL)
	{
		(*l)= (TextList *)malloc(sizeof(TextList));
		(*l)->str = (char *)malloc(strlen(element)+1);
		strcpy((*l)->str, element);
		(*l)->next = NULL;
	}
	else
		if ((*l)->next!= NULL)
			add_to_list(&(*l)->next, element);
		else
		{
			e = (TextList *)malloc(sizeof(TextList));
			e->str = (char *)malloc(strlen(element)+1);
			strcpy(e->str, element);
			e->next = NULL;
			(*l)->next = e;
		}
}

/* Function parse() parses a string of characters into parts separated
by one of the delimiters until one of forbidden characters is found.
Parameters: str - a string to parse,
delimiters - a string of possible delimiters,
forbidden - a string of characters on which the parsing stops.
Return value: a linked list of tokens. */
///////////////////////////////////////////////////////////////////////////////
TextList* CGraph::Parse(char *str, char *delimiters, char *forbidden)
{
	char *token;
	TextList *l=NULL;
	char *s, *str2;

	str2 = (char *)malloc(strlen(str)+1);
	strcpy(str2, str);
	s=strtok(str2, forbidden);

	token = strtok(s, delimiters);

	while (token != NULL)
	{
		add_to_list(&l, token);
		token = strtok(NULL, delimiters);
	}

	return l;
}

///////////////////////////////////////////////////////////////////////////////
bool CGraph::InitFromFile(const string& strInitFilename)
{
	static const int buff_size = 64;
	static char buff[buff_size];

    //cout<<"Reading file"<<strInitFilename<<endl;
    
	FILE* pFile = fopen(strInitFilename.c_str(), "r");
	if (!pFile)
		return false;

    //printf("InitFromFile file opened\n");

    if (!ourFormat)
        ReadWholeGraph(pFile);
    else
    	while (!feof(pFile))
        	{
                fgets(buff, buff_size, pFile);
                if (!AddToGraph(Parse(buff, " ", "\t\0\n")))
                {
                    m_vecNodes.clear();
                    m_mapNodes.clear();
                    return false;
                }
            }

	if (pFile)
		fclose(pFile);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
EDGE_TYPE CGraph::GetEdgeType(CNode* pV1, CNode* pV2)
{
	vector<AdjNode*>::iterator iter;

	if (pV1->m_vecBlackAdj.size() < pV2->m_vecBlackAdj.size())
	{
		for (iter = pV1->m_vecBlackAdj.begin(); iter != pV1->m_vecBlackAdj.end(); iter++)
			if (*(*iter)->pNode == *pV2)
				return EDGE_TYPE_BLACK;
	}
	else
	{
		for (iter = pV2->m_vecBlackAdj.begin(); iter != pV2->m_vecBlackAdj.end(); iter++)
			if (*(*iter)->pNode == *pV1)
				return EDGE_TYPE_BLACK;
	}

	if (pV1->m_vecAdj.size() < pV2->m_vecAdj.size())
	{
		for (iter = pV1->m_vecAdj.begin(); iter != pV1->m_vecAdj.end(); iter++)
			if (*(*iter)->pNode == *pV2)
				return EDGE_TYPE_WHITE;
	}
	else
	{
		for (iter = pV2->m_vecAdj.begin(); iter != pV2->m_vecAdj.end(); iter++)
			if (*(*iter)->pNode == *pV1)
				return EDGE_TYPE_WHITE;
	}

	return EDGE_TYPE_NO_EDGE;
}

///////////////////////////////////////////////////////////////////////////////
string CGraph::CutCarReturn(string str)
{
	basic_string <char>::size_type size = str.find("\n");
	return str.substr(0, size);
}

///////////////////////////////////////////////////////////////////////////////
/* WARNING: for simplicity this code does not check for End-Of-File! */
unsigned short int CGraph::read_word(FILE *in, bool *end)
  {
    unsigned char b1, b2;
    
    b1=getc(in); /* Least-significant Byte */
    b2=getc(in); /* Most-significant Byte */

    if (b1==EOF || b2==EOF)
        *end = true;

    return b1 | (b2 << 8);
  }

/* This function assumes that the adjacency matrix is statically allocated.
 * The return value is the number of nodes in the graph.
 * If number of labels specified is larger than zero, labels are added.
 * Note: replace with label generation and reading later
 */
bool CGraph::ReadWholeGraph(FILE *in)
  { unsigned short int  nodes;
    unsigned short int  edges = 0;
    unsigned short int  target;
    int i, j;
    int total_edges = 0;
    bool end = false;
    string label;

    /* Read the number of nodes */
    nodes = read_word(in, &end);
#ifdef _DEBUG_MODE_    
    printf("ReadWholeGraph on %d nodes\n", nodes);
#endif

    /* Create nodes*/
    for (i=0; i<nodes; i++)
    {
        label = "a";
        if (NumberOfLabels>0)
            label += i%NumberOfLabels;
		CNode* pNode = new CNode(label, i);
		m_vecNodes.push_back(pNode);
		m_mapNodes.insert(HashPair(pNode->GetID(), (CNode*)m_vecNodes.back()));
	}
#ifdef _DEBUG_MODE_
    printf("Created %d nodes\n", nodes);
#endif

    /* For each node i ... */
    for(i=0; i<nodes; i++)
      { 
        /* Read the number of edges coming out of node i */
          edges=read_word(in, &end);
        if (end) return true;
        
        //printf("Node %d has %d outgoing edges\n",i, edges);
        /* For each edge out of node i... */
        for(j=0; j<edges; j++)
          { 
            /* Read the destination node of the edge */
              target = read_word(in, &end);
            if (end) return true;
            total_edges++;

			hash_map<int, CNode*>::iterator iterSource = m_mapNodes.find(i);
			hash_map<int, CNode*>::iterator iterTarget = m_mapNodes.find(target);

            //printf("Found edge (%d,%d)\n", i, target);
			
			if ((iterSource == m_mapNodes.end()) || (iterTarget == m_mapNodes.end()))
				return false;

			CNode* Sor = (iterSource->second);
			CNode* Tar = (iterTarget->second);

			if (!Sor->IsConnected(Tar))
				Sor->AddAdjNode(Tar, "a");

			if (!Tar->IsConnected(Sor))
				Tar->AddAdjNode(Sor, "a");
          }
      }
#ifdef _DEBUG_MODE_      
            printf("ReadWholeGraph finished total_edges=%d\n",total_edges);
#endif
    return true;
  }

///////////////////////////////////////////////////////////////////////////////
bool CGraph::AddToGraph(TextList* T)
{
	static bool bAllNodesInitialized = false;
	string strName, strId;

	if (strcmp(T->str, "node") == 0)
	{
		TextList* NextT = T->next;
		TextList* NextId = NextT->next;
		
		strName = NextT->str;
		strId = NextId->str;

		strName = CutCarReturn(strName);
		strId = CutCarReturn(strId);
		CNode* pNode = new CNode(strName, atoi(strId.c_str()));

		m_vecNodes.push_back(pNode);
		m_mapNodes.insert(HashPair(pNode->GetID(), (CNode*)m_vecNodes.back()));

		free(NextId->str);
		free(NextId);
		free(NextT->str);
		free(NextT);
		free(T->str);
		free(T);
	}
	else
	{
		if (strcmp(T->str, "edge") == 0)
		{
			TextList* SourceVertex = T->next;
			TextList* TargetVertex = SourceVertex->next;
			TextList* EdgeName = TargetVertex->next;

			string strTemp = SourceVertex->str;
			strTemp = CutCarReturn(strTemp);
			hash_map<int, CNode*>::iterator iterSource = m_mapNodes.find(atoi(strTemp.c_str()));

			strTemp = TargetVertex->str;
			strTemp = CutCarReturn(strTemp);
			hash_map<int, CNode*>::iterator iterTarget = m_mapNodes.find(atoi(strTemp.c_str()));
			
			if ((iterSource == m_mapNodes.end()) || (iterTarget == m_mapNodes.end()))
				return false;

			CNode* Sor = (iterSource->second);
			CNode* Tar = (iterTarget->second);

			if (!Sor->IsConnected(Tar))
				Sor->AddAdjNode(Tar, CutCarReturn(EdgeName->str));

			if (!Tar->IsConnected(Sor))
				Tar->AddAdjNode(Sor, CutCarReturn(EdgeName->str));
			
			free(EdgeName->str);
			free(EdgeName);
			free(TargetVertex->str);
			free(TargetVertex);
			free(SourceVertex->str);
			free(SourceVertex);
		}
		else
		{
			free(T->str);
			free(T);
			return true;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
void CGraph::PrintNodes()
{
	for (vector<CNode*>::iterator iter = m_vecNodes.begin(); iter != m_vecNodes.end(); iter++)
	{
		printf("%s: ", (*iter)->m_strName.c_str());
		for (vector<AdjNode*>::iterator adj_iter = (*iter)->m_vecAdj.begin(); 
			adj_iter != (*iter)->m_vecAdj.end(); adj_iter++)
		{
			printf("%s %s", (*adj_iter)->pNode->m_strName.c_str(), (*adj_iter)->strEdgeName.c_str());
		}
		printf("\n");
	}
}

///////////////////////////////////////////////////////////////////////////////
bool CGraph::GetAdjNodes(int nNodeId, vector<AdjNode*>& rAdjNodes)
{
	hash_map<int, CNode*>::iterator iter = m_mapNodes.find(nNodeId);
	if (iter == m_mapNodes.end())
		return false;

	rAdjNodes = (iter->second)->m_vecAdj;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool CGraph::ResetTravNodes()
{
	vector<CNode*>::iterator iter;

	for (iter = m_vecNodes.begin(); iter != m_vecNodes.end(); iter++)
		(*iter)->m_nTravNum = 0;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
int CGraph::GetNumOfEdges()
{
	int nNumOfEdges = 0;
	vector<CNode*>::iterator iter;

	for (iter = m_vecNodes.begin(); iter != m_vecNodes.end(); iter++)
		nNumOfEdges += (*iter)->GetNumOfAdjNodes();

	return nNumOfEdges/2;
}

///////////////////////////////////////////////////////////////////////////////
vector<CNode*>* CGraph::GetAllNodes()
{
	vector<CNode*>* pvecRetVal = new vector<CNode*>();
	vector<CNode*>::iterator iter;

	for (iter = m_vecNodes.begin(); iter != m_vecNodes.end(); iter++)
		pvecRetVal->push_back(*iter);

	return pvecRetVal;
}

///////////////////////////////////////////////////////////////////////////////
bool CGraph::RemoveEdge(Edge* pEdge)
{
	if (!pEdge)
		return false;

	hash_map<int, CNode*>::iterator iterV1 = m_mapNodes.find(pEdge->v1->GetID());
	hash_map<int, CNode*>::iterator iterV2 = m_mapNodes.find(pEdge->v2->GetID());

	if ((iterV1 == m_mapNodes.end()) || (iterV2 == m_mapNodes.end()))
		return false;

	CNode* v1 = iterV1->second;
	CNode* v2 = iterV2->second;

	if (!v1 || !v2)
		return false;

	return v1->RemoveAdjNode(v2) && v2->RemoveAdjNode(v1);
}

///////////////////////////////////////////////////////////////////////////////
int CGraph::GetNumOfNodes()
{
	return (int)m_vecNodes.size();
}

///////////////////////////////////////////////////////////////////////////////
//Added by lior 27.8.06 12:40
//clear all the bfs in the nodes to be white
void CGraph::ResetBFSColor()
{
	vector<CNode*>::iterator iter;

	for (iter = m_vecNodes.begin(); iter != m_vecNodes.end(); iter++)
		(*iter)->m_BfsColor = BFS_COLOR_WHITE;
}

///////////////////////////////////////////////////////////////////////////////
void CGraph::Reset()
{
	for (vector<CNode*>::iterator iter = m_vecNodes.begin(); iter != m_vecNodes.end(); iter++)
		(*iter)->Reset();
}

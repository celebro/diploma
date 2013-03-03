#ifndef _BISECTION_H_
#define _BISECTION_H_


#include "Graph.h"
//This function find a "min" cut,in L_Graph, such that the two cut's are in the same size (vertex). 
//the ans is return by the two args LeftCut and RightCut.
void CreateBiSection(CSubGraph& L_Graph ,CSubGraph* pLeftCut,CSubGraph* pRightCut);
void CreateBiSection_Version2(CSubGraph& L_Graph ,CSubGraph* pLeftCut,CSubGraph* pRightCut);

///////////////////////////////////////// TESTING ///////////////////////////////
//void Add2Hole(vector<CNode*>& vecNodeToSplit,vector<CNode*>* pvecCurrCut);
//void ChooseRandNodeNoEdge(vector<CNode*>& vecNodeToSplit, vector<CNode*>* pvecCurrCut);
//void ChooseRandNodeFromEdges(vector<CNode*>& vecAdjNodes, vector<CNode*>& vecNodeToSplit, vector<CNode*>* pvecCurrCut);
int RandNum(int nUpperBound);
vector<CNode*>* GetAllAdjNodes(vector<CNode*>& vecNodeToSplit, vector<CNode*>* pvecCurrCut);
//bool GreedyBisectionImprove(vector<CNode*>* pvecCut1,vector<CNode*>* pvecCut2);
///////////////////////////////////////// TESTING END  //////////////////////////


#endif //_BISECTION_H_
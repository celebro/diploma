#include "MISFileCreator.h"
#include <cstdio>

///////////////////////////////////////////////////////////////////////////////
bool CMISFileCreator::WriteToFile(vector<CIsoSubGraph*>* G, string str)
{
	FILE* fd;
	fd = fopen(str.c_str(),"w");

	if (!fd)
	{
		printf("Failed to open file %s\n", str.c_str());
		return false;
	}

	int nEdgeCounter = 0;

	for(vector<CIsoSubGraph*>::iterator iter1 = G->begin();iter1 != G->end();iter1++)
	{
		for(vector<CIsoSubGraph*>::iterator iter2 = G->begin();iter2 != G->end();iter2++)
		{
			if(*iter1 != *iter2)
			{
				if(!(*iter1)->IsIntersect(*iter2))
					nEdgeCounter++;

			}
		}
	}

	fprintf(fd, "%d %d\n", G->size(), nEdgeCounter/2);
	vector<int> vecAdjNodes;
	int nSecond = 0;

	for(vector<CIsoSubGraph*>::iterator iter1 = G->begin();iter1 != G->end();iter1++)
	{
		vecAdjNodes.clear();
		nSecond = 0;

		for(vector<CIsoSubGraph*>::iterator iter2 = G->begin();iter2 != G->end();iter2++)
		{
			if(*iter1 != *iter2)
				if(!(*iter1)->IsIntersect(*iter2))
					vecAdjNodes.push_back(nSecond);

			nSecond++;
		}
		fprinttVec(fd,vecAdjNodes);
	}




	if (fd)
		fclose(fd);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
void CMISFileCreator::fprinttVec(FILE* fd,vector<int>& vecAdjNodes)
{
	fprintf(fd,"1 %d",vecAdjNodes.size());
	for(vector<int>::iterator iter =  vecAdjNodes.begin();iter !=  vecAdjNodes.end();iter++)
		fprintf(fd," %d",*iter);
	fprintf(fd,"\n");



}

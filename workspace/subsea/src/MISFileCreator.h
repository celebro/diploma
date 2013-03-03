#ifndef _CMIS_FILE_CREATOR_H_
#define _CMIS_FILE_CREATOR_H_

#include <string>
#include <vector>
#include "IsoSubGraph.h"

///////////////////////////////////////////////////////////////////////////////
class CMISFileCreator
{

public:
	bool WriteToFile(vector<CIsoSubGraph*>* G, string str);
	 
private:
	void fprinttVec(FILE* fd,vector<int>& vecAdjNodes);

};


#endif //_CMIS_FILE_CREATOR_H_
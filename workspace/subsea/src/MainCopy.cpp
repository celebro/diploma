#include "SubSeaAlgo.h"
#include <iostream>
#include <fstream>
#include <time.h>
#include <string>
#include "Def.h"
#include <ctime>

#include "wclique1.h"
using namespace std;

clock_t TimeToFindFirstOcc,TimeToFindAllOcc,TimeBeginMIS,TimeEndMIS;  //statistic Timer
int gNumOfBisectionCut=0,gMaxCutFound=0,gTotalCutWeight=0;
bool bFirstOcc = false;
ofstream outputStream;


#ifndef WIN32
	#include "sys/times.h"
	#include "time.h"
	struct tms TMS;
	clock_t tu_begin,ts_begin;
	clock_t tu_end,ts_end;
#endif


int main(int argc, char** argv)
{
	char c = 'y';
	CSubSeaAlgo Algo;
	string StrLargeFileName,StrSmallFileName;
	pair<int,int> RetVal;
	clock_t T1;

	cout<<"Hi";
	

	 ifstream myfileLarge ("c:/large.txt");
	 ifstream myfileSmall ("c:/small.txt");
	  
	 if (myfileLarge.is_open() && myfileSmall.is_open())
	  {
		  while (! myfileLarge.eof() && !myfileSmall.eof())
		{
		  getline (myfileSmall,StrSmallFileName);
		  getline (myfileLarge,StrLargeFileName);
		  cout << StrLargeFileName << " "<<StrSmallFileName;
		 // calcAlg(lineLarge,lineSmall);


		  if(SAVE_STATISTIC_TO_FILE)
			outputStream << StrLargeFileName << "\t" << StrSmallFileName << "\t";

		if (Algo.Init(StrSmallFileName, StrLargeFileName))
		{
			if(SAVE_STATISTIC_TO_FILE)
				outputStream <<Algo.GetNumOfNodes(false)<<"\t"<<Algo.GetNumOfEdges(false)
				<<"\t"<<Algo.GetNumOfNodes(true)<<"\t"<<Algo.GetNumOfEdges(true)<<"\t";

			T1 =clock();
			RetVal = Algo.Run();
		
			cout << "*****************************************"<<endl;
			cout <<"Number of IsoMorphism found: " <<RetVal.first << endl;
			if(SAVE_STATISTIC_TO_FILE)
				outputStream << RetVal.first << "\t";

			if(PRODUCE_MIS)
					cout <<"Maximum Independent Set size: " <<RetVal.second<< endl;
				cout << "*****************************************"<<endl;
			if(STATISTIC_INFO)
			{
				cout<<"Max Cut (Bisection) weight: "<< gMaxCutFound <<endl;
				cout<<"Avg Cut (Bisection) weight: "<< (double)gTotalCutWeight/gNumOfBisectionCut <<endl;
				cout<<"Total num of cuts that were made: "<< gNumOfBisectionCut<<endl;
				cout << "Time to find the FIRST occurrence in Sec: " << double(TimeToFindFirstOcc - T1) / CLOCKS_PER_SEC << endl;
				cout << "Time to find the ALL occurrence in Sec: " << double(TimeToFindAllOcc - T1) / CLOCKS_PER_SEC << endl;
				if(PRODUCE_MIS)
					cout<<"Time to find MIS in Sec: " << double(TimeEndMIS - TimeBeginMIS) / CLOCKS_PER_SEC << endl;
				#ifndef WIN32
					cout <<"User Time (excluding MIS): " << double(tu_end - tu_begin)/ 100.0 << endl;
					cout <<"System (Kernel) Time (excluding MIS): " << double(ts_end - ts_begin)/ 100.0<< endl;
				#endif
			}
			if(SAVE_STATISTIC_TO_FILE)
				outputStream << double(clock() - T1) / CLOCKS_PER_SEC << "\t";
				cout <<"Total Runing Time in Sec: " << double(clock() - T1) / CLOCKS_PER_SEC << endl << endl;
		
		}
	}
	else
	{
		cout << "Init fail - in vaild file name or content"<<endl;
		exit(0);
	}

		return 0;
}
int main1(int argc, char** argv) 
{
	if(SAVE_STATISTIC_TO_FILE)
	{
		//ofstream outputStream;
		outputStream.open("Statistic.txt",ios_base::out|ios_base::app);
		if( !outputStream) 
		{
			cerr << "Error opening input stream" << endl;
			return 0;
		}
	}              



	char c = 'y';
	CSubSeaAlgo Algo;
	string StrLargeFileName,StrSmallFileName;
	pair<int,int> RetVal;
	clock_t T1;
	
	cout<<"///////////////////////////////////////////////////////////////////////////////"<<endl;
	cout<<"/////////////////////////////////  SubSeaAlgo  ////////////////////////////////" << endl;
	cout<<"///////////////////////////////////////////////////////////////////////////////"<<endl << endl;

	//LargeGraph Name, SmallGraph Name,LargeGraph Nodes,  SmallGraph Nodes
	if(argc < 3)
	{
		cout<< "Insert Large graprh name: ";
		cin >> StrLargeFileName;
		cout<< "Insert Small Patern graph name: ";
		cin >> StrSmallFileName;
	}
	else
	{
		StrSmallFileName = argv[1];
		StrLargeFileName = argv[2];
		
	}



	if(SAVE_STATISTIC_TO_FILE)
		outputStream << StrLargeFileName << "\t" << StrSmallFileName << "\t";

	if (Algo.Init(StrSmallFileName, StrLargeFileName))
	{
		if(SAVE_STATISTIC_TO_FILE)
			outputStream <<Algo.GetNumOfNodes(false)<<"\t"<<Algo.GetNumOfEdges(false)
			<<"\t"<<Algo.GetNumOfNodes(true)<<"\t"<<Algo.GetNumOfEdges(true)<<"\t";

		T1 =clock();
		RetVal = Algo.Run();
		
		cout << "*****************************************"<<endl;
		cout <<"Number of IsoMorphism found: " <<RetVal.first << endl;
		if(SAVE_STATISTIC_TO_FILE)
			outputStream << RetVal.first << "\t";

		if(PRODUCE_MIS)
				cout <<"Maximum Independent Set size: " <<RetVal.second<< endl;
		cout << "*****************************************"<<endl;
		if(STATISTIC_INFO)
		{
			cout<<"Max Cut (Bisection) weight: "<< gMaxCutFound <<endl;
			cout<<"Avg Cut (Bisection) weight: "<< (double)gTotalCutWeight/gNumOfBisectionCut <<endl;
			cout<<"Total num of cuts that were made: "<< gNumOfBisectionCut<<endl;
			cout << "Time to find the FIRST occurrence in Sec: " << double(TimeToFindFirstOcc - T1) / CLOCKS_PER_SEC << endl;
			cout << "Time to find the ALL occurrence in Sec: " << double(TimeToFindAllOcc - T1) / CLOCKS_PER_SEC << endl;
			if(PRODUCE_MIS)
				cout<<"Time to find MIS in Sec: " << double(TimeEndMIS - TimeBeginMIS) / CLOCKS_PER_SEC << endl;
			#ifndef WIN32
				cout <<"User Time (excluding MIS): " << double(tu_end - tu_begin)/ 100.0 << endl;
				cout <<"System (Kernel) Time (excluding MIS): " << double(ts_end - ts_begin)/ 100.0<< endl;
			#endif
		}
		if(SAVE_STATISTIC_TO_FILE)
			outputStream << double(clock() - T1) / CLOCKS_PER_SEC << "\t";
		cout <<"Total Runing Time in Sec: " << double(clock() - T1) / CLOCKS_PER_SEC << endl << endl;
		
	}
	else
	{
		cout << "Init fail - in vaild file name or content"<<endl;
		exit(0);
	}

	cout << "Continue? y/n: ";
	cin >> c ;
	cout << endl;

	while(c == 'y')
	{
		//statistic Reset
		gNumOfBisectionCut=0;
		gMaxCutFound=0;
		gTotalCutWeight=0;
		bFirstOcc = false;

		cout<< "Insert Small Patern graph name: ";
		cin >> StrSmallFileName;
		if (Algo.Init(StrSmallFileName))
		{
			T1 =clock();
			RetVal= Algo.Run();
		
			cout << "*****************************************"<<endl;
			cout <<"Number of IsoMorphism found: " <<RetVal.first<< endl;
			if(PRODUCE_MIS)
				cout <<"Maximum Independent Set size: " <<RetVal.second<< endl;
			cout << "*****************************************"<<endl;
			if(STATISTIC_INFO)
			{
				cout<<"Max Cut (Bisection) weight: "<< gMaxCutFound <<endl;
				cout<<"Avg Cut (Bisection) weight: "<< (double)gTotalCutWeight/gNumOfBisectionCut <<endl;
				cout<<"Total num of cuts that were made: "<< gNumOfBisectionCut<<endl;
				cout << "Time to find the FIRST occurrence in Sec: " << double(TimeToFindFirstOcc - T1) / CLOCKS_PER_SEC << endl;
				cout << "Time to find the ALL occurrence in Sec: " << double(TimeToFindAllOcc - T1) / CLOCKS_PER_SEC << endl;
				if(PRODUCE_MIS)
					cout<<"Time to find MIS in Sec: " << double(TimeEndMIS - TimeBeginMIS) / CLOCKS_PER_SEC << endl;
					
				#ifndef WIN32
					cout <<"User Time (excluding MIS): " << double(tu_end - tu_begin)/ 100.0 << endl;
					cout <<"System (Kernel) Time (excluding MIS): " << double(ts_end - ts_begin)/ 100.0<< endl;
				#endif
			}
			cout <<"Total Runing Time in Sec: " << double(clock() - T1) / CLOCKS_PER_SEC << endl << endl;
		}
		else
			cout << "Init fail - in vaild file name or content"<<endl;

		cout << "Continue? y/n: ";
		cin >> c ;
		cout << endl;
			

	}

	if(SAVE_STATISTIC_TO_FILE)
	{
		outputStream << "\n";
		outputStream.close();
	}              

 	return 0;
}

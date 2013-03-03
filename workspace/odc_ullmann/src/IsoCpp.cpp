#include <iostream>
#include "graph.hpp"
#include "isoalg.hpp"
#include <libgen.h>

using namespace std;

bool fileexists(const char *filename) {
  ifstream ifile(filename);
  return ifile;
}


int main(int argc, char* argv[]) {
	char* a;
	char* b;


//	a = "/Users/jure/Temp/graphs/si2/rand/r001/si2_r001_s80.A02";
//	b = "/Users/jure/Temp/graphs/si2/rand/r001/si2_r001_s80.B02";

	//	a = "/Users/jure/Temp/graphs/si2/rand/r001/si2_r001_s100.A00";
//	b = "/Users/jure/Temp/graphs/si2/rand/r001/si2_r001_s100.B00";

//	a = "/Users/jure/Temp/graphs/si2/rand/r001/si2_r001_m200.A00";
//	b = "/Users/jure/Temp/graphs/si2/rand/r001/si2_r001_m200.B00";

//	a = "/Users/jure/Temp/graphs/si2/rand/r001/si2_r001_m400.A88";
//	b = "/Users/jure/Temp/graphs/si2/rand/r001/si2_r001_m400.B88";

//	a = "/Users/jure/Temp/graphs/si2/rand/r001/si2_r001_m600.A00";
//	b = "/Users/jure/Temp/graphs/si2/rand/r001/si2_r001_m600.B00";

//	a = "/Users/jure/Temp/graphs/si2/rand/r001/si2_r001_m1000.A00";
//	b = "/Users/jure/Temp/graphs/si2/rand/r001/si2_r001_m1000.B00";
	a = "/home/celebro/data/fri/diploma/src/workspace/data/mivia/si2/rand/r001/400.A00";
	b = "/home/celebro/data/fri/diploma/src/workspace/data/mivia/si2/rand/r001/400.B00";

	if (argc >= 3) {
		a = argv[1];
		b = argv[2];
	}

	if (!fileexists(a) || !fileexists(b)) {
		cerr << "One of the files not found.";
		return 1;
	}

	cout.flush();

	Digraph g(a);
	Digraph h(b);

//	g.print();
//	h.print();


	O alg(g, h);
	//printf("%s ", basename(a));
	alg.find(true);


	return 0;
}

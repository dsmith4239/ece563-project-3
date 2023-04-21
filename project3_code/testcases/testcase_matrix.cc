#include "../cache.cc"//#include "cache.h"
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <fstream>

#define KB 1024

using namespace std;


// naive mult testing


int main(int argc, char **argv){

	cache *mycache = NULL;

	for (unsigned b=32; b<=256; b=b*2){
	string path = "C:/Users/Smith/Desktop/NCSU/spring 23/ECE563/project/ece563-project-3/project3_code/matrix_outputs/blocksize_";
	string inpath = "C:/Users/Smith/Desktop/NCSU/spring 23/ECE563/project/ece563-project-3/project3_code/traces/bmult_n128_b";
	inpath.append(to_string(b));
	inpath.append(".t");
	
	path.append(to_string(b));
	path.append(".t");
	cout << "BLOCK SIZE = " << dec << b << endl;
	cout << "===================" << endl << endl;
	mycache = new cache(     16*KB,			//size
				  4,			//associativity
				  b,			//cache line size
				  WRITE_BACK,		//write hit policy
				  WRITE_ALLOCATE, 	//write miss policy
				  5, 			//hit time
				  100, 			//miss penalty
				  48    		//address width
				  );     
	mycache->print_configuration();
	mycache->load_trace(inpath.c_str());
	mycache->run();	
	mycache->print_to_file(path);
	delete mycache;
	}

	for (unsigned a=1; a<=16; a=a*2){
	string path = "C:/Users/Smith/Desktop/NCSU/spring 23/ECE563/project/ece563-project-3/project3_code/matrix_outputs/associativity_";
	path.append(to_string(a));
	cout << "ASSOCIATIVITY = " << dec << a << endl;
	cout << "===================" << endl << endl;
	mycache = new cache(     16*KB,			//size
				  a,			//associativity
				  64,			//cache line size
				  WRITE_BACK,		//write hit policy
				  WRITE_ALLOCATE, 	//write miss policy
				  5, 			//hit time
				  100, 			//miss penalty
				  48    		//address width
				  );     

	mycache->print_configuration();
	mycache->load_trace("C:/Users/Smith/Desktop/NCSU/spring 23/ECE563/project/ece563-project-3/project3_code/traces/bmult_n128_b64.t");
	mycache->run();	
	mycache->print_to_file(path);
	delete mycache;
	}

	for (unsigned cs=16; cs<=64; cs=cs*2){
	string path = "C:/Users/Smith/Desktop/NCSU/spring 23/ECE563/project/ece563-project-3/project3_code/matrix_outputs/cachesize_";
	path.append(to_string(cs));
	cout << "CACHE SIZE = " << dec << cs << endl;
	cout << "===================" << endl << endl;
	mycache = new cache(     cs*KB,			//size
				  4,			//associativity
				  64,			//cache line size
				  WRITE_BACK,		//write hit policy
				  WRITE_ALLOCATE, 	//write miss policy
				  5, 			//hit time
				  100, 			//miss penalty
				  48    		//address width
				  );     

	mycache->print_configuration();
	mycache->load_trace("C:/Users/Smith/Desktop/NCSU/spring 23/ECE563/project/ece563-project-3/project3_code/traces/bmult_n128_b64.t");
	mycache->run();	
	mycache->print_to_file(path);
	delete mycache;
	}



}

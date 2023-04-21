#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <iomanip>
#include "cache.h"
#include <fstream>

using namespace std;

void multiply(unsigned n);
void block_multiply(unsigned n, unsigned block_size);
void trace(char rw, unsigned long addr);

const char* arg = "multiply";

void usage(){
	cout << "Usage: ";
	cout << "multiply|block_multiply <n> <block_size>" << endl;
	cout << "<block_size> required only by block_multiply" << endl;
}

ofstream test;
/********************************
   *      MAIN                    *
    ********************************/
int main(int argc, char** argv){
        test.open("C:/Users/Smith/Desktop/NCSU/spring 23/ECE563/project/ece563-project-3/project3_code/traces/cache_multiply.t",fstream::out);
        if(!test.is_open()){
        cout << "File not created" << endl;
        return -1;
        }
        // ---------------------
        multiply(64); 
        // ---------------------
        if (argc < 3){
                usage(); 
		return -1;
        }
        int n = atoi(argv[2]);
       	if (n<=0){
                usage();
		return -1; 
	} 
	if (!strcmp(argv[1], "multiply")){
                multiply(n);
        }
        else if (!strcmp(argv[1], "block_multiply")){
                if (argc < 4){
                	usage(); 
			return -1;
                }
                int block_size = atoi(argv[3]);
		if (block_size <=0 || block_size > n){
			usage(); 
			return -1;
		}
                block_multiply(n,block_size);
        }else{
                usage(); 
		return -1;
        }
        return 0;
}


/* This function generates the memory accesses trace for a naive matrix multiplication code (see pseudocode below).
The code computes c=a*b, where a, b and c are square matrices containing n*n single-precision floating-point values.
Assume that the matrices are stored in row-major order, as follows:

a[0,0] a[0,1] ... a[0,n-1] a[1,0] a[1,1] ... a[1,n-1] ... a[n-1,0] a[n-1,1] ... a[n-1,n-1]

Assume that the three matrices are stored contiguously in memory starting at address 0x0.
The trace should contain all memory accesses to matrices a, b and c. You can ignore variables i, j, k and parameter n.
You can ignore the initialization of matrix c (i.e., assume it is already initialized to all 0s.

for (int i = 0; i < n; i++) {
  for (int j = 0; j < n; j++) {
    for (int k = 0; k < n; k++) {
      c[i,j] = a[i,k] * b[k,j] + c[i,j];
    }
  }
}



The trace should be written to standard output, and it should follow the format required by your cache simulator.
*/

// we need to run tests on this so i'm writing to a test.t file too

void multiply(unsigned n){

  if(!test.is_open()){
    cout << "File not created" << endl;
    return;
  }else{
    
  }
  // ** assuming 8-bit words? **

  unsigned long a_start = 0x0;
  unsigned long b_start = n * n * 4; // n**2 values * 4 bytes per value
  unsigned long c_start = n * n * 8; // b_start * 2 matrices

  unsigned long a_offset = -4*n - 4; // to negate initial increments
  unsigned long b_offset = -4*n - 4;
  unsigned long c_offset = -4*n - 4;

  for (int i = 0; i < n; i++) {
    // c next row: c += 4n
    c_offset += 4*n;
    // a next row: a += 4n
    a_offset += 4*n;

    for (int j = 0; j < n; j++) {
      // b next col: b += 4
      b_offset += 4;
      // c next col: c += 4
      c_offset += 4;


      for (int k = 0; k < n; k++) {
        // b next row: b += 4n
        b_offset += 4*n;
        // a next col: a += 4
        a_offset += 4;
        
        //c[i,j] = a[i,k] * b[k,j] + c[i,j];
        // write = read, read, read
        // read a [i,k]
        trace('r',a_start + a_offset);
        // a goes to next row
        // read b [k,j]
        trace('r',b_start + b_offset);
        // b goes to next row
        
        trace('r',c_start + c_offset);
        // c goes to next col
        
        trace('w',c_start + c_offset);
      }
    }
  }
  test.close();
}













/* This functi n generates the memory accesses trace for a matrix multiplication code that uses blocking (see pseudocode below).
The code computes c=a*b, where a, b and c are square matrices containing n*n single-precision floating-point values.
Assume that the matrices are stored in row-major order, as follows:

a[0,0] a[0,1] ... a[0,n-1] a[1,0] a[1,1] ... a[1,n-1] ... a[n-1,0] a[n-1,1] ... a[n-1,n-1]

Assume that the three matrices are stored contiguously in memory starting at address 0x0.
The trace should contain all memory accesses to matrices a, b and c. You can ignore variables ii, jj, kk, i, j, k and parameters n and block_size.
You can ignore the initialization of matrix c (i.e., assume it is already initialized to all 0s.

for (ii = 0; ii < n; ii+=block_size) {
  for (jj = 0; jj < n; jj+=block_size) {
    for (kk = 0; kk < n; kk+=block_size) {
      for (i = ii; i < ii+block_size; i++) {
        for (j = jj; j < jj+block_size; j++) {
          for (k = kk; k < kk+block_size; k++) {
            c[i,j] = a[i,k] * b[k,j] + c[i,j];
          }
        }
      }
    }
  }
}

The trace should be written to standard output, and it should follow the format required by your cache simulator.
*/
void block_multiply(unsigned n, unsigned block_size){
}

void trace(char rw, unsigned long addr){
 //cout << rw << " 0x" << setw(8) << setfill('0') << hex << addr << endl;
  test << rw << " 0x" << setw(8) << setfill('0') << hex << addr << endl;
}
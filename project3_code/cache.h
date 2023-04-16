#ifndef CACHE_H_
#define CACHE_H_

#include <stdio.h>
#include <stdbool.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define UNDEFINED 0xFFFFFFFFFFFFFFFF //constant used for initialization

typedef enum {WRITE_BACK, WRITE_THROUGH, WRITE_ALLOCATE, NO_WRITE_ALLOCATE} write_policy_t; 

typedef enum {HIT, MISS} access_type_t;

typedef long long address_t; //memory address type

typedef struct{
	unsigned index;
	unsigned dirty;
	address_t tag;
	unsigned entry_access;
} block_t;

typedef struct{
	unsigned index;
	vector<block_t> blocks;
} set_t;

class cache{

	/* Add the data members required by your simulator's implementation here */
	unsigned c_size;
	unsigned c_associativity; // n
	unsigned c_line_size;	// block size
	write_policy_t c_wr_hit_policy;
	write_policy_t c_wr_miss_policy;
	unsigned c_hit_time;
	unsigned c_miss_penalty;
	unsigned c_address_width;
	unsigned c_accesses; // increments on every run

	/* number of memory accesses processed */
	unsigned number_memory_accesses;
	unsigned reads;
	unsigned read_misses;
	unsigned writes;
	unsigned write_misses;
	unsigned evictions;
	unsigned memory_writes;
	float maccesstime;
	/* trace file input stream */	
	ifstream stream;

	unsigned c_num_sets;
	unsigned c_num_blocks;
	unsigned c_blocks_per_set;


	unsigned tag_bits;

	block_t null_block;
	set_t null_set;

	vector<set_t> cache_sets;

	// the CACHE has array of SETS with length c_num_sets
	// each SET has an array of BLOCKS with length ASSOCIATIVITY
	// each BLOCK has {index, dirty, tag}
	
	// BLOCKS are assigned to SETS based on value of INDEX (calculated during R/W)

public:

	/* 
	* Instantiates the cache simulator 
        */
	cache(unsigned cache_size, 		// cache size (in bytes)
              unsigned cache_associativity,     // cache associativity (fully-associative caches not considered)
	      unsigned cache_line_size,         // cache block size (in bytes)
	      write_policy_t write_hit_policy,  // write-back or write-through
	      write_policy_t write_miss_policy, // write-allocate or no-write-allocate
	      unsigned cache_hit_time,		// cache hit time (in clock cycles)
	      unsigned cache_miss_penalty,	// cache miss penalty (in clock cycles)	
	      unsigned address_width            // number of bits in memory address
	);	
	
	// de-allocates the cache simulator
	~cache();

	// loads the trace file (with name "filename") so that it can be used by the "run" function  
	void load_trace(const char *filename);

	// processes "num_memory_accesses" memory accesses (i.e., entries) from the input trace 
	// if "num_memory_accesses=0" (default), then it processes the trace to completion 
	void run(unsigned num_memory_accesses=0);
	
	// processes a read operation and returns hit/miss
	access_type_t read(address_t address);
	
	// processes a write operation and returns hit/miss
	access_type_t write(address_t address);

	// returns the next block to be evicted from the cache
	unsigned evict(unsigned index);
	
	// prints the cache configuration
	void print_configuration();
	
	// prints the execution statistics
	void print_statistics();

	//prints the metadata information (including "dirty" but, when applicable) for all valid cache entries  
	void print_tag_array();


	
};

#endif /*CACHE_H_*/

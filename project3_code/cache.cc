#include "cache.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <iomanip>
#include <math.h>

using namespace std;

cache::cache(unsigned size, 
      unsigned associativity,
      unsigned line_size,
      write_policy_t wr_hit_policy,
      write_policy_t wr_miss_policy,
      unsigned hit_time,
      unsigned miss_penalty,
      unsigned address_width
){
	c_size = size;
	c_associativity = associativity;
	c_line_size = line_size;
	c_wr_hit_policy = wr_hit_policy;
	c_wr_miss_policy = wr_miss_policy;
	c_hit_time = hit_time;
	c_miss_penalty = miss_penalty;
	c_address_width = address_width;
	c_accesses = 0;

	number_memory_accesses = 0;
	reads = 0;
	read_misses = 0;
	writes = 0;
	write_misses = 0;
	evictions = 0;
	memory_writes = 0;
	maccesstime = 0;

	// sanity checks
	c_num_sets = c_size / (c_line_size * c_associativity);
	c_num_blocks = c_associativity * c_num_sets;
	c_blocks_per_set = c_num_blocks / c_num_sets; // == associativity

	tag_bits = UNDEFINED;


	null_block.dirty = UNDEFINED;
	null_block.index = UNDEFINED;
	null_block.tag = UNDEFINED;
	null_block.entry_access = UNDEFINED;
	

	null_set.index = UNDEFINED;
	null_set.blocks = vector<block_t>();
	for(int i = 0; i < c_blocks_per_set; i++){
		null_set.blocks.push_back(null_block);
	}

	cache_sets = vector<set_t>();
	for(int i = 0; i < c_num_sets; i++){
	cache_sets.push_back(null_set);
	cache_sets[0].index = UNDEFINED;
	}
}

void cache::print_configuration(){
	cout << "CACHE CONFIGURATION" << endl;
	cout << "size = " << exp2(log2(c_size) - 10) << " KB" << endl;
	cout << "associativity = " << c_associativity << "-way" << endl;
	cout << "cache line size = " << c_line_size << " B" << endl;
	switch(c_wr_hit_policy){
		case 0: cout << "write hit policy = write-back" << endl;break;
		case 1: cout << "write hit policy = write-through" << endl;break;
	}
	//cout << "write hit policy = " << c_wr_hit_policy << endl;
	switch(c_wr_miss_policy){
		case 2: cout << "write miss policy = write-allocate" << endl; break;
		case 3: cout << "write miss policy = no-write-allocate" << endl; break;
	}
	//cout << "write miss policy = " << c_wr_miss_policy << endl;
	cout << "cache hit time = " << c_hit_time << " CLK" << endl;
	cout << "cache miss penalty = " << c_miss_penalty << " CLK" << endl;
	cout << "memory address width = " << c_address_width << " bits" << endl;
}

cache::~cache(){
	cache_sets.clear();
	stream.clear();
	stream.close();
	stream.seekg(0, ios::beg);
	

}

void cache::load_trace(const char *filename){
   stream.open(filename);
}

void cache::run(unsigned num_entries){

   unsigned first_access = number_memory_accesses;
   string line;
   unsigned line_nr=0;
   //fseek(,0,SEEK_SET);
   while (getline(stream,line)){

	line_nr++;
        char *str = const_cast<char*>(line.c_str());
	
        // tokenize the instruction
        char *op = strtok (str," ");
	char *addr = strtok (NULL, " ");
	address_t address = strtoul(addr, NULL, 16);

/* added */
	cout << "ADDRESS:: @=0x" << hex << address << endl;

	/* 
		edit here:
		insert the code to process read and write operations
	   	using the read() and write() functions below

	*/
	c_accesses++;
	if(*op == 'w') write(address);
	else if(*op == 'r') read(address);


	number_memory_accesses++;
	if (num_entries!=0 && (number_memory_accesses-first_access)==num_entries)
		break;
   }
}

void cache::print_statistics(){
	cout << "STATISTICS" << endl;

	number_memory_accesses = c_accesses;
	cout << "memory accesses = " << dec << number_memory_accesses << endl;

	cout << "read = " << reads << endl;
	cout << "read misses = " << read_misses << endl;
	cout << "write = " << writes << endl;
	cout << "write misses = " << write_misses << endl;
	cout << "evictions = " << evictions << endl;
	cout << "memory writes = " << memory_writes << endl;

	// Avg. memory access time = Hit time + Miss rate × Miss Penalty 
	maccesstime = c_hit_time + ((float)c_miss_penalty * (read_misses + write_misses) / number_memory_accesses);
	cout << "average memory access time " << maccesstime << endl;	
}

access_type_t cache::read(address_t address){
	reads++;
	// split address into tag, index, block offset
	unsigned index_width = log2(c_num_sets);
	unsigned blockoffset_width = log2(c_line_size);
	unsigned tag_width = c_address_width - index_width - blockoffset_width;
	tag_bits = tag_width;
	address_t index_mask = 0;
	address_t tag_mask = 0;
	address_t bo_mask = 0;
	for(int i = 0; i < index_width; i++){
		index_mask = index_mask << 1;
		index_mask++;
	}
	index_mask = index_mask << blockoffset_width;
	
	for(int i = 0; i < tag_width; i++){
		tag_mask = tag_mask << 1;
		tag_mask++;
	}
	tag_mask = tag_mask << (blockoffset_width + index_width);

	for(int i = 0; i < blockoffset_width; i++){
		bo_mask = bo_mask << 1;
		bo_mask++;
	}

	unsigned index = (address & index_mask) >> blockoffset_width;
	unsigned blockoffset = (address & bo_mask);
	unsigned tag = (address & tag_mask) >> (blockoffset_width + index_width);

	// -----------------------------------------------------

	// check cache at set(index) for block	
	set_t current_set = cache_sets.at(index);
	bool found = false;
	unsigned next_free_block = (unsigned)UNDEFINED;
	unsigned readblock = (unsigned)UNDEFINED;
	for(int blocknum = 0; blocknum < c_blocks_per_set; blocknum++){
		if(current_set.blocks.at(blocknum).tag == tag) {
			found = true;
			readblock = blocknum;
		}
		// find first empty block for new cache data
		if(next_free_block == (unsigned)UNDEFINED && current_set.blocks.at(blocknum).index == (unsigned)UNDEFINED){
			next_free_block = blocknum;
		}

	}
	// if present, return hit (and do something after?)
	if(found) {
		cache_sets.at(index).blocks.at(readblock).entry_access = c_accesses;
		return HIT;
	}
	else{ // miss
		read_misses++;
		if(next_free_block == (unsigned)UNDEFINED){
			// eviction policy
			// evict the block with value () out of all sets that has the
			// lowest entry_access value
			next_free_block = evict(index);
			if(cache_sets.at(index).blocks.at(next_free_block).dirty == 1) memory_writes++;
		}

		cache_sets.at(index).blocks.at(next_free_block).dirty = 0;
		cache_sets.at(index).blocks.at(next_free_block).index = index;
		cache_sets.at(index).blocks.at(next_free_block).tag = tag;
		cache_sets.at(index).blocks.at(next_free_block).entry_access = c_accesses;
		return MISS;
	}
}

access_type_t cache::write(address_t address){
    writes++;
	// split address into tag, index, block offset
	unsigned index_width = log2(c_num_sets);
	unsigned blockoffset_width = log2(c_line_size);
	unsigned tag_width = c_address_width - index_width - blockoffset_width;
	tag_bits = tag_width;
	address_t index_mask = 0;
	address_t tag_mask = 0;
	address_t bo_mask = 0;
	for(int i = 0; i < index_width; i++){
		index_mask = index_mask << 1;
		index_mask++;
	}
	index_mask = index_mask << blockoffset_width;
	
	for(int i = 0; i < tag_width; i++){
		tag_mask = tag_mask << 1;
		tag_mask++;
	}
	tag_mask = tag_mask << (blockoffset_width + index_width);

	for(int i = 0; i < blockoffset_width; i++){
		bo_mask = bo_mask << 1;
		bo_mask++;
	}

	unsigned index = (address & index_mask) >> blockoffset_width;
	unsigned blockoffset = (address & bo_mask);
	unsigned tag = (address & tag_mask) >> (blockoffset_width + index_width);
	
	// check cache at set(index) for block	
	set_t current_set = cache_sets.at(index);
	bool found = false;
	unsigned next_free_block = (unsigned)UNDEFINED;
	unsigned writeblock = (unsigned)UNDEFINED;
	for(int blocknum = 0; blocknum < c_blocks_per_set; blocknum++){
		if(current_set.blocks.at(blocknum).tag == tag) {
			found = true;
			writeblock = blocknum;
		}
		// find first empty block for new cache data
		if(next_free_block == (unsigned)UNDEFINED && current_set.blocks.at(blocknum).index == (unsigned)UNDEFINED){
			next_free_block = blocknum;
		}

	}
	// if present, return hit (and do something after?)
	if(found) {
		// mark as dirty if not already and refresh entry access
		cache_sets.at(index).blocks.at(writeblock).dirty = 1;
		cache_sets.at(index).blocks.at(writeblock).entry_access = c_accesses;
		return HIT;
	}
	else{
		write_misses++;
		if(next_free_block == (unsigned)UNDEFINED){
			// eviction policy - find LRU set index with block (index)
			next_free_block = evict(index);
			if(cache_sets.at(index).blocks.at(next_free_block).dirty == 1) memory_writes++;
		}

		cache_sets.at(index).blocks.at(next_free_block).dirty = 1;	// dirty == 1 only if write back
		cache_sets.at(index).blocks.at(next_free_block).index = index;
		cache_sets.at(index).blocks.at(next_free_block).tag = tag;
		cache_sets.at(index).blocks.at(next_free_block).entry_access = c_accesses;
		return MISS;
	}
}

void cache::print_tag_array(){
	cout << "TAG ARRAY" << endl;
	// for each block (blocks_per_set)
	for(int bn = 0; bn < c_blocks_per_set; bn++){
	// for each set, see if that block tag != undefined and print if it is
		cout << "BLOCKS " << bn << endl;


		// writeback

		if(c_wr_hit_policy == WRITE_BACK){
			cout << setfill(' ') << setw(7) << "index" << setw(6) << "dirty" << setw(4+tag_bits/4) << "tag" << endl; 
			for(int sn = 0; sn < c_num_sets; sn++){
				block_t current = cache_sets.at(sn).blocks.at(bn);
				if(current.tag != (unsigned)UNDEFINED){
					//stringstream tag << "0x" << hex << current.tag;

					//cout << current.index << " " << current.dirty << " " << current.tag << endl;				
					cout << setfill(' ') << setw(7) << current.index << setw(6) << current.dirty << setw(4+tag_bits/4) << "0x" << hex << current.tag << endl; 
				}
			}
		}
		else if(c_wr_hit_policy == WRITE_THROUGH){
			cout << setfill(' ') << setw(7) << "index" << setw(4+tag_bits/4) << "tag" << endl; 
			for(int sn = 0; sn < c_num_sets; sn++){
				block_t current = cache_sets.at(sn).blocks.at(bn);
				if(current.tag != (unsigned)UNDEFINED){
					string hexadec = "0x" + to_string(current.tag);
					
					//cout << current.index << " " << current.dirty << " " << current.tag << endl;				
					cout << setfill(' ') << setw(7) << current.index << "0x" << setw(4+tag_bits/4) << "0x" << hex << current.tag << endl; 
				}
			}
		}








	}
}

unsigned cache::evict(unsigned index){
	// find last recently used block in set (index)
	// with lowest entry_access
	evictions++;
	unsigned access_number = (unsigned)UNDEFINED;
	unsigned LRU = (unsigned)UNDEFINED;
	for(int blocknum = 0; blocknum < c_blocks_per_set; blocknum++){
		if(cache_sets.at(index).blocks.at(blocknum).entry_access < access_number){
			LRU = blocknum;
			access_number = cache_sets.at(index).blocks.at(blocknum).entry_access;
		}
	}

	return LRU; // replaces block number
}

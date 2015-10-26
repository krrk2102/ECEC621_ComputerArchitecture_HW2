#ifndef __BASE_CACHE_H__
#define __BASE_CACHE_H__

#include <iostream>
#include <list>
#include <vector>
#include <limits>

using namespace std;

#define ADDR_BITS 32
typedef struct _cacheLine {
    uint32_t tag;
    uint32_t *data;
    // data points to data stored in
    // cache line. cache line size is
    // multiple of 32bits.
    uint32_t LRUStackBits;
    // These bits keep track of usage position
    // of cache line in LRU stack. MRU cache
    // line is represented by the top of the
    // stack, while LRU is the bottom of the
    // stack. 'LRUStackBits=0' represents the
    // LRU cache line.
} cacheLine;

typedef struct _mem {
    uint32_t addr;
    uint32_t data;
} main_memory;

class BaseCache{
private:
    
    //cache base parameters
    uint32_t cacheSize; //in Bytes
    uint32_t associativity;
    uint32_t blockSize;  //in Bytes
    cacheLine **cacheLines;
    
    //cache derived parameters
    //define any additional parameters
    //needed as part of your design
    //WRITE ME
    uint32_t tagLength;
    uint32_t indexLength;
    uint32_t offsetLength;
    uint32_t NumberofSets;
    list<int> * LRUstack; // This list acts as a stack to keep track of least recent used cache.
    
    //cache access statistics
    uint32_t numReads;
    uint32_t numWrites;
    uint32_t numReadHits;
    uint32_t numReadMisses;
    uint32_t numWriteHits;
    uint32_t numWriteMisses;
	
public:
    //Default constructor to set everything to '0'
    BaseCache();
    //Constructor to initialize cache parameters and to create the cache
    BaseCache(uint32_t _cacheSize, uint32_t _associativity, uint32_t _blockSize);
    //Set cache base parameters
    void setCacheSize(uint32_t _cacheSize);
    void setAssociativity(uint32_t _associativity);
    void setBlockSize(uint32_t _blockSize);
    
    //Get cache base parameters
    uint32_t getCacheSize();
    uint32_t getAssociativity();
    uint32_t getBlockSize();
    
    //Get cache access statistics
    uint32_t getReadHits();
    uint32_t getReadMisses();
    uint32_t getWriteHits();
    uint32_t getWriteMisses();
    double getReadHitRate();
    double getReadMissRate();
    double getWriteHitRate();
    double getWriteMissRate();
    double getOverallHitRate();
    double getOverallMissRate();
    
    //Initialize cache derived parameters
    void initDerivedParams();
    //Reset cache access statistics
    void resetStats();
    //Create cache and clear it
    void createCache();
    //Reset cache
    void clearCache();
    //Read data
    //return true if it was a hit, false if it was a miss
    //data is only valid if it was a hit, input data pointer
    //is not updated upon miss.
    bool read(uint32_t addr, uint32_t *data);
    void readMissHandler(uint32_t addr, vector<main_memory> &memory);
    void readMissHandlerL2(uint32_t addr, uint32_t data);
    //Write data
    //Function returns write hit or miss status.
    bool write(uint32_t addr, uint32_t data);
    
    /********ADD ANY ADDITIONAL METHODS IF REQUIRED*********/
    // This method is to update most recent used cache in one set, by changing LRU stack and cache LRUStackBits. 
    void MostRecentUsed(int _SetNumber, int _WayNumber);
    
    //Destructor to free all allocated memeroy.
    ~BaseCache();
};

class test:public BaseCache {
	
};
#endif

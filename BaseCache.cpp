#include <iostream>
#include "BaseCache.h"
#include <cmath>
using namespace std;

//Default constructor to set everything to '0'
BaseCache::BaseCache() {
    cacheSize = 0;
    associativity = 0;
    blockSize = 0;
    cacheLines = NULL;
    LRUstack = NULL;
    
    numReadHits = 0;
    numReadMisses = 0;
    numReads = 0;
    numWriteHits = 0;
    numWriteMisses = 0;
    numWrites = 0;
    
    initDerivedParams();
}

//Constructor to initialize cache parameters, create the cache and clears it
BaseCache::BaseCache(uint32_t _cacheSize, uint32_t _associativity, uint32_t _blockSize) {
    cacheSize = _cacheSize;
    associativity = _associativity;
    blockSize = _blockSize;
	
    
    numReadHits = 0;
    numReadMisses = 0;
    numReads = 0;
    numWriteHits = 0;
    numWriteMisses = 0;
    numWrites = 0;
    
    initDerivedParams();
    createCache();
}

//Set cache base parameters
void BaseCache::setCacheSize(uint32_t _cacheSize) {
    cacheSize = _cacheSize;
}
void BaseCache::setAssociativity(uint32_t _associativity) {
    associativity = _associativity;
}
void BaseCache::setBlockSize(uint32_t _blockSize) {
    blockSize = _blockSize;
}

//Get cache base parameters
uint32_t BaseCache::getCacheSize() {
    return cacheSize;
}
uint32_t BaseCache::getAssociativity() {
    return associativity;
}
uint32_t BaseCache::getBlockSize() {
    return blockSize;
}

//Get cache access statistics
uint32_t BaseCache::getReadHits() {
    return numReadHits;
}
uint32_t BaseCache::getReadMisses() {
    return numReadMisses;
}
uint32_t BaseCache::getWriteHits() {
    return numWriteHits;
}
uint32_t BaseCache::getWriteMisses() {
    return numWriteMisses;
}
double BaseCache::getReadHitRate() {
    double ReadHitRate;
    if (numReads != 0) {
        ReadHitRate = (double)numReadHits*100.0/(double)numReads;
    } else {
        return 0;
    }
    return ReadHitRate;
}
double BaseCache::getReadMissRate() {
    double ReadMissRate;
    if (numReads != 0) {
        ReadMissRate = (double)numReadMisses*100.0/(double)numReads;
    } else {
        return 0;
    }
    return ReadMissRate;
}
double BaseCache::getWriteHitRate() {
    double WriteHitRate;
    if (numWrites != 0) {
        WriteHitRate = (double)numWriteHits*100.0/(double)numWrites;
    } else {
        return 0;
    }
    return WriteHitRate;
}
double BaseCache::getWriteMissRate() {
    double WriteMissRate;
    if (numWrites != 0) {
        WriteMissRate = (double)numWriteMisses*100.0/(double)numWrites;
    } else {
        return 0;
    }
    return WriteMissRate;
}
double BaseCache::getOverallHitRate() {
    double OverallHitRate;
    if (numWrites != 0 || numReads != 0) {
        OverallHitRate = ((double)numReadHits+(double)numWriteHits)*100.0/((double)numReads+(double)numWrites);
    } else {
        return 0;
    }
    return OverallHitRate;
}
double BaseCache::getOverallMissRate() {
    double OverMissRate;
    if (numReads != 0 || numWrites != 0) {
        OverMissRate = ((double)numReadMisses+(double)numWriteMisses)*100.0/((double)numReads+(double)numWrites);
    } else {
        return 0;
    }
    return OverMissRate;
}

//Initialize cache derived parameters
void BaseCache::initDerivedParams() {
    if (blockSize != 0 && associativity != 0) {
        NumberofSets = cacheSize/(blockSize*associativity);
    } else {
        NumberofSets = 0;
    }
    offsetLength = (uint32_t)log2(blockSize);
    indexLength = (uint32_t)log2(NumberofSets);
    tagLength = ADDR_BITS - offsetLength - indexLength;
    LRUstack = new list<int>[NumberofSets];
}

//Reset cache access statistics
void BaseCache::resetStats() {
    numReadHits = 0;
    numReadMisses = 0;
    numReads = 0;
    numWriteHits = 0;
    numWriteMisses = 0;
    numWrites = 0;
    initDerivedParams();
}

//Create cache and clear it
void BaseCache::createCache() {
    cacheLines = new cacheLine* [NumberofSets];
    for (uint32_t i = 0; i < NumberofSets; i++) {
        cacheLines[i] = new cacheLine[associativity];
        for (uint32_t j = 0; j < associativity; j++) {
            cacheLines[i][j].data = new uint32_t[blockSize/4];
            for (uint32_t k = 0; k < (blockSize/4); k++) {
                cacheLines[i][j].data[k] = 0;
            }
            cacheLines[i][j].LRUStackBits = 0;
            cacheLines[i][j].tag = numeric_limits<uint32_t>::max();
        }
    }
}

//Reset cache
void BaseCache::clearCache() {
    for (uint32_t i = 0; i < NumberofSets; i++) {
        for (uint32_t j = 0; j < associativity; j++) {
            delete [] cacheLines[i][j].data;
        }
        delete [] cacheLines[i];
    }
    delete [] cacheLines;
}

//Read data
//return true if it was a hit, false if it was a miss
//data is only valid if it was a hit, input data pointer
//is not updated upon miss. Make sure to update LRU stack
//bits. You can choose to separate the LRU bits update into
//a separate function that can be used from both read() and write().
bool BaseCache::read(uint32_t addr, uint32_t *data) {
    // In case of cache not be initialized or accidentally cleared.
    if (cacheLines != NULL) {
        uint32_t tag_needed = addr>>(offsetLength+indexLength);
        uint32_t index_needed = addr>>offsetLength;
        index_needed = index_needed & (uint32_t)(pow(2, indexLength)-1);
        uint32_t offset_needed = addr & (uint32_t)(pow(2, offsetLength)-1);
        offset_needed = offset_needed>>2;
        for (uint32_t i = 0; i < associativity; i++) {
            if (cacheLines[index_needed][i].tag == tag_needed) {
                MostRecentUsed(index_needed, i);
                *data = cacheLines[index_needed][i].data[offset_needed];
                numReads++;
                numReadHits++;
                return true;
            }
        }
        numReads++;
        numReadMisses++;
    }
    return false;
}

void BaseCache::readMissHandler(uint32_t addr, vector<main_memory> &memory) {
    if (cacheLines != NULL) {
        uint32_t tag_needed = addr>>(offsetLength+indexLength);
        uint32_t index_needed = addr>>offsetLength;
        index_needed = index_needed & (uint32_t)(pow(2, indexLength)-1);
        uint32_t offset_needed = addr & (uint32_t)(pow(2, offsetLength)-1);
        offset_needed = offset_needed>>2;
        int mem_no = (addr>>2)-offset_needed;
        for (uint32_t i = 0; i < associativity; i++) {
            if (cacheLines[index_needed][i].LRUStackBits == 0) {
                MostRecentUsed(index_needed, i);
                cacheLines[index_needed][i].tag = tag_needed;
                for (uint32_t j = 0; j < blockSize/4; j++) {
                    if (mem_no+j < (int)memory.size()) {
                        cacheLines[index_needed][i].data[j] = memory[mem_no+j].data;
                    }
                }
                break;
            }
        }
    }
}

void BaseCache::readMissHandlerL2(uint32_t addr, uint32_t data) {
    if (cacheLines != NULL) {
        uint32_t tag_needed = addr>>(offsetLength+indexLength);
        uint32_t index_needed = addr>>offsetLength;
        index_needed = index_needed & (uint32_t)(pow(2, indexLength)-1);
        uint32_t offset_needed = addr & (uint32_t)(pow(2, offsetLength)-1);
        offset_needed = offset_needed>>2;
        for (uint32_t i = 0; i < associativity; i++) {
            if (cacheLines[index_needed][i].LRUStackBits == 0) {
                MostRecentUsed(index_needed, i);
                cacheLines[index_needed][i].tag = tag_needed;
                cacheLines[index_needed][i].data[offset_needed] = data;
                break;
            }
        }
    }
}

//Write data
//Function returns write hit or miss status.
bool BaseCache::write(uint32_t addr, uint32_t data) {
    // In case of cache not be initialized or accidentally cleared.
    if (cacheLines != NULL) {
        uint32_t tag_needed = addr>>(offsetLength+indexLength);
        uint32_t index_needed = addr>>offsetLength;
        index_needed = index_needed & (uint32_t)(pow(2, indexLength)-1);
        uint32_t offset_needed = addr & (uint32_t)(pow(2, offsetLength)-1);
        offset_needed = offset_needed>>2;
        for (uint32_t i = 0; i < associativity; i++) {
            if (cacheLines[index_needed][i].tag == tag_needed) {
                MostRecentUsed(index_needed, i);
                cacheLines[index_needed][i].data[offset_needed] = data;
                numWrites++;
                numWriteHits++;
                return true;
            }
        }
        numWriteMisses++;
        numWrites++;
    }
    return false;
}

//Destructor to free all allocated memeroy.
BaseCache::~BaseCache() {
    clearCache();
    delete [] LRUstack;
}

// This method is additional added method for updating most recent used cache
void BaseCache::MostRecentUsed(int _SetNumber, int _WayNumber) {
    // In case the stack is not initialized.
    if (LRUstack != NULL) {
        bool exist_in_LRUstack = false;
        for (list<int>::iterator it = LRUstack[_SetNumber].begin(); it != LRUstack[_SetNumber].end(); ) {
            if (*it == _WayNumber) {
                it = LRUstack[_SetNumber].erase(it);
                LRUstack[_SetNumber].push_front(_WayNumber);
                cacheLines[_SetNumber][_WayNumber].LRUStackBits = 1;
                if (LRUstack[_SetNumber].size() == associativity) {
                    cacheLines[_SetNumber][LRUstack[_SetNumber].back()].LRUStackBits = 0;
                }
                exist_in_LRUstack = true;
                break;
            } else {
                it++;
            }
        }
        // When cache just initialized, the stack isn't full and element in the back is not actually LRU.
        if (exist_in_LRUstack == false) {
            if (LRUstack[_SetNumber].size() < associativity) {
                LRUstack[_SetNumber].push_front(_WayNumber);
                cacheLines[_SetNumber][_WayNumber].LRUStackBits = 1;
                if (LRUstack[_SetNumber].size() == associativity) {
                    cacheLines[_SetNumber][LRUstack[_SetNumber].back()].LRUStackBits = 0;
                }
            } else {
                cout<<"Stack compacity conflict with way number."<<endl;
            }
        }
    }
}

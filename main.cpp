#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include "BaseCache.h"
using namespace std;

int main(int argc, char **argv) {
    string inp_line, parsed_line;
    string command;
    unsigned int address, data;
    uint32_t cacheSizeL1, associativityL1, blockSizeL1, cacheSizeL2, associativityL2, blockSizeL2;
    argv[1] = "65536";
    cacheSizeL1 = atoi(argv[1]);
    argv[2] = "4";
    associativityL1 = atoi(argv[2]);
    argv[3] = "32";
    blockSizeL1 = atoi(argv[3]);
    argv[4] = "131072";
    cacheSizeL2 = atoi(argv[4]);
    argv[5] = "16";
    associativityL2 = atoi(argv[5]);
    argv[6] = "32";
    blockSizeL2 = atoi(argv[6]);
    argv[7] = "/Users/wushangqi/ecec621-winter2015-sw843/classwork/a02/input3.txt";
    ifstream fp_inp(argv[7]);
    
    BaseCache L1BaseCache(cacheSizeL1, associativityL1, blockSizeL1);
    BaseCache L2BaseCache(cacheSizeL2, associativityL2, blockSizeL2);
    int minAccessTime = numeric_limits<int>::max();
    int maxAccessTime = numeric_limits<int>::min();
    
    string memory_path = "/Users/wushangqi/ecec621-winter2015-sw843/classwork/a02/memfoot.dat";
    ifstream main_mem(memory_path.c_str());
    vector<main_memory> MainMemory;
    MainMemory.reserve((int)pow(2, 18));
    while (!main_mem.eof()) {
        string buff;
        main_mem>>buff;
        if (buff.empty() == false) {
            main_memory tmp;
            tmp.addr = stoul(buff, NULL, 16);
            buff.clear();
            main_mem>>buff;
            tmp.data = stoul(buff, NULL, 16);
            buff.clear();
            MainMemory.push_back(tmp);
        }
    }
    main_mem.close();
    
    if(fp_inp.is_open()) {
        while(getline(fp_inp, inp_line)) {
            stringstream inp_string(inp_line);
            if(getline(inp_string,parsed_line, ' '))
                command = parsed_line;
            if(getline(inp_string,parsed_line, ' '))
                address = stoul(parsed_line, NULL, 16);
            if (!(command.compare("w"))) {
                if(getline(inp_string,parsed_line, ' ')) {
                    data = stoul(parsed_line, NULL, 16);
                }
            }
            //Issue read/write command
            if (!(command.compare("w"))) {
                L1BaseCache.write(address, data);
                if (minAccessTime > 1) {
                    minAccessTime = 1;
                }
                if (maxAccessTime < 1) {
                    maxAccessTime = 1;
                }
                L2BaseCache.write(address, data);
                if (minAccessTime > 15) {
                    minAccessTime = 15;
                }
                if (maxAccessTime < 15) {
                    maxAccessTime = 15;
                }
                MainMemory[address/4].data = data;
                if (minAccessTime > 70) {
                    minAccessTime = 70;
                }
                if (maxAccessTime < 70) {
                    maxAccessTime = 70;
                }
            }
            if (!(command.compare("r"))) {
                if(L1BaseCache.read(address, &data)) {
                    if (minAccessTime > 1) {
                        minAccessTime = 1;
                    }
                    if (maxAccessTime < 1) {
                        maxAccessTime = 1;
                    }
                } else {
                    if (L2BaseCache.read(address, &data)) {
                        L1BaseCache.readMissHandlerL2(address, data);
                        if (minAccessTime > 16) {
                            minAccessTime = 16;
                        }
                        if (maxAccessTime < 16) {
                            maxAccessTime = 16;
                        }
                    } else {
                        L1BaseCache.readMissHandler(address, MainMemory);
                        L2BaseCache.readMissHandler(address, MainMemory);
                        if (minAccessTime > 86) {
                            minAccessTime = 86;
                        }
                        if (maxAccessTime < 86) {
                            maxAccessTime = 86;
                        }
                    }
                }
            }
        }
        fp_inp.close();
    }
    cout <<endl;
    cout << "L1 Read Hits (HitRate): "<<dec<<L1BaseCache.getReadHits()<<" ("<<dec<<(int)L1BaseCache.getReadHitRate()<<"%)"<<endl;
    cout << "L1 Read Misses (MissRate): "<<dec<<L1BaseCache.getReadMisses() <<" ("<<dec<<(int)L1BaseCache.getReadMissRate()<<"%)"<<endl;
    cout << "L1 Write Hits (HitRate): "<<dec<<L1BaseCache.getWriteHits()<<" ("<<dec<<(int)L1BaseCache.getWriteHitRate()<<"%)"<<endl;
    cout << "L1 Write Misses (MissRate): "<<dec<<L1BaseCache.getWriteMisses() <<" ("<<dec<<(int)L1BaseCache.getWriteMissRate()<<"%)"<<endl;
    cout << "L1 Overall Hit Rate: "<<dec<<(int)L1BaseCache.getOverallHitRate() <<"%" << endl;
    cout << "L1 Overall Miss Rate: "<<dec<<(int)L1BaseCache.getOverallMissRate()<<"%"<<endl;
    
    cout <<endl;
    cout << "L2 Read Hits (HitRate): "<<dec<<L2BaseCache.getReadHits()<<" ("<<dec<<(int)L2BaseCache.getReadHitRate()<<"%)"<<endl;
    cout << "L2 Read Misses (MissRate): "<<dec<<L2BaseCache.getReadMisses() <<" ("<<dec<<(int)L2BaseCache.getReadMissRate()<<"%)"<<endl;
    cout << "L2 Write Hits (HitRate): "<<dec<<L2BaseCache.getWriteHits()<<" ("<<dec<<(int)L2BaseCache.getWriteHitRate()<<"%)"<<endl;
    cout << "L2 Write Misses (MissRate): "<<dec<<L2BaseCache.getWriteMisses() <<" ("<<dec<<(int)L2BaseCache.getWriteMissRate()<<"%)"<<endl;
    cout << "L2 Overall Hit Rate: "<<dec<<(int)L2BaseCache.getOverallHitRate() <<"%" << endl;
    cout << "L2 Overall Miss Rate: "<<dec<<(int)L2BaseCache.getOverallMissRate()<<"%"<<endl;
    
    cout<<endl;
    cout<<"Average memory access time (AMAT) (Reads): "<<dec<<(int)(1+L1BaseCache.getReadMissRate()*0.01*(15+L2BaseCache.getReadMissRate()*0.7))<<"ns"<<endl;
    cout<<"Minimum access time: "<<dec<<minAccessTime<<"ns"<<endl;
    cout<<"Maximum access time: "<<dec<<maxAccessTime<<"ns"<<endl;
    
    return 1;
}

void f(int p[]) {
	int * q;
	const int * r;
	int s[10];
	p = q; // 1
	p = r; // 2
	p = s; // 3
	q = p; // 4
	q = r; // 5
	q = s; // 6
	r = p; // 7
	r = q; // 8
	r = s; // 9
	s = p; // 10
	s = q; // 11
	s = r; // 12
	
	BaseCache b;
	test d;
	BaseCache* pb;
	test* pd;
	b = d; // a
	d = b; // b
	pd = pb; // c
	pb = pd; // d
	d = pd; // e
	b = *pd; // f
	*pd = *pb; // g
}


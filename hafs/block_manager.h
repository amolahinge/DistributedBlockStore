#include<iostream>
#include <string>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <vector>
#include <unordered_map>
#include <ios>
#include <cstdio>
#include <fstream>
#include <utility>

#include "SHA256.h"

using namespace std;

const int BLOCK_SIZE         = 4096;
const int BLOCK_DIVIDER_SIZE = 2048;
const int BLOCK_PER_DIVIDER  = 2048;
const int MAX_BLOCK          = BLOCK_PER_DIVIDER*BLOCK_DIVIDER_SIZE;


class BlockManager {
    private:
        string fsRoot;

        bool isAlligned(int addr) {
            if(addr % BLOCK_SIZE == 0) {
                return true;
            }
            return false;
        }

        int getBlockNumber(int addr) {
            return addr / BLOCK_SIZE;
        }

        int getBlockOffset(int blockNumber) {
            return blockNumber % BLOCK_DIVIDER_SIZE;
        }
        
    public:
        BlockManager() {

        }
        BlockManager(string fsRoot) {
            this->fsRoot = fsRoot;
        }

        // vector<int

        bool write(int addr, string data) {
            if(!isAlligned(addr)) {
                return unallignedWrite(addr, data);
            }

            return allignedWrite(addr, data);
        }

        bool allignedWrite(int addr, string data) {
            int blockNumber = getBlockNumber(addr);
            int blockOffset = getBlockOffset(blockNumber);
            string offsetPath = fsRoot + "/" + to_string(blockOffset);
            string blockPath = offsetPath + "/" + to_string(blockNumber);
            cout << "[BlockManager] Performing an alligned write on addr[" << addr <<"] with block path[" << blockPath << "] and offset path[" << offsetPath << "]" << endl;
            
            mkdir(offsetPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO); // To make sure the offset directory exists

            int fd = open(blockPath.c_str(),
                O_CREAT | O_RDWR,
                S_IRWXU | S_IRWXG | S_IRWXO);

            int writeRes = pwrite(fd, &data[0], BLOCK_SIZE, 0);
            
            if(writeRes == -1) {
                close(fd);
                return false;
            }

            close(fd);
            return true;
        }

        bool unallignedWrite(int addr, string data) {
            int firstBlockNumber        = getBlockNumber(addr);
            int firstBlockDir           = getBlockOffset(firstBlockNumber);
            int firstBlockReadLocation  = addr % BLOCK_SIZE;
            int firstBlockReadSize      = BLOCK_SIZE - firstBlockReadLocation;

            int secondBlockNumber       = firstBlockNumber + 1;
            int secondBlockDir          = getBlockOffset(secondBlockNumber);
            int secondBlockReadLocation = 0;
            int secondBlockReadSize     = firstBlockReadLocation;

            string firstOffsetPath      = fsRoot + "/" + to_string(firstBlockDir);
            string firstBlockPath       = firstOffsetPath + "/" + to_string(firstBlockNumber);

            string secondOffsetPath     = fsRoot + "/" + to_string(secondBlockDir);
            string secondBlockPath      = secondOffsetPath + "/" + to_string(secondBlockNumber);

            mkdir(firstBlockPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO); // To make sure the offset directory exists
            mkdir(secondBlockPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO); // To make sure the offset directory exists


            int firstFd = open(firstBlockPath.c_str(),
                O_CREAT | O_RDWR,
                S_IRWXU | S_IRWXG | S_IRWXO);

            int secondFd = open(secondBlockPath.c_str(),
                O_CREAT | O_RDWR,
                S_IRWXU | S_IRWXG | S_IRWXO);

            string firstBuf;
            firstBuf.resize(firstBlockReadSize);
            firstBuf.replace(0, firstBlockReadSize, data.substr(0, firstBlockReadSize));
            string secondBuf;
            secondBuf.resize(secondBlockReadSize);
            secondBuf.replace(0, firstBlockReadSize, data.substr(firstBlockReadSize, secondBlockReadSize));


            int firstRes  = pwrite(firstFd, &firstBuf[0], firstBlockReadSize, firstBlockReadLocation);
            int secondRes = pwrite(secondFd, &secondBuf[0], secondBlockReadSize, secondBlockReadLocation);

            if(firstRes == -1 || secondRes == -1) {
                close(firstFd);
                close(secondFd);
                return false;
            }
            close(firstFd);
            close(secondFd);
            return true;
        }

        bool read(int addr, string* data) {
            if(!isAlligned(addr)) {
                return unallignedRead(addr, data);
            }
            return allignedRead(addr, data);
        }   

        bool allignedRead(int addr, string* data) {
            int blockNumber = getBlockNumber(addr);
            int blockOffset = getBlockOffset(blockNumber);
            string offsetPath = fsRoot + "/" + to_string(blockOffset);
            string blockPath = offsetPath + "/" + to_string(blockNumber);
            cout << "[BlockManager] Performing an alligned read on addr[" << addr <<"] with block path[" << blockPath << "] and offset path[" << offsetPath << "]" << endl;
            

            mkdir(blockPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO); // To make sure the offset directory exists

            int fd = open(blockPath.c_str(),
                O_RDONLY,
                S_IRWXU | S_IRWXG | S_IRWXO);

            if(fd < 0) {
                return false; // Probably this block does not exist!
            }

            

            string buf;
            buf.resize(BLOCK_SIZE);
            int readRes = pread(fd, &buf[0], BLOCK_SIZE, 0);

            if(readRes == -1) {
                close(fd);
                return false;
            }

            data->resize(BLOCK_SIZE);
            data->replace(0, BLOCK_SIZE, buf);
        

            close(fd);
            return true;
        }

        bool unallignedRead(int addr, string* data) {
            int firstBlockNumber        = getBlockNumber(addr);
            int firstBlockDir           = getBlockOffset(firstBlockNumber);
            int firstBlockReadLocation  = addr % BLOCK_SIZE;
            int firstBlockReadSize      = BLOCK_SIZE - firstBlockReadLocation;

            int secondBlockNumber       = firstBlockNumber + 1;
            int secondBlockDir          = getBlockOffset(secondBlockNumber);
            int secondBlockReadLocation = 0;
            int secondBlockReadSize     = firstBlockReadLocation;

            string firstOffsetPath      = fsRoot + "/" + to_string(firstBlockDir);
            string firstBlockPath       = firstOffsetPath + "/" + to_string(firstBlockNumber);

            string secondOffsetPath     = fsRoot + "/" + to_string(secondBlockDir);
            string secondBlockPath      = secondOffsetPath + "/" + to_string(secondBlockNumber);

            mkdir(firstBlockPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO); // To make sure the offset directory exists
            mkdir(secondBlockPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO); // To make sure the offset directory exists


            int firstFd = open(firstBlockPath.c_str(),
                O_RDONLY,
                S_IRWXU | S_IRWXG | S_IRWXO);

            int secondFd = open(secondBlockPath.c_str(),
                O_RDONLY,
                S_IRWXU | S_IRWXG | S_IRWXO);

            string firstBuf;
            firstBuf.resize(firstBlockReadSize);
            string secondBuf;
            secondBuf.resize(secondBlockReadSize);


            int firstRes  = pread(firstFd, &firstBuf[0], firstBlockReadSize, firstBlockReadLocation);
            int secondRes = pread(secondFd, &secondBuf[0], secondBlockReadSize, secondBlockReadLocation);

            data->resize(BLOCK_SIZE);
            data->replace(0, BLOCK_SIZE, string(BLOCK_SIZE, '\0'));
            if(firstRes != -1) {
                data->replace(0, firstBlockReadSize, firstBuf);
            }
            if(secondRes != -1) {
                data->replace(firstBlockReadSize, secondBlockReadSize, secondBuf);
            }

            close(firstFd);
            close(secondFd);
            return true;
        }

        string CalCheckSum(int addr)
        {
            string data;
            read(addr,&data);
            return sha256(data);
        }       
};
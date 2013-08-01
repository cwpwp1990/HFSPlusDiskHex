//
//  main.cpp
//  HFSPlusDiskHex
//
//  Created by Peng Pagict on 13-5-28.
//  Copyright (c) 2013年 pagict. All rights reserved.
//

#include <iostream>
#include <vector>
#include <utility>
#include "ScanDisk/DiskScan.hh"
#include "ScanVolume/VolumeScan.h"

std::vector<std::string> diskList;

const std::string Unit[] = {"Byte(s)", "KiloByte(s)", "MegaByte(s)", "GigaByte(s)", "TidaByte(s)"};

inline std::string convertByteUnit(unsigned long szInByte)
{
    int i=0;
    float tmp = szInByte;
    while( tmp > 1024) {
        tmp = tmp / 1024;
        i++;
    }
    
    char buf[64];
    sprintf(buf,"%f ", tmp);
    return std::string(buf)+Unit[i];
}


int main(int argc, const char * argv[])
{
    // 逐个尝试有多少磁盘
#define MAX_DISK 64
    for (int i=0; i<MAX_DISK; i++) {
        char diskName[64];
        sprintf(diskName, "/dev/disk%d",i);
        FILE* fp;
        if ((fp=fopen(diskName, "rb")) != NULL) {
            printf("(%d)%s ",(int)diskList.size(), diskName);
            std::string str(diskName);
            diskList.push_back(str);
            fclose(fp);
        }
    }
    printf("\n");
    const char* diskName;

#ifdef DEBUG
    diskName = "/Users/pengpagict/Documents/pseuDisk.dmg";
    //diskName = "/dev/disk3";
#endif
    int select;
#ifdef RELEASE
    char ch = getchar();
    select = atoi(&ch);
    diskName = diskList.at(select).c_str();
#endif
    
    enum PartitionType type=partitionTable(diskName);

    printf("scaning device %s... ...\n", diskName);
    
    PartitionItemList partitionList;
    if (type==GPT) {
        printf("The Partiton table Type is GPT. ");
        printf("The Partiton information are listed below:\n");
        GPTHeader hder;
        scanGPTTable(diskName, partitionList, hder);
    }
    else if (type==MBR) {
        //scanMBRTable(diskList.at(ch));
    }
    int index = 0;
    for (PartitionItemList::iterator it = partitionList.begin(); it != partitionList.end(); it++, index++) {
        printf("Volume(%d):",index);
        printf("\t\t\tType:%-15s",(*it).partitionType.c_str());
        printf("\t\t\tName:%-18s", it->partitionName.c_str());
        printf("\t\t\tSize:%s", convertByteUnit(it->volumeSzByte).c_str());
        printf("\n");
    }
    printf("select volume number to check:...\n");
    
    long start_offset;
    
#ifdef RELEASE
    ch = getchar();
    select = atoi(&ch);
#endif

#ifdef DEBUG
    select = 1;
#endif
    
    
    start_offset = partitionList.at(select).startLBA;
    
    FILE* p_file = fopen(diskName, "rb");
    HFSPlusVolumeHeader volumeHeader;
    getVolumeHeader(p_file,start_offset, &volumeHeader);
    return 0;
}


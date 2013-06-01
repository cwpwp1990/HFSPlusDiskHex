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

void
printPrompt() {
}

void
diskPrompt() {
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
    
    char ch = getchar();
    int select = atoi(&ch);
    enum PartitionType type=partitionTable(diskList.at(select).c_str());
    printf("scaning device %s... ...\n", diskList.at(select).c_str());
    
    PartitionItemList partitionList;
    if (type==GPT) {
        printf("The Partiton table Type is GPT. ");
        printf("The Partiton information are listed below:\n");
        GPTHeader hder;
        scanGPTTable(diskList.at(select).c_str(), partitionList, hder);
    }
    else if (type==MBR) {
        //scanMBRTable(diskList.at(ch));
    }
    int index = 0;
    for (PartitionItemList::iterator it = partitionList.begin(); it != partitionList.end(); it++, index++) {
        printf("Volume(%d):",index);
        printf("\t\t\tType:%-15s",(*it).partitionType.c_str());
        printf("\t\t\tName:%-18s", it->partitionName.c_str());
        printf("\t\t\tSize in Byte:%lu", it->volumeSzByte);
        printf("\n");
    }
    
    return 0;
}


//
// DiskScan.cpp
// HFS_Plus-DiskHex
//
// Created by Peng Pagict on 13-5-29
// Copyright (c) 2013Äê pagict. All rights reserved.
//

#include "DiskRead.h"
#include "DiskScan.hh"
#include "PartitionTypeGUID.h"
#include <cstdio>

/* STANDARD GPT table item struct
 */
struct GPTItem {
    char partitionType[16];
    char partitionGUID[16];
    
    unsigned long startLBA;
    unsigned long endLBA;
    
    unsigned long attributeLabel;
    char partitionName[72];
};

void getString(const char *buf, size_t bufSz, std::string& str)
{
    for (int i = 0; i < bufSz; i += 2) {
        if (buf[i] == 0) {
            return;
        }
        str.append(buf+i);
    }
}

void  formatGUID(const char buf[16], std::string& str)
{
    int _1;
    unsigned short _2;
    unsigned short _3;
    
    memcpy(&_1, buf+0, sizeof(int));
    char _1Str[8];
    sprintf(_1Str, "%08X", _1);
    str.append(_1Str);
    str.append("-");
    
    memcpy(&_2, buf+4, sizeof(short));
    char _2Str[4];
    sprintf(_2Str, "%04X",_2);
    str.append(_2Str);
    str.append("-");
    
    memcpy(&_3, buf+6, sizeof(short));
    char _3Str[4];
    sprintf(_3Str, "%04X", _3);
    str.append(_3Str);
    str.append("-");
    
    const char* p = buf+8;
    unsigned char _4;
    char _4Str[3];
    
    for (int i = 0; i < 2; i++) {
        _4 = *p;
        sprintf(_4Str, "%02X", (int)_4);
        str.append(_4Str);
        p++;
    }
    str.append("-");
    
    for (int i = 0; i < 6; i++) {
        _4 = *p;
        sprintf(_4Str, "%02X", (int)_4);
        str.append(_4Str);
        p++;
    }
    
}


/*  To get the Type of the Partition Table
 *  Current can only recognise GPT and MBR
 */
enum PartitionType
partitionTable(const char* deviceName)
{
    enum PartitionType retType;
    FILE* fp=fopen(deviceName,"rb");

    if (!fp) {
        retType = UNKNOWN;
        return retType;
    }

    unsigned char buf[BLOCKSIZE];
    fread(buf,BLOCKSIZE,1, fp);
    
    unsigned char* p = buf+0x1be;
    p += 4;
    if ( *p == 0xee) {
        retType = GPT;
    } else {
        retType = MBR;
    }

    fclose(fp);
    return retType;
}

inline std::string guidString(const std::string& guidstr)
{
    using std::string;
    if (guidstr == HFSPlus) {
        return string("HFS+");
    }
    
    if (guidstr == AppleBoot) {
        return string("Apple Boot");
    }
    
    if (guidstr == EFISystem) {
        return string("EFI System");
    }
    
    return string("Unknown Type");
}

/* extract some critical attributes of the GPT partitio table
 * item.
 */
void briefGPTItem(GPTItem* item, PartitionItemInfo* briefItem)
{
    std::string guid;
    formatGUID(item->partitionType, guid);
    
    briefItem->partitionType = guidString(guid);
    briefItem->startLBA = item->startLBA;
    briefItem->endLBA = item->endLBA;
    getString(item->partitionName, 72, briefItem->partitionName);
    
    briefItem->volumeSzByte = (briefItem->endLBA - briefItem->startLBA + 1) * BLOCKSIZE;
}

/* get all partition brief information of a specified disk.
 */
void    scanGPTTable(const char* deviceName, PartitionItemList& list, GPTHeader& hder)
{
    FILE *fp = fopen(deviceName, "rb");
    if (!fp) {
        return;
    }
    unsigned char buf[BLOCKSIZE];
    fseek(fp, BLOCKSIZE, SEEK_SET);
    fread(buf, BLOCKSIZE, 1, fp);
    
    memcpy(&hder, buf, sizeof(GPTHeader));
    
    fseek(fp, hder.h_partitionHeader*BLOCKSIZE, SEEK_SET);
    fread(buf, BLOCKSIZE, 1, fp);
    
    unsigned char *p=buf;
    long _1st8bytes;
    memcpy(&_1st8bytes, p, sizeof(long));
    
    for (int i = 0; i < hder.h_tableItemCnt; i++) {
        
        GPTItem item;
        memcpy(&item, p, hder.h_tableItemSz);
        
        if (!item.startLBA) {
            break;
        }
        
        if (item.startLBA > 0) {
            PartitionItemInfo info;
            briefGPTItem(&item, &info);
            
            list.push_back(info);
        }
        
        p+=hder.h_tableItemSz;
        if (i % 4 == 3) {
            fseek(fp, BLOCKSIZE, SEEK_CUR);
            fread(buf, BLOCKSIZE, 1, fp);
            p=buf;
        }
    }
    
    
    fclose(fp);
}
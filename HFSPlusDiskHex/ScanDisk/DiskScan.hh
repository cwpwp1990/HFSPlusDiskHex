// DiskScan.h
// HFS_Plus-DiskHex
//
// Created by Peng Pagict on 13-5-29
// Copyright (c) 2013年 pagict. All rights reserved.
//
// 对磁盘进行的操作

#ifndef __HFSPlus_DiskHex__DiskScan__
#define __HFSPlus_DiskHex__DiskScan__

#include <vector>
#include <string>

enum PartitionType {
    UNKNOWN,
    GPT,
    MBR
};

/* STANDARD GPT Header struct
 */
struct GPTHeader {
    unsigned char h_autograph[4];
    unsigned int h_revision;
    unsigned int h_headerSize;
    unsigned int h_headerCRC;
    unsigned int h_reserved;
    unsigned long h_curLBA;
    unsigned long h_backupLBA;
    unsigned long h_1stPart;
    unsigned long h_lastPart;
    unsigned char h_diskGUID[16];
    unsigned long h_partitionHeader;
    unsigned int h_tableItemCnt;
    unsigned int h_tableItemSz;
    unsigned int h_partitionCRC;
};

struct PartitionItemInfo {
    std::string partitionType;
    unsigned long startLBA;
    unsigned long endLBA;
    unsigned long volumeSzByte;
    std::string partitionName;
};

enum PartitionType     partitionTable(const char* deviceName);

typedef std::vector<PartitionItemInfo> PartitionItemList;
void    scanGPTTable(const char* deviceName, PartitionItemList& list, GPTHeader& header);

#endif /* defined(__HFS_Plus_DiskHex__DiskRead__) */

//
//  VolumeScan.h
//  HFSPlusDiskHex
//
//  Created by Peng Pagict on 13-5-31.
//  Copyright (c) 2013年 pagict. All rights reserved.
//

#ifndef HFSPlusDiskHex_VolumeScan_h
#define HFSPlusDiskHex_VolumeScan_h

#import <Foundation/Foundation.h>
#include <stdio.h>

int    getVolumeHeader(FILE* p_file, unsigned long startOffset, HFSPlusVolumeHeader* p_volumeHeader);

#endif

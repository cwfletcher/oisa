#ifndef __SCAN_ORAM_HEADER__
#define __SCAN_ORAM_HEADER__

/// Scan an entire array for reading one block
/// An array of N blocks starting at base_addr, each block is the size of block_sz(words)
//  read the idx block to the address data
void ScanORAM_Read(int* base_addr, int N, int block_sz_w, int* data, int idx);

/// Scan an entire array for writing one block
/// An array of N blocks starting at base_addr, each block is the size of block_sz(words)
//  write the idx block with the block at the address data
void ScanORAM_Write(int* base_addr, int N, int block_sz_w, int* data, int idx);

#endif

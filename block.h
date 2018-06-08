#ifndef BLOCK_H
#define BLOCK_H

#define SUPER_EMPTY_BYTES 1
#define SUPER_TABLE_SIZE 1
#define INODE_EMPTY_BYTES 239

#define SUPER_BLOCK_TYPE   1
#define INODE_BLOCK_TYPE   2
#define EXTENT_BLOCK_TYPE  3
#define FREE_BLOCK_TYPE    4
   
#define SUPER_EMPTY_BYTES  239
#define INODE_EMPTY_BYTES  233
#define EXTENT_EMPTY_BYTES 250
#define FREE_EMPTY_BYTES   252
#define MAX_NAME_LEN       9
#define BLOCK_DETAIL_BYTES 6

/* all block types have this information. type refers to block type number,
 * magicNum refers to the number used for checking if  data is corrupted,
 * next refers to the index of the next block in the disk  */
typedef struct
{
   unsigned char type;
   unsigned char magicNum;
   unsigned short next;
}  BlockDetails;

/* inode block - block type #2. fileName is up to 8 chars, fileSize refers to
 * number of blocks in file, lastIndex is last block index of file */
typedef struct
{
   BlockDetails details;
   unsigned char fileName[MAX_NAME_LEN];
   unsigned int fileSize;
   unsigned int FP;
   unsigned int startFP;
   unsigned short numBlocks;
   char empty[INODE_EMPTY_BYTES];  
} InodeBlock;

/* superblock block - block type #1. contains pointer to root inode, 
 * the number of the last file created in the file system, a boolean
 * to check if the file is closed, and the number of blocks in the disk. */
typedef struct
{
   BlockDetails details;
   unsigned short rootInodeBlock;
   unsigned short currFileNum;
   unsigned char isClosed;
   unsigned short numBlocks;
   unsigned short fileTable[SUPER_TABLE_SIZE];
   unsigned char empty[SUPER_EMPTY_BYTES];
   char empty[SUPER_EMPTY_BYTES];
} SuperBlock;

/* file extent block - block type #3. contains block index of file inode. */
typedef struct
{
   BlockDetails details;
   unsigned short inodeIndex;
   char data[EXTENT_EMPTY_BYTES];
} FileExtentBlock;

/* free block - block type #4, ready for future writes. all blocks in disk
 * initially instantiated as FreeBlocks. */
typedef struct 
{
   BlockDetails details;
   char empty[FREE_EMPTY_BYTES];
} FreeBlock;

#endif

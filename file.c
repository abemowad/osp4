#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "string.h"
#include "block.h"
#include "libTinyFS.h"
#include "libDisk.h"

/* ----  TO DO  ----
 * 1) closed should change FD table and closedInode table
 */ 

unsigned short nextFreeBlock()
{
   int i;
   unsigned short blockNum;
   char someBlock[BLOCKSIZE];

   blockNum = 0;
   for (i = 2; i < diskTable[mountedDisk].superBlock.numBlocks; i++)
   { 
      if (readBlock(mountedDisk, i, someBlock) != 0)
         return 0;

      if (someBlock[0] == FREE_BLOCK_TYPE)
      {
         blockNum = i;
         break;
      }
   }     
   return blockNum;
}

/* creates inode block for new file */
InodeBlock createInodeBlock(char *name)
{
   InodeBlock inodeBlock;

   inodeBlock.details.type = INODE_BLOCK_TYPE;
   inodeBlock.details.magicNum = MAGIC_NUMBER;
   inodeBlock.details.next = 0;

   strcpy(inodeBlock.fileName, name);

   inodeBlock.fileSize = 0;
   inodeBlock.FP = 0;
   inodeBlock.startFP = 0;
   
   inodeBlock.numBlocks = 0;
   inodeBlock.isClosed = 0;
   inodeBlock.location = nextFreeBlock();
   return inodeBlock;
}

/* inserts inode into disk's inode table and returns fd */
fileDescriptor insertNewInode(InodeBlock inodeBlock)
{
   int i;
   fileDescriptor FD;

   FD = -1;
   for (i = 1; i < SUPER_TABLE_SIZE; i++)
   {
      if (diskTable[mountedDisk].inodeTable[i].fileSize == -1)
      {
         diskTable[mountedDisk].inodeTable[i] = inodeBlock;
         FD = i;
         break;
      }
   }
   return FD;
}

/* finds index of inode block in closed inode table if present */
unsigned char findClosedFile(char *name)
{
   unsigned char i;

   for (i = 0; i < SUPER_TABLE_SIZE; i++)
   {
      if (diskTable[mountedDisk].closedInodes[i].fileSize == -1)
         continue;
      if (strcmp(name, diskTable[mountedDisk].closedInodes[i].fileName) == 0)
         return i;
   }
   return 0;
}

/* checks if file is already open and returns its fd */
fileDescriptor findOpenFile(char *name)
{
   fileDescriptor FD;

   for (FD = 1; FD < SUPER_TABLE_SIZE; FD++)
   {
      if (diskTable[mountedDisk].inodeTable[FD].fileSize == -1)
         continue;
      if (strcmp(name, diskTable[mountedDisk].inodeTable[FD].fileName) == 0)
         return FD;
   }
   return 0;
}

/* Opens a file for reading and writing on the currently mounted file system. 
 * Creates a dynamic resource table entry for the file, and returns a file 
 * descriptor (integer) that can be used to reference this file while the 
 * filesystem is mounted. */
fileDescriptor tfs_openFile(char *name)
{
   unsigned char index;
   fileDescriptor FD;
   InodeBlock inodeBlock;
   
   if (mountedDisk < 0) {
      return noMountedDiskErr;
   }

   if ((FD = findOpenFile(name)))
      return FD;

   if ((index = findClosedFile(name)))
   {
      diskTable[mountedDisk].closedInodes[index].fileSize = -1;
      inodeBlock = diskTable[mountedDisk].closedInodes[index];
      inodeBlock.isClosed = 0;
   }
   else
      inodeBlock = createInodeBlock(name);
    
   if (inodeBlock.location == 0)
      return -1;

   FD = insertNewInode(inodeBlock);
   printf("ADDING CREATED ACCESSED AND MODIFIED\n");
   diskTable[mountedDisk].inodeTable[FD].timestamp.created = time(0);
   diskTable[mountedDisk].inodeTable[FD].timestamp.accessed = time(0);
   diskTable[mountedDisk].inodeTable[FD].timestamp.modified = time(0);
   if (writeBlock(mountedDisk, inodeBlock.location, &inodeBlock) != 0)
      return -1; 
   return FD; 
}

/* returns EOF FP */
int getMaxFP(fileDescriptor FD)
{
   int maxFP, numBlocks;
   InodeBlock inodeBlock;

   inodeBlock = diskTable[mountedDisk].inodeTable[FD];

   /* find number of blocks to skip over block detail bytes for fp */
   fprintf(stderr, "filesize: %d\n", inodeBlock.fileSize);
   numBlocks = inodeBlock.fileSize / EXTENT_EMPTY_BYTES + 1;
   if (inodeBlock.fileSize % EXTENT_EMPTY_BYTES == 0)
      numBlocks -= 1;

   maxFP = inodeBlock.startFP + inodeBlock.fileSize +
      (numBlocks - 1) * BLOCK_DETAIL_BYTES;
   
   return maxFP;
}

/* change the file pointer location to offset (absolute). Returns success/error codes.
 * how this works is changing inode table fp and then overwriting inode block in disk
 * with new inode */
int tfs_seek(fileDescriptor FD, int offset)
{
   int FP, maxFP;
   unsigned short numBlocks;
   InodeBlock *inodeBlock;

   inodeBlock = &diskTable[mountedDisk].inodeTable[FD];
   numBlocks = offset / (EXTENT_EMPTY_BYTES) + 1;
   if (offset % EXTENT_EMPTY_BYTES == 0)
      numBlocks -= 1;

   maxFP = getMaxFP(FD); 
   FP = inodeBlock->startFP + offset + (numBlocks - 1) * BLOCK_DETAIL_BYTES;

   fprintf(stderr, "offset : %d\n", offset);
   fprintf(stderr, "startFP: %d\n", inodeBlock->startFP);
   fprintf(stderr, "FP : %d\n", FP);
   fprintf(stderr, "MAX FP : %d\n", maxFP);
   /* check if negative offset or FP points past last byte in file */
   if (offset < 0 || FP > maxFP)
   {
      fprintf(stderr, "invalid offset\n");
      return -1;
   } 
   if (writeBlock(mountedDisk, inodeBlock->location, inodeBlock) != 0)
   {
      fprintf(stderr, "Could not update inode after tfs_seek");
      return -1;
   }
   
   inodeBlock->FP = FP;
   return 0;
}

/* creates file extent block to store file contents */
FileExtentBlock createExtentBlock(fileDescriptor FD)
{
   FileExtentBlock extentBlock;
   InodeBlock inodeBlock;

   inodeBlock = diskTable[mountedDisk].inodeTable[FD];

   extentBlock.details.type = EXTENT_BLOCK_TYPE;
   extentBlock.details.magicNum = MAGIC_NUMBER;
   extentBlock.details.next = inodeBlock.details.next + inodeBlock.numBlocks;

   return extentBlock;
}

/* writes file contents to a block in disk */ 
int writeExtentBlock(int *firstBlock, int *inodePrev, 
   FileExtentBlock *extentBlock, FileExtentBlock prevBlock, fileDescriptor FD)
{
   InodeBlock inodeBlock = diskTable[mountedDisk].inodeTable[FD];

   
   if (*firstBlock)
   {
      *firstBlock = 0;
      return 0;
   }

   if (*inodePrev)
   {
       *inodePrev = 0;
       return writeBlock(mountedDisk, inodeBlock.details.next, extentBlock);
   }
   else
      return writeBlock(mountedDisk, prevBlock.details.next, extentBlock);
}

unsigned short findFileBlocks(int size)
{
   unsigned short i, currIndex, prevIndex, numFreeBlocks, sizeBlocks, startBlock;
   
   numFreeBlocks = 0;
   sizeBlocks = size / EXTENT_EMPTY_BYTES;

   if (size % EXTENT_EMPTY_BYTES != 0)
      sizeBlocks += 1;
  
   for (i = 0; i < diskTable[mountedDisk].superBlock.numBlocks; i++)
   {
      if ((currIndex = nextFreeBlock()) == 0)
         return 0;
      
      if (i == 0)
      {
         startBlock = currIndex;
         ++numFreeBlocks;
      }
      else
      {
         if (currIndex = prevIndex + 1)
            ++numFreeBlocks;
         else
         {
            numFreeBlocks = 1;
            startBlock = currIndex;
         }
      }
      if (numFreeBlocks == sizeBlocks)
         return startBlock; 
      prevIndex = currIndex;
   }
   return 0;
}

/* Writes buffer ‘buffer’ of size ‘size’, which represents an entire 
 * file’s contents, to the file system. Sets the file pointer to 0 
 * (the start of file) when done. Returns success/error codes. */
int tfs_writeFile(fileDescriptor FD, char *buffer, int size)
{
   int i, firstBlock, inodePrev;   
   FileExtentBlock extentBlock, prevBlock;
   InodeBlock *inodeBlock;
   
   if (diskTable[mountedDisk].inodeTable[FD].fileSize == -1 || size <= 0)
      return -1;

   i = 0;
   firstBlock = 1;
   inodePrev = 1;
   extentBlock = createExtentBlock(FD);
   prevBlock = extentBlock;
   inodeBlock = &diskTable[mountedDisk].inodeTable[FD];
   
   if (inodeBlock->details.next == 0)
   {
      if (!(inodeBlock->details.next = findFileBlocks(size)))
         return -1;
   }
   else if (inodeBlock->fileSize < size)
      return -1;   

   inodeBlock->fileSize = size;
   inodeBlock->startFP = inodeBlock->details.next * BLOCKSIZE + BLOCK_DETAIL_BYTES - 1;

   tfs_seek(FD, 0);
   while (size)
   {
      if (i % EXTENT_EMPTY_BYTES  == 0)
      {
         inodeBlock->numBlocks++;

         if (writeExtentBlock(&firstBlock, &inodePrev, 
            &extentBlock, prevBlock, FD) != 0)
            return -1;

         prevBlock = extentBlock;
         extentBlock = createExtentBlock(FD);
      }

      extentBlock.data[i] = buffer[i];
      i++;
      size--;
   }
   
   if (writeBlock(mountedDisk, inodeBlock->location, inodeBlock) != 0)
      return -1;

   printf("%ld\n",diskTable[mountedDisk].inodeTable[FD].timestamp.modified);
   long int myt = time(0);
   printf("%ld\n", myt);
   printf("CHANGING MODIFIED\n");
   diskTable[mountedDisk].inodeTable[FD].timestamp.modified = time(0);

   if (firstBlock)
      return writeBlock(mountedDisk, inodeBlock->details.next, &extentBlock);
   else
      return writeBlock(mountedDisk, prevBlock.details.next, &extentBlock);
}

/* reads one byte from the file and copies it to buffer, using the current file
 * pointer location and incrementing it by one upon success. If the file pointer
 * is already at the end of the file then tfs_readByte() should return an error 
 * and not increment the file pointer. */
int tfs_readByte(fileDescriptor FD, char *buffer)
{
   int maxFP, blockNum, byteIndex;
   FileExtentBlock extentBlock;
   InodeBlock *inodeBlock;

   extentBlock = createExtentBlock(FD);
   inodeBlock = &diskTable[mountedDisk].inodeTable[FD];

   maxFP = getMaxFP(FD);

   blockNum = inodeBlock->fileSize / EXTENT_EMPTY_BYTES;
   if (inodeBlock->fileSize % EXTENT_EMPTY_BYTES == 0)
      blockNum -= 1;

   if (blockNum < 0)
   {
      fprintf(stderr, "Nothing has been written\n");
      return -1;
   }
   else if (inodeBlock->FP == maxFP + 1)
   {
      fprintf(stderr, "File pointer has reached EOF\n");
      return -1;
   }

   inodeBlock->FP += 1;
   if (writeBlock(mountedDisk, inodeBlock->location, inodeBlock) != 0)
   {
      fprintf(stderr, "Write error\n");
      return -1;
   }
   
   if (readBlock(mountedDisk, blockNum, &extentBlock) != 0)
      return -1;

   printf("CHANGING ACCESSED\n");
   diskTable[mountedDisk].inodeTable[FD].timestamp.accessed = time(0);
   
   byteIndex = (inodeBlock->FP - 1) % EXTENT_EMPTY_BYTES;
   *buffer = extentBlock.data[byteIndex];
   fprintf(stderr, "data : %d\n", extentBlock.data[1]);
   return 0;
}

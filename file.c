#include <stdio.h>
#include <stdlib.h>

#include "block.h"

/* ----  TO DO  ----
 * 1) Create fileTable
 * 2) Create inodeTable
 * 3) Make sure file is updated upon every modification of blocks
 */ 

/* returns EOF FP */
int getMaxFP(fileDescriptor FD)
{
   int maxFP, numBlocks;

   /* does inodeTable actually look like this? 
    * find number of blocks to skip over block detail bytes for fp */
   numBlocks = inodeTable[FD]->fileSize / EXTENT_EMPTY_BYTES + 1;
   if (inodeTable[FD]->fileSize % EXTENT_EMPTY_BYTES == 0)
      numBlocks -= 1;

   maxFP = inodeTable[FD]->startFP + inodeTable[FD]->fileSize +
      (numBlocks - 1) * BLOCK_DETAIL_BYTES - 1;
   
   return maxFP;
}

/* change the file pointer location to offset (absolute). Returns success/error codes.
 * how this works is changing inode table fp and then overwriting inode block in disk
 * with new inode */
int tfs_seek(fileDescriptor FD, int offset)
{
   int FP, maxFP;
  
   maxFP = getMaxFP(FD); 
   FP = inodeTable[FD]->startFP + offset + (numBlocks - 1) * BLOCK_DETAIL_BYTES;

   /* check if negative offset or FP points past last byte in file */
   if (offset < 0 || FP > maxFP)
      return -1;

   /* fileTable[FD] is block offset that inodeBlock[FD] is written to 
    * it points to first byte of inode right?
    * inodeBlock[FD] is struct but still might work as argument */
   if (writeBlock(curDiskNum, fileTable[FD], inodeBlock[FD]) != 0)
      return -1;
   
   inodeTable[FD]->FP = FP;
   return 0;
}

/* creates file exten block to store file contents */
ExtentBlock createExtentBlock()
{
   ExtentBlock extentBlock;

   extentBlock.details.type = EXTENT_BLOCK_TYPE;
   extentBlock.details.magicNum = MAGIC_NUMBER;
   extentBlock.details.next = inodeTable[FD]->details.next + inodeTable[FD]->numBlocks;

   return extentBlock;
}

/* writes file contents to a block in disk */ 
int writeExtentBlock(int *firstBlock, int *inodePrev, 
   ExtentBlock *extentBlock, ExtentBlock prevBlock)
{
   if (*firstBlock)
   {
      *firstBlock = 0;
      return 0;
   }

   if (*inodePrev)
   {
       *inodePrev = 0;
       return writeBlock(curDiskNum, inodeBlock[FD]->details.next, extentBlock);
   }
   else
      return writeBlock(curDiskNum, prevBlock.details.next, extentBlock);
}

/* Writes buffer ‘buffer’ of size ‘size’, which represents an entire 
 * file’s contents, to the file system. Sets the file pointer to 0 
 * (the start of file) when done. Returns success/error codes. */
int tfs_writeFile(fileDescriptor FD, char *buffer, int size)
{
   int i, firstBlock, inodePrev;   
   ExtentBlock extentBlock, prevBlock;
   
   if (size <= 0)
      return -1;

   firstBlock = 1;
   inodePrev = 1;
   extentBlock = createExtentBlock();
   prevBlock = extentBlock;

   tfs_seek(FD, 0);
   while (size)
   {
      if (i % EXTENT_EMPTY_BYTES  == 0)
      {
         inodeBlock[FD]->numBlocks++;

         if (writeExtentBlock(&firstBlock, &inodePrev, 
            &extentBlock, prevBlock) != 0)
            return -1;

         prevBlock = extentBlock;
         extentBlock = createExtentBlock();
      }

      extentBlock->data[i] = buffer[i];
      i++;
      size--;
   }

   /* inodeBlock[FD]->details.next = fileTable[FD] + 1; (upon creation of file) 
    * and numBlocks should be set to 0 */
   if (firstBlock)
      return writeBlock(curDiskNum, inodeBlock[FD]->details.next, &extentBlock, prevBlock);
   else
      return writeBlock(curDiskNum, prevBlock.details.next, &extentBlock, prevBlock);
}

/* reads one byte from the file and copies it to buffer, using the current file
 * pointer location and incrementing it by one upon success. If the file pointer
 * is already at the end of the file then tfs_readByte() should return an error 
 * and not increment the file pointer. */
int tfs_readByte(fileDescriptor FD, char *buffer)
{
   int maxFP, blockNum, byteIndex;
   ExtentBlock extentBlock;

   extentBlock = createExtentBlock();
   maxFP = getMaxFP[FD];

   blockNum = inodeTable[FD]->fileSize / EXTENT_EMPTY_BYTES;
   if (inodeTable[FD]->fileSize % EXTENT_EMPTY_BYTES == 0)
      blockNum -= 1;

   if (blockNum < 0 || inodeBlock[FD]->FP == maxFP)
      return -1;

   inodeBlock[FD]->FP += 1;
   if (writeBlock(curDiskNum, fileTable[FD], inodeBlock[FD]) != 0)
      return -1;
   
   if (readBlock(curDiskNum, blockNum, &extentBlock) != 0)
      return -1;

   byteIndex = inodeBlock[FD]->FP / ((blockNum + 1) * 
      (BLOCKSIZE - BLOCK_DETAIL_BYTES)) + inodeBlock[FD]->FP % BLOCKSIZE - 1
   *buffer = extentBlock.data[byteIndex]; 
   return 0;
}

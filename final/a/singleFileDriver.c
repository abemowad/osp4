#include <stdio.h>
#include <stdlib.h>

#include "libTinyFS.h"
#include "TinyFS_errno.h"

void singleFileDriver()
{
   int i, mkfsVal, mountVal, openVal, closeVal, readVal, writeVal, deleteVal,
      seekVal;
   char emptyByte, firstByte, lastByte, randByte, EOFByte, 
      bigGoodBlock[BLOCKSIZE], bigBadBlock[2 * BLOCKSIZE], 
      smallGoodBlock[BLOCKSIZE / 2];

   lastByte = randByte = 0;
   for (i = 0; i < BLOCKSIZE; i++)
      bigGoodBlock[i] = i  % BLOCKSIZE;

   for (i = 0; i < 2 * BLOCKSIZE; i++)
      bigBadBlock[i] = i  % BLOCKSIZE;

   for (i = 0; i < BLOCKSIZE / 2; i++)
      smallGoodBlock[i] = i % BLOCKSIZE;

   mkfsVal = tfs_mkfs("moo.txt", DEFAULT_DISK_SIZE);
   mountVal = tfs_mount("moo.txt");

   printf("\n-------------------------- SINGLE FILE DRIVER -------------------------\n");
   printf("\n-----Opening file-----\n");
   openVal = tfs_openFile("fake.txt");
   printf("tfs_openFile returns fd: %d\n", openVal);
   
   printf("\n-----Opening the already open file-----\n");
   openVal = tfs_openFile("fake.txt");
   printf("tfs_openFile returns fd: %d\n", openVal);

   printf("\n-----Attempting to read byte from empty file-----\n");
   readVal = tfs_readByte(openVal, &emptyByte);
   printf("tfs_readByte returns: %d\n", readVal);

   printf("\n-----Writing 2 blocks of data to empty file-----\n");
   writeVal = tfs_writeFile(openVal, bigGoodBlock, BLOCKSIZE);
   printf("tfs_writeFile returns: %d\n", writeVal);
   
   printf("\n-----Attempting to write more blocks than available-----\n");
   writeVal = tfs_writeFile(openVal, bigBadBlock, 2 * BLOCKSIZE);
   printf("tfs_writeFile returns: %d\n", writeVal);
   
   printf("\n-----Moving file pointer to random byte in block 2-----\n");
   seekVal = tfs_seek(openVal, BLOCKSIZE / 2 + 2);
   printf("tfs_seek returns: %d\n", seekVal);

   printf("\n-----Reading random byte from block 2-----\n");
   readVal = tfs_readByte(openVal, &randByte);
   printf("tfs_readByte returns: %d\n", readVal);
   printf("randByte: %d\n", randByte); 

   printf("\n-----Resizing file to smaller size by writing less bytes-----\n");
   writeVal = tfs_writeFile(openVal, smallGoodBlock, BLOCKSIZE / 2);
   printf("tfs_writeFile returns: %d\n", writeVal);

   printf("\n-----Moving file pointer to beginning of file-----\n");
   seekVal = tfs_seek(openVal, 0);
   printf("tfs_seek returns: %d\n", seekVal);

   printf("\n-----Reading first byte from file-----\n");
   readVal = tfs_readByte(openVal, &firstByte);
   printf("tfs_readByte returns: %d\n", readVal);
   printf("firstByte: %d\n", firstByte);
   
   printf("\n-----Moving file pointer to last byte-----\n");
   seekVal = tfs_seek(openVal, BLOCKSIZE / 2 - 1);
   printf("tfs_seek returns: %d\n", seekVal);

   printf("\n-----Reading last byte from file-----\n");
   readVal = tfs_readByte(openVal, &lastByte);
   printf("tfs_readByte returns: %d\n", readVal);
   printf("lastByte: %d\n", lastByte);

   printf("\n-----Attempting to read after last byte-----\n");
   readVal = tfs_readByte(openVal, &EOFByte);
   printf("tfs_readByte returns: %d\n", readVal);
   printf("EOFByte: %d\n", EOFByte);
   
   printf("\n-----Closing File-----\n");
   closeVal = tfs_closeFile(openVal);
   printf("tfs_closeFile returns: %d\n", closeVal);
   
   printf("\n-----Reopening closed file-----\n");
   openVal = tfs_openFile("fake.txt");
   printf("tfs_openFile returns fd: %d\n", openVal);

   printf("\n-----Moving file pointer to last byte-----\n");
   seekVal = tfs_seek(openVal, BLOCKSIZE / 2 - 1);
   printf("tfs_seek returns: %d\n", seekVal);

   printf("\n-----Rereading last byte from previously closed file-----\n");
   readVal = tfs_readByte(openVal, &lastByte);
   printf("tfs_readByte returns: %d\n", readVal);
   printf("lastByte: %d\n", lastByte);

   printf("\n-----Deleting file-----\n");
   deleteVal = tfs_deleteFile(openVal);
   printf("tfs_deleteFile returns: %d\n", deleteVal);
      
   printf("\n-----Reopening deleted file-----\n");
   openVal = tfs_openFile("fake.txt");
   printf("tfs_openFile returns fd: %d\n", openVal);

   printf("\n-----Attempting to read byte from previously deleted file-----\n");
   readVal = tfs_readByte(openVal, &emptyByte);
   printf("tfs_readByte returns: %d\n", readVal);
   printf("\n----------------------- SINGLE FILE DRIVER COMPLETED ----------------------\n");
}

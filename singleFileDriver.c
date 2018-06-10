#include <stdio.h>
#include <stdlib.h>

#include "libTinyFS.h"
#include "TinyFS_errno.h"

int main1()
{
   int i, mkfsVal, mountVal, openVal, readVal, writeVal, seekVal;
   char emptyByte, firstByte, lastByte, randByte, EOFByte, 
      bigGoodBlock[BLOCKSIZE], bigBadBlock[2 * BLOCKSIZE], 
      smallGoodBlock[BLOCKSIZE / 2];

   lastByte = randByte = 0;
   for (i = 0; i < BLOCKSIZE; i++)
      bigGoodBlock[i] = i % BLOCKSIZE;

   for (i = 0; i < 2 * BLOCKSIZE; i++)
      bigBadBlock[i] = i  % BLOCKSIZE;

   for (i = 0; i < BLOCKSIZE / 2; i++)
      smallGoodBlock[i] = i % BLOCKSIZE;

   mkfsVal = tfs_mkfs("moo.txt", DEFAULT_DISK_SIZE);
   printf("tfs_mkfs: %d\n", mkfsVal);

   mountVal = tfs_mount("moo.txt");
   printf("tfs_mount: %d\n", mountVal);

   openVal = tfs_openFile("fake.txt");
   printf("tfs_openFile: %d\n", openVal);
   
   openVal = tfs_openFile("fake.txt");
   printf("tfs_openFile: %d\n", openVal);

   readVal = tfs_readByte(openVal, &emptyByte);
   printf("tfs_readByte: %d\n", readVal);

   writeVal = tfs_writeFile(openVal, bigGoodBlock, BLOCKSIZE);
   printf("tfs_writeFile: %d\n", writeVal);
   
   writeVal = tfs_writeFile(openVal, bigBadBlock, 2 * BLOCKSIZE);
   printf("tfs_writeFile: %d\n", writeVal);
   
   writeVal = tfs_writeFile(openVal, smallGoodBlock, BLOCKSIZE / 2);
   printf("tfs_writeFile: %d\n", writeVal);

   readVal = tfs_readByte(openVal, &firstByte);
   printf("tfs_readByte: %d\n", readVal);
   printf("firstByte: %d\n", firstByte);

   seekVal = tfs_seek(openVal, BLOCKSIZE / 2);
   printf("tfs_seek: %d\n", seekVal);

   /* this part isn't working yet */
   readVal = tfs_readByte(openVal, &lastByte);
   printf("tfs_readByte: %d\n", readVal);
   printf("lastByte: %d\n", lastByte);

   readVal = tfs_readByte(openVal, &EOFByte);
   printf("tfs_readByte: %d\n", readVal);
   printf("EOFByte: %d\n", EOFByte);

   // once lastByte is correct and tfs_readByte is functioning
   
   /*readVal = tfs_readByte(openVal, &randByte);
   printf("tfs_readByte: %d\n", readVal);
   printf("randByte: %d\n", randByte);*/
   return 0;
}

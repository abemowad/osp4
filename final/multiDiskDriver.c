#include <stdio.h>
#include <stdlib.h>

#include "libTinyFS.h"
#include "TinyFS_errno.h"

void multiDiskDriver()
{
   int i, mkfsVal, mountVal, unmountVal, seekVal, openVal, writeVal, readVal;
   char randByte, someBlocks[BLOCKSIZE], someOtherBlocks[BLOCKSIZE];
   
   for (i = 0; i < BLOCKSIZE; i++)
   {
      someBlocks[i] = i % BLOCKSIZE;
      someOtherBlocks[i] = (i - 1) % BLOCKSIZE;
   }

   printf("\n-------------------------- MULTI DISK DRIVER -------------------------\n");
   printf("\n------------ Making First File Sytstem------------\n");
   mkfsVal = tfs_mkfs("goo.txt", DEFAULT_DISK_SIZE);
   printf("tfs_mkfs return value: %d\n", mkfsVal);

   printf("\n------------ Making Second File Sytstem------------\n");
   mkfsVal = tfs_mkfs("joo.txt", DEFAULT_DISK_SIZE);
   printf("tfs_mkfs return value: %d\n", mkfsVal);

   printf("\n------------ Mounting Second File Sytstem------------\n");
   mountVal = tfs_mount("joo.txt");
   printf("tfs_mount return value: %d\n", mountVal);
   
   printf("\n-----Opening file-----\n");
   openVal = tfs_openFile("fake.txt");
   printf("tfs_openFile returns fd: %d\n", openVal);

   printf("\n-----Writing 2 blocks of data to empty file-----\n");
   writeVal = tfs_writeFile(openVal, someBlocks, BLOCKSIZE);
   printf("tfs_writeFile returns: %d\n", writeVal);

   printf("\n------------ Unmounting Second File Sytstem------------\n");
   unmountVal = tfs_unmount();
   printf("tfs_unmount return value: %d\n", unmountVal);

   printf("\n------------ Mounting First File Sytstem------------\n");
   mountVal = tfs_mount("goo.txt");
   printf("tfs_mount return value: %d\n", mountVal);

   printf("\n-----Opening file-----\n");
   openVal = tfs_openFile("fake.txt");
   printf("tfs_openFile returns fd: %d\n", openVal);

   printf("\n-----Writing 2 blocks of different data to empty file-----\n");
   writeVal = tfs_writeFile(openVal, someOtherBlocks, BLOCKSIZE);
   printf("tfs_writeFile returns: %d\n", writeVal);

   printf("\n-----Moving file pointer to random byte in block 2-----\n");
   seekVal = tfs_seek(openVal, BLOCKSIZE / 2 + 2);
   printf("tfs_seek returns: %d\n", seekVal);

   printf("\n-----Reading random byte from block 2-----\n");
   readVal = tfs_readByte(openVal, &randByte);
   printf("tfs_readByte returns: %d\n", readVal);
   printf("randByte: %d\n", randByte);

   printf("\n------------ Unmounting First File Sytstem------------\n");
   unmountVal = tfs_unmount();
   printf("tfs_unmount return value: %d\n", unmountVal);

   printf("\n------------ Remounting Second File Sytstem------------\n");
   mountVal = tfs_mount("joo.txt");
   printf("tfs_mount return value: %d\n", mountVal);

   printf("\n-----Opening previously written file-----\n");
   openVal = tfs_openFile("fake.txt");
   printf("tfs_openFile returns fd: %d\n", openVal);

   printf("\n-----Moving file pointer to corresponding random byte in block 2-----\n");
   seekVal = tfs_seek(openVal, BLOCKSIZE / 2 + 2);
   printf("tfs_seek returns: %d\n", seekVal);

   printf("\n-----Reading random byte from block 2-----\n");
   readVal = tfs_readByte(openVal, &randByte);
   printf("tfs_readByte returns: %d\n", readVal);
   printf("randByte: %d\n", randByte);
   printf("\n----------------------- MULTI DISK DRIVER COMPLETED ----------------------\n");
}

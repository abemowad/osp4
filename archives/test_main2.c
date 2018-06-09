#include "libDisk.h"
#include "libTinyFS.h"

int openDisk(char *filename, int nBytes);

int main()
{
   SuperBlock superblock;
   InodeBlock inodeblock;
   FreeBlock freeblock;
   int diskNum, writeBlocks, readBlocks, err;
   char writeBuf[256] = "Hello there master Luke";
   char readBuf[256];

   err = tfs_mkfs("moo.txt", 2560); 
   
   fprintf(stderr, "Should be unmounted: %d\n", mountedDisk);
   tfs_mount("moo.txt");
   fprintf(stderr, "Should be mounted: %d\n", mountedDisk);

   fprintf(stderr,"====================\n");
   
   readBlocks = readBlock(mountedDisk, 0, readBuf);

   fprintf(stderr, "total blocks: %d\n", diskTable[0].totalBlocks);
   superblock = *((SuperBlock*)readBuf);
   fprintf(stderr, "sb type: %d\n", superblock.details.type);
   fprintf(stderr, "sb magicnum: %d\n", superblock.details.magicNum);
   fprintf(stderr, "sb rootBlock: %d\n", superblock.rootInodeBlock);
   fprintf(stderr, "sb fileNum: %d\n", superblock.totalFileNum);
   fprintf(stderr, "sb num blocks: %d\n\n", superblock.numBlocks);

   readBlocks = readBlock(mountedDisk, 1, readBuf);
   inodeblock = *((InodeBlock*)readBuf);
   fprintf(stderr, "inode type: %d\n", inodeblock.details.type);
   fprintf(stderr, "inode magicnum: %d\n", inodeblock.details.magicNum);
   fprintf(stderr, "inode filename: %s\n", inodeblock.fileName);

   readBlocks = readBlock(mountedDisk, 5, readBuf);
   freeblock = *((FreeBlock*)readBuf);
   fprintf(stderr, "free type: %d\n", freeblock.details.type);
   fprintf(stderr, "free magicnum: %d\n", freeblock.details.magicNum);

   readBlocks = readBlock(mountedDisk, 11, readBuf);
   fprintf(stderr, "should be error... %d\n", readBlocks);
   freeblock = *((FreeBlock*)readBuf);
   fprintf(stderr, "free type: %d\n", freeblock.details.type);
   fprintf(stderr, "free magicnum: %d\n", freeblock.details.magicNum);

}

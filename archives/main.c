#include "libDisk.h"
#include "libTinyFS.h"

int openDisk(char *filename, int nBytes);

int main()
{
   SuperBlock superblock;
   InodeBlock inodeblock;
   int diskNum, writeBlocks, readBlocks, err;
   char writeBuf[256] = "Hello there master Luke";
   char readBuf[256];

   err = tfs_mkfs("moo.txt", 2560); 
   
   fprintf(stderr, "Should be unmounted: %d\n", mountedDisk);
   tfs_mount("moo.txt");
   fprintf(stderr, "Should be mounted: %d\n", mountedDisk);



   fprintf(stderr,"====================\n");
   
   readBlocks = readBlock(mountedDisk, 0, readBuf);

   superblock = *((SuperBlock*)readBuf);
   fprintf(stderr, "sb type: %d\n", superblock.details.type);
   fprintf(stderr, "sb magicnum: %d\n", superblock.details.magicNum);
   fprintf(stderr, "sb rootBlock: %d\n", superblock.rootInodeBlock);
   fprintf(stderr, "sb fileNum: %d\n", superblock.currFileNum);
   fprintf(stderr, "sb num blocks: %d\n\n", superblock.numBlocks);

   readBlocks = readBlock(mountedDisk, 1, readBuf);
   inodeblock = *((InodeBlock*)readBuf);
   fprintf(stderr, "inode type: %d\n", inodeblock.details.type);
   fprintf(stderr, "inode magicnum: %d\n", inodeblock.details.magicNum);
   fprintf(stderr, "inode filename: %s\n", inodeblock.fileName);
}

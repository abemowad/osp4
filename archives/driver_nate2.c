#include "libDisk.h"
#include "additional_features.h"
#include "libTinyFS.h"

int openDisk(char *filename, int nBytes);

int main()
{
   SuperBlock superblock;
   InodeBlock inodeblock;
   FreeBlock freeblock;
   int i, writeBlocks, readBlocks, err;
   int numbers_fd, letters_fd, more1_fd, more2_fd, more3_fd;
   char writeBuf[5000];
   char readBuf[256];

   /* Sets the data to be written to numbers.txt */
   for (i = 0; i < 5000; i++) {
      writeBuf[i] = i;
   }
   writeBuf[5000] = 0;

   err = tfs_mkfs("moo.txt", 20480); 
   tfs_mount("moo.txt");
   readBlocks = readBlock(mountedDisk, 0, readBuf);
   printf("%d\t%s\n",readBlocks,readBuf);
   
   superblock = *((SuperBlock*)readBuf);
   fprintf(stderr, "sb type: %d\n", superblock.details.type);
   fprintf(stderr, "sb magicnum: %d\n", superblock.details.magicNum);
   fprintf(stderr, "sb rootBlock: %d\n", superblock.rootInodeBlock);
   fprintf(stderr, "sb fileNum: %d\n", superblock.totalFileNum);
   fprintf(stderr, "sb num blocks: %d\n\n", superblock.numBlocks);

   /* Opening a file, writing to it and then reading specific bytes from file.
      Also tests tfs_seek functionality */
   printf("Opening numbers.txt...\n");
   numbers_fd = tfs_openFile("numbers.txt");
   printf("retval is %d\n", numbers_fd);
   writeBlocks = tfs_writeFile(numbers_fd, writeBuf, 5000);
   printf("numbers.txt written to, writeBlocks: %d\n",writeBlocks);
      
   //tfs_readdir();

   /* SHOWING FUNCTIONALITY OF TIMESTAMPS */
   tfs_readFileInfo(numbers_fd);
}

#include "libDisk.h"
#include "additional_features.h"
#include "libTinyFS.h"

int openDisk(char *filename, int nBytes);

int main2()
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

   /* Opens a separate file named letters.txt, writes to it and reads bytes */
   for (i = 0; i < 26; i++) {
      writeBuf[i] = i+97; /* writes all lower case letters to first 26 spots */
   }

   printf("\nOpening letters.txt...\n");
   letters_fd = tfs_openFile("letters.txt");
   tfs_writeFile(letters_fd, writeBuf, 26);
   printf("First byte should be \"a\"... first byte: %c\n", readBuf[0]);
   tfs_seek(letters_fd, 3);
   tfs_readByte(letters_fd, readBuf);
   printf("Third byte should be \"c\"... first byte: %c\n", readBuf[0]);
   tfs_seek(letters_fd, 26);
   tfs_readByte(letters_fd, readBuf);
   printf("26th byte should be \"z\"... first byte: %c\n", readBuf[0]);
   tfs_seek(letters_fd, 30);
   err = tfs_readByte(letters_fd, readBuf);
   printf("30th byte should give error... err code: %d\n", err);

   readBlocks = readBlock(mountedDisk, 3, readBuf);

   printf("%d\t%s\n",readBlocks,&(readBuf[6]));

   /* Opening a file, writing to it and then reading specific bytes from file.
      Also tests tfs_seek functionality */
   printf("Opening numbers.txt...\n");
   numbers_fd = tfs_openFile("numbers.txt");
   printf("retval is %d\n", numbers_fd);
   writeBlocks = tfs_writeFile(numbers_fd, writeBuf, 5000);
   printf("numbers.txt written to, writeBlocks: %d\n",writeBlocks);
      
   tfs_readdir();

   /* SHOWING FUNCTIONALITY OF TIMESTAMPS */
   tfs_readFileInfo(numbers_fd);
   printf("Sleeping for 2 seconds...\n");
   sleep(2);
   printf("Writing to numbers.txt...\n");
   tfs_writeFile(numbers_fd, writeBuf, 26);
   tfs_readFileInfo(numbers_fd);
   printf("Sleeping for 2 seconds...\n");
   sleep(2);
   printf("Reading from numbers.txt...\n");
   tfs_readByte(numbers_fd, readBuf);
   tfs_readFileInfo(numbers_fd);
}

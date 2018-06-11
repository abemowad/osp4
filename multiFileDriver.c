#include "libDisk.h"
#include "additional_features.h"
#include "libTinyFS.h"
#include "multiFileDriver.h"

int openDisk(char *filename, int nBytes);

void multiFileDriver()
{
   SuperBlock superblock;
   InodeBlock inodeblock;
   FreeBlock freeblock;
   int i, writeBlocks, readBlocks, err;
   int numbers_fd, letters_fd, more1_fd, more2_fd, more3_fd;
   char writeBuf[5000];
   char readBuf[256];

   /* Sets the data to be written to numbers.txt */
   for (i = 0; i < 256; i++) {
      writeBuf[i] = i;
   }

   /* Opening a file, writing to it and then reading specific bytes from file.
      Also tests tfs_seek functionality */
   
   printf("\n\n\n----------------------- MULTI FILE DRIVER -------------------\n\n");
   printf(" ----- Opening numbers.txt ----- \n");
   numbers_fd = tfs_openFile("numbers.txt.");
   printf("New file descriptor: %d\n", numbers_fd);
   tfs_writeFile(numbers_fd, writeBuf, 256);
   tfs_readByte(numbers_fd, readBuf);
   printf("First byte should be zero... first byte: %d\n", readBuf[0]);
   tfs_seek(numbers_fd, 100);
   tfs_readByte(numbers_fd, readBuf);
   printf("100th byte should be 100... 100th byte: %d\n", readBuf[0]);

   /* Renames numbers.txt to new_name.txt. All of the byte values should be
   the same */
   printf("\n ******* RENAMING ADDITIONAL FEATURE ********\n");
   printf(" ----- Renaming numbers.txt to new_name.txt ----- \n");
   tfs_rename("numbers.txt", "new_name.txt");
   tfs_seek(numbers_fd, 0);
   tfs_readByte(numbers_fd, readBuf);
   printf("First byte should still be zero after rename... first byte: %d\n", readBuf[0]);
   tfs_seek(numbers_fd, 100);
   tfs_readByte(numbers_fd, readBuf);
   printf("100th byte should still be 100... 100th byte: %d\n", readBuf[0]);

   /* Opens a separate file named letters.txt, writes to it and reads bytes */
   for (i = 0; i < 26; i++) {
      writeBuf[i] = i+97; /* writes all lower case letters to first 26 spots */
   }

   printf("\n ----- Opening letters.txt (size 30 bytes) -----\n");
   letters_fd = tfs_openFile("letters.txt");
   printf("New file descriptor: %d\n", letters_fd);
   tfs_writeFile(letters_fd, writeBuf, 26);
   tfs_seek(letters_fd, 0);
   tfs_readByte(letters_fd, readBuf);
   printf("First byte should be \"a\"... first byte: %c\n", readBuf[0]);
   tfs_seek(letters_fd, 2);
   tfs_readByte(letters_fd, readBuf);
   printf("Third byte should be \"c\"... first byte: %c\n", readBuf[0]);
   tfs_seek(letters_fd, 25);
   tfs_readByte(letters_fd, readBuf);
   printf("26th byte should be \"z\"... first byte: %c\n", readBuf[0]);
   printf("30th byte should give error... \n");
   tfs_seek(letters_fd, 30);
   err = tfs_readByte(letters_fd, readBuf);
   printf("err code: %d\n", err);

   /* new_name.txt still open, should be able to read from it */
   printf("\nnew_name.txt is still open, we should be able to read from it...\n");
   tfs_seek(numbers_fd, 100);
   tfs_readByte(numbers_fd, readBuf);
   printf("100th byte should be 100... 500th byte: %d\n", readBuf[0]);

   /* letters.txt still open, should be able to read from it */
   printf("\nletters.txt is still open, we should be able to read from it...\n");
   tfs_seek(letters_fd, 25);
   tfs_readByte(letters_fd, readBuf);
   printf("26th byte should be \"z\"... 26th byte: %c\n", readBuf[0]);

   
   /* we can open more files and see the file descriptor behavior */
   printf("\nOpening more1.txt...\n");
   more1_fd = tfs_openFile("more1.txt");
   printf("Lowest available file descriptor is 4 (root inode has fd 0)\n");
   printf("New file descriptor: %d\n", more1_fd);
   //tfs_readFileInfo(more1_fd);
   //tfs_writeFile(more1_fd, writeBuf, 25);
   //tfs_readFileInfo(more1_fd);

   printf("\nOpening more2.txt...\n");
   more2_fd = tfs_openFile("more2.txt");
   printf("Lowest available file descriptor is now 5\n");
   printf("more2.txt has fd: %d\n", more2_fd);

   printf("\nClosing new_file.txt...\n");
   tfs_closeFile(numbers_fd);
   printf("new_file.txt closed, lowest available fd is 2 "
       "(single file driver still open)\n");
   printf("Opening more3.txt...\n");
   more3_fd = tfs_openFile("more3.txt");
   printf("more3.txt has fd: %d\n", more3_fd);


   printf("\n-----Reopening new_file.txt... should have same data-----\n");
   numbers_fd = tfs_openFile("new_file.txt.");
   printf("New file descriptor: %d\n", numbers_fd);
   tfs_writeFile(numbers_fd, writeBuf, 256);
   tfs_seek(numbers_fd, 100);
   tfs_readByte(numbers_fd, readBuf);
   printf("100th byte should be 100... 100th byte: %d\n", readBuf[0]);
   
   printf("\n ----- Deleting letters.txt -----\n");
   letters_fd = tfs_deleteFile(letters_fd);

   printf("\n-----Reopening letters.txt... should not have same data-----\n");
   letters_fd = tfs_openFile("letters.txt");

   printf("Attempting read of letters.txt... \n");
   tfs_seek(letters_fd, 0);
   tfs_readByte(letters_fd, readBuf);
   
   printf("\n ******* READDIR ADDITIONAL FEATURE ********\n");
   tfs_readdir();

   printf("Writing 256 bytes to more1.txt\n");
   tfs_writeFile(more1_fd, writeBuf, 256);
   printf("Deleting letters.txt...\n");
   tfs_deleteFile(letters_fd);
   printf("Deleting more2.txt...\n");
   tfs_deleteFile(more2_fd);
   printf("Deleting more3.txt...\n");
   tfs_deleteFile(more3_fd);


   printf("\n ----- New files on disk after closing / deleting ----- \n");
   tfs_readdir();

   /* SHOWING FUNCTIONALITY OF TIMESTAMPS */
   printf("\n ******* TIMESTAMPS ADDITIONAL FEATURE ********\n");
   tfs_readFileInfo(more1_fd);
   printf("\nSleeping 2 seconds and then writing to more1.txt...\n");
   sleep(2); 
   tfs_writeFile(more1_fd, writeBuf, 256);
   tfs_readFileInfo(more1_fd);
   printf("\nSleeping 2 more seconds and then reading from more1.txt...\n");
   sleep(2); 
   tfs_readByte(more1_fd, readBuf);
   tfs_readFileInfo(more1_fd);
}

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

   err = tfs_mkfs("moo.txt", 10240); 
   tfs_mount("moo.txt");
   
   /* Opening a file, writing to it and then reading specific bytes from file.
      Also tests tfs_seek functionality */
   printf("Opening numbers.txt...\n");
   numbers_fd = tfs_openFile("numbers.txt.");
   sleep(2);
   tfs_writeFile(numbers_fd, writeBuf, 5000);
   tfs_readByte(numbers_fd, readBuf);
   /*printf("First byte should be zero... first byte: %d\n", readBuf[0]);
   tfs_seek(numbers_fd, 500);
   tfs_readByte(numbers_fd, readBuf);
   printf("500th byte should be 500... 500th byte: %d\n", readBuf[0]);
   tfs_seek(numbers_fd, 1234);
   tfs_readByte(numbers_fd, readBuf);
   printf("1234th byte should be 1234... 1234th byte: %d\n", readBuf[0]);
   tfs_seek(numbers_fd, 5001);
   err = tfs_readByte(numbers_fd, readBuf);
   printf("5001st byte should give error... err code: %d\n", err);
   */

   /*

   /* Renames numbers.txt to new_name.txt. All of the byte values should be
      the same *
   printf("\nRenaming numbers.txt to new_name.txt...\n");
   tfs_rename("numbers.txt", "new_name.txt");
   printf("First byte should be zero... first byte: %d\n", readBuf[0]);
   tfs_seek(numbers_fd, 500);
   tfs_readByte(numbers_fd, readBuf);
   printf("500th byte should be 500... 500th byte: %d\n", readBuf[0]);
   tfs_seek(numbers_fd, 1234);
   tfs_readByte(numbers_fd, readBuf);
   printf("1234th byte should be 1234... 1234th byte: %d\n", readBuf[0]);
   tfs_seek(numbers_fd, 5001);
   err = tfs_readByte(numbers_fd, readBuf);
   printf("5001st byte should give error... err code: %d\n", err);


   /* Opens a separate file named letters.txt, writes to it and reads bytes *
   for (i = 0; i < 26; i++) {
      writeBuf[i] = i+97; /* writes all lower case letters to first 26 spots *
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

   /* new_name.txt is still open, we should be able to read from it *
   tfs_seek(numbers_fd, 500);
   tfs_readByte(numbers_fd, readBuf);
   printf("500th byte should be 500... 500th byte: %d\n", readBuf[0]);

   /* letters.txt still open, should be able to read from it *
   tfs_seek(letters_fd, 26);
   tfs_readByte(letters_fd, readBuf);
   printf("26th byte should be \"z\"... first byte: %c\n", readBuf[0]);

   
   /* we can open more files and see the file descriptor behavior */
   printf("\nOpening more1.txt...\n");
   more1_fd = tfs_openFile("more1.txt");
   tfs_readFileInfo(more1_fd);
   sleep(2);
   tfs_writeFile(more1_fd, writeBuf, 26);
   tfs_readFileInfo(more1_fd);
   printf("Lowest available file descriptor is 3 (root inode has fd 0)\n");
   printf("more1.txt has fd: %d\n", more1_fd);

   printf("Opening more2.txt...\n");
   more2_fd = tfs_openFile("more2.txt");
   printf("Lowest available file descriptor is now 4\n");
   printf("more2.txt has fd: %d\n", more2_fd);

   printf("Closing new_file.txt...\n");
   tfs_closeFile(numbers_fd);
   printf("new_file.txt closed, lowest available fd is 1\n");
   printf("\nOpening more3.txt...\n");
   more3_fd = tfs_openFile("more3.txt");
   printf("more3.txt has fd: %d\n", more3_fd);

   
   printf("\n*********");
   printf("\nReading info all files in the disk...\n");
   tfs_readdir();
   printf("\n*********\n");

   printf("Deleting more2.txt...\n");
   tfs_deleteFile(more2_fd);
   printf("Lowest available file descriptor is now %d\n", more2_fd);

   printf("\nReading info all files in the disk...\n");
   tfs_readdir();

   /* SHOWING FUNCTIONALITY OF TIMESTAMPS */
   tfs_readFileInfo(more1_fd);
   tfs_readFileInfo(numbers_fd);
}

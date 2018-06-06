#include "libDisk.h"
#include "TinyFS_errno.h"

int totalDisks = 0;
DiskTableEntry *diskTable;

/* Searches through the disk table to find a disk with the given filename. If 
a disk exists, returns the disk number, otherwise returns -1 for no disk with
that filename */
int openExistingDisk(char *filename)
{
   int disk;

   for (disk = 0; disk < totalDisks; disk++) {
      fprintf(stderr,"%s\n", diskTable[disk].filename);
      if (!(strcmp(diskTable[disk].filename, filename))) {
         diskTable[disk].diskFile = fopen(filename, "r+"); 
         return diskTable[disk].diskNum;
      }
   }
   return -1; 
}

/* Opens a new disk and adds an entry to the disk table */
int openNewDisk(char *filename, int nBytes)
{
   DiskTableEntry *newDisk;
   totalDisks++;

   if (totalDisks == 1) { /* just added the first disk in the disk table */
      if (NULL == (diskTable = (DiskTableEntry*)malloc(sizeof(DiskTableEntry)))) {
         perror(NULL);
         return -1;
      }
   }

   else {
      if (realloc(diskTable, sizeof(DiskTableEntry)*totalDisks) == NULL) {
         perror(NULL);
         return -1;
      }
   }

   newDisk = diskTable+(totalDisks-1); /* points to the newly added disk */

   if (NULL == (newDisk->diskFile = fopen(filename, "w+"))) {
      perror(NULL);
      return -1;
   }
   truncate(newDisk->filename, nBytes);
   newDisk->filename = filename;
   newDisk->diskNum = totalDisks-1;
   newDisk->isMounted = 0;

   fprintf(stderr, "total disks: %i\n", totalDisks);
   fprintf(stderr, "file: %s\n", diskTable[totalDisks-1].diskFile);
   fprintf(stderr, "newest filename from table: %s\n", diskTable[totalDisks-1].filename);

   return newDisk->diskNum;
}

/* Truncates the existing disk in the disk table to the given number of bytes */
int truncateExistingDisk(int diskNum, int nBytes) 
{
   /* erases the file and treats as new file */
   if (NULL == (diskTable[diskNum].diskFile = fopen(diskTable[diskNum].filename, "w+"))) {
      perror(NULL);
      return -1;
   }
   if (truncate(diskTable[diskNum].filename, nBytes) == -1) {
      return -1;
   }

   return diskTable[diskNum].diskNum;
}

int openDisk(char *filename, int nBytes)
{
   int diskNum, truncErr;
   
   if (nBytes == 0) {
      return openExistingDisk(filename);
   }

   else if (nBytes > 0)
   {
      if ((diskNum = openExistingDisk(filename)) == -1) { 
         return openNewDisk(filename, nBytes);
      }
      else {
         return truncateExistingDisk(diskNum, nBytes);
      }
   }

   else {
      return -1;
   }
}

int readBlock(int disk, int bNum, void *block)
{
   int offset = bNum * BLOCKSIZE;
   char *filename;
   FILE *diskFile;

   if (!(diskTable[disk].isMounted) || (disk >= totalDisks)) {
      return diskClosedErr;
   }

   filename = diskTable[0].filename;
   if (NULL == (diskFile = fopen(filename, "r+"))) {
      perror(NULL);
   }
   fprintf(stderr, "%s\n", filename);
   if (fseek(diskFile, offset, SEEK_SET) == -1) {
      return outOfDiskBoundsErr;
   }
   fread(block, BLOCKSIZE, 1, diskFile);

   return 0;
}

/*
int writeBlock(int disk, int bNum, void *block)
{
   SuperBlock superBlock = (SuperBlock)diskTable[disk][0]; 

   if (superBlock.isClosed) {
      return DISKCLOSED; 
   }

   diskTable[disk][bNum] = *((FreeBlock*)block);

   return 0;
}
*/

/* closeDisk() takes a disk number ‘disk’ and makes the disk closed to 
 * further I/O; i.e. any subsequent reads or writes to a closed disk should
 * return an error. Closing a disk should also close the underlying file, 
 * committing any writes being buffered by the real OS. */
/*void closeDisk(int disk)
{
   SuperBlock superBlock = (SuperBlock)diskTable[disk][0]; 
   superBlock.isClosed = 1;
}*/ 

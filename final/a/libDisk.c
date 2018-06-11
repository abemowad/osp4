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

   if ((diskTable = realloc(diskTable, sizeof(DiskTableEntry)*totalDisks)) == NULL) {
      perror(NULL);
      return -1;
   }

   newDisk = diskTable+(totalDisks-1); /* points to the newly added disk */

   if (NULL == (newDisk->diskFile = fopen(filename, "w+"))) {
      perror(NULL);
      return -1;
   }

   
   newDisk->filename = filename;
   newDisk->diskNum = totalDisks-1;
   newDisk->isMounted = 0; 

   if (truncate(newDisk->filename, nBytes) == -1) {
      perror(NULL);
      return -1;
   }

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
      perror(NULL);
      return -1;
   }

   return diskTable[diskNum].diskNum;
}

int openDisk(char *filename, int nBytes)
{
   int diskNum, truncErr;
   
   if (nBytes == 0) {
      diskNum = openExistingDisk(filename);
   
      if (diskNum == -1) {
         printf("No disk associated with %s\n", filename);
         return -1;
      }
   }

   else if (nBytes > 0)
   {
      /* if search for given filename in disk table returns none (-1), opens a new disk */
      if ((diskNum = openExistingDisk(filename)) == -1) { 
         diskNum = openNewDisk(filename, nBytes);
      }
      else {
         diskNum = truncateExistingDisk(diskNum, nBytes);
      }
      diskTable[diskNum].totalBlocks = nBytes/BLOCKSIZE; 
   }

   else {
      return -1;
   }
   if (diskNum > -1) {
      diskTable[diskNum].isMounted = 1; 
   }
   
   return diskNum;
}

int readBlock(int disk, int bNum, void *block)
{
   int offset = bNum * BLOCKSIZE, readSize;
   char *filename = diskTable[disk].filename;
   FILE *diskFile = diskTable[disk].diskFile;

   if (!(diskTable[disk].isMounted) || (disk >= totalDisks)) {
      return diskClosedErr; /* either disk not mounted or out of range of table */ }

   if (bNum >= diskTable[disk].totalBlocks) {
      return outOfDiskBoundsErr; /* past the bounds of the current disk size */
   }

   if (fseek(diskFile, offset, SEEK_SET) == -1) {
      return outOfDiskBoundsErr; /* past the bounds of the current disk size */
   }

   readSize = fread(block, BLOCKSIZE, 1, diskFile);

   return 0;
}

int writeBlock(int disk, int bNum, void *block)
{
   int offset = bNum * BLOCKSIZE, writeSize;
   char *filename = diskTable[disk].filename;
   FILE *diskFile = diskTable[disk].diskFile;


   //printf("total blocks: %d block: %d \n", diskTable[disk].totalBlocks, bNum);
   if (!(diskTable[disk].isMounted) || (disk >= totalDisks)) {
      return diskClosedErr; /* either disk not mounted or out of range of table */
   }

   if (bNum >= diskTable[disk].totalBlocks) {
      return outOfDiskBoundsErr; /* past the bounds of the current disk size */
   }

   if (fseek(diskFile, offset, SEEK_SET) == -1) {
      return outOfDiskBoundsErr; /* past the bounds of the current disk size */
   }

   writeSize = fwrite(block, BLOCKSIZE, 1, diskFile);

   return 0;
}

/* closeDisk() takes a disk number ‘disk’ and makes the disk closed to 
 * further I/O; i.e. any subsequent reads or writes to a closed disk should
 * return an error. Closing a disk should also close the underlying file, 
 * committing any writes being buffered by the real OS. */
void closeDisk(int disk)
{
   FILE *diskFile = diskTable[disk].diskFile;

   diskTable[disk].isMounted = 0;
   fclose(diskFile);
}

void printEntry(int disk) {
   if (diskTable != NULL) {
      fprintf(stderr, "------------: \n");
      fprintf(stderr, "filename: %s\n", diskTable[disk].filename);
      //fprintf(stderr, "diskfile: %x\n", diskTable[disk].diskFile);
      fprintf(stderr, "disknum: %i\n", diskTable[disk].diskNum);
      fprintf(stderr, "ismounted: %i\n", diskTable[disk].isMounted);
      fprintf(stderr, "------------: \n");
   }
   else {
      fprintf(stderr, "empty \n");
   }
}


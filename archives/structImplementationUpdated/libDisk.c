#include "libDisk.h"
#include "TinyFS_errno.h"

int curDiskNum = 0;
DiskTableEntry *diskTable;

int openDisk(char *filename, int nBytes)
{
   if (access(filename, R_OK & W_OK) == 0) {  
      if (nBytes > 0) {
         if (truncate(filename, nBytes) != 0) {
            perror(NULL);
            return -1;
         }
      }
      else if (nBytes == 0) {
         open(filename, O_RDWR);
      }
   }

   else if (!(access(filename, F_OK))) {  /* no file currently exists, need to create new one */
      if (nBytes > 0) {
         open(filename, O_RDWR);
      }
   }

   else {  /* File exists but permissions are denied */
      perror(NULL);
      return -1;
   }

   return curDiskNum++; 
}


int readBlock(int disk, int bNum, void *block)
{
   SuperBlock superBlock = (SuperBlock)diskTable[disk][0]; 
   FreeBlock diskBlock = diskTable[disk][bNum]; /* Block offset within the specified disk */

   if (superBlock.isClosed) {
      return DISKCLOSED;  /* need more specific behavior with error codes? */
   }

   *((FreeBlock*)block) = diskBlock;

   return 0;
}


int writeBlock(int disk, int bNum, void *block)
{
   SuperBlock superBlock = (SuperBlock)diskTable[disk][0]; 

   if (superBlock.isClosed) {
      return DISKCLOSED;  /* need more specific behavior with error codes? */
   }

   diskTable[disk][bNum] = *((FreeBlock*)block);

   return 0;
}

/* closeDisk() takes a disk number ‘disk’ and makes the disk closed to 
 * further I/O; i.e. any subsequent reads or writes to a closed disk should
 * return an error. Closing a disk should also close the underlying file, 
 * committing any writes being buffered by the real OS. */
void closeDisk(int disk)
{
   SuperBlock superBlock = (SuperBlock)diskTable[disk][0]; 
   superBlock.isClosed = 1;
} 

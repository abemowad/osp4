#include "libDisk.h"
#include "TinyFS_errno.h"

int curDiskNum = 0;
DiskTableEntry *diskTable;

int openDisk(char *filename, int nBytes)
{
   int fd, truncErr;
 
   if (nBytes == 0)
      fd = open(filename, O_RDONLY);
   else if (nBytes > 0)
   {
      if ((fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, WRITE_FLAG)) != -1)
         truncErr = truncate(filename, nBytes);
   }
   else
      return -1;

   if (fd == -1 || truncErr == -1)
   {
      perror(NULL);
      return -1;
   }

   return fd;
}

/*
int readBlock(int disk, int bNum, void *block)
{
   SuperBlock superBlock = (SuperBlock)diskTable[disk][0]; 
   FreeBlock diskBlock = diskTable[disk][bNum]; 

   if (superBlock.isClosed) {
      return DISKCLOSED;  
   }

   *((FreeBlock*)block) = diskBlock;

   return 0;
}
*/

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

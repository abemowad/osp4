#include "libDisk.h"

int curDiskNum = 0;
FreeBlock **diskTable;


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
      
}


int writeBlock(int disk, int bNum, void *block)
{

}

/* closeDisk() takes a disk number ‘disk’ and makes the disk closed to 
 * further I/O; i.e. any subsequent reads or writes to a closed disk should
 * return an error. Closing a disk should also close the underlying file, 
 * committing any writes being buffered by the real OS. */
void closeDisk(int disk)
{
   
}  

#include <stdio.h>
#include <stdlib.h>

#define INODE_FILESIZE_OFFSET 15
#define INODE_FP_OFFSET       17 

/* checks if fp points within the bytes range of the given fd */
int validFilePointer(fileDescriptor FD, int fp, int fpError)
{
   int sizeByteIndex;
   
   sizeByteIndex = getInodeProperty(FD, INODE_FILESIZE_OFFSET);
}

/* finds starting byte in disk file of a given inode property for a given fd,
 * (inode property offsets are defined as macros) 
 * eg. returns starting byte number of the file pointer for fd 2 */
int getInodeProperty(fileDescriptor FD, int propetyOffset)
{
   unsigned short inodeBlock;
   int propertyIndex;

   /* what's actual var for fileTable? */
   inodeBlock = fileTable[FD];
   propertyIndex = inodeBlock * BLOCKSIZE + propertyOffset;

   return propertyIndex;
}

/* returns file pointer of input fd or -1 upon failure */
int getFP(fileDescriptor FD)
{
   int fpByteIndex, fp, fpError;

   fpByteIndex = getInodeProperty(FD, INODE_FP_OFFSET);

   /* where is diskFp? */
   if (fseek(diskFp, fpByteIndex, SEEK_SET) < 0)
   {
      perror(NULL);
      return -1;
   }

   if(!fread(&fp, sizeof(int), 1, diskFp))
   {
      perror(NULL);
      return -1;
   }

   return fp;
}

/* sets file pointer of input fd, returns 0 upon success or -1 upon failure */
int setFP(fileDescriptor FD, int fp)
{
   int fpByteIndex, fpError;

   fpByteIndex = getInodeProperty(FD, INODE_FP_OFFSET);

   /* where is diskFP? */
   if (fseek(diskFp, fpByteIndex, SEEK_SET) < 0)
   {
      perror(NULL);
      return -1;
   }

   if (!fwrite(&fp, sizeof(int), 1, diskFp))
   {
      perror(NULL);
      return -1;
   }

   return 0;
}

/* change the file pointer location to offset (absolute). Returns success/error codes.*/
int tfs_seek(fileDescriptor FD, int offset)
{
   int fp;

   if ((fp = getFP(FD)) == -1)
      return -1;

   fp += offset;
 
   if (!validFilePointer(FD, fp))
      return -1;

   if (setFP(FD, fp) == -1)
      return -1;

   return 0;
}

#include "block.h"
#include "TinyFS_errno.h"
#include "libTinyFS.h"
#include "libDisk.h"

/******************************************************************************
                        DIRECTORY LISTING AND RENAMING
******************************************************************************/

/* Renames a file based on a passed in new name. Returns 0 on success
and a negative number signifying an error */
int tfs_rename(const char *oldName, const char *newName)
{
   int i; 
   int totalFiles = diskTable[mountedDisk].superBlock.totalFileNum;
   InodeBlock *inode;

   if (strlen(newName) > 8) {
      return fileNameTooBigErr;
   }

   for (i = 0; i < totalFiles; i++) {
      inode = &(diskTable[mountedDisk].inodeTable[i]);
      if (!strcmp(oldName, inode->fileName)) {
         strcpy(inode->fileName, newName); 
         return 0;
      } 
   }
   return fileNotFoundErr;
}

/* Lists all the files and directories on the disk */
int tfs_readdir()
{
   int i = 0, curFile = 0, totalFiles;
   InodeBlock inode;


   if (mountedDisk < 0) {
      return noMountedDiskErr;
   }

   totalFiles = diskTable[mountedDisk].superBlock.totalFileNum;

   while (i < SUPER_TABLE_SIZE) {

      /* file exists (however may be closed) */
      inode = diskTable[mountedDisk].inodeTable[i];
      if (inode.fileSize > -1) {
         printf("Filename: %s\n", inode.fileName);
         printf("File size: %d bytes\n", inode.fileSize);

         if (inode.isClosed) {
            printf("Closed\n\n");
         }
         else {
            printf("Open\n\n");
         }

      }
      i++; 
   }
}

/******************************************************************************
                                  TIMESTAMPS
******************************************************************************/

int tfs_readFileInfo(fileDescriptor FD)
{
   InodeBlock inode;

   if (mountedDisk < 0) {
      return noMountedDiskErr;
   }
   inode = diskTable[mountedDisk].inodeTable[FD];
   char *created_str = ctime(&inode.timestamp.created);
   char *accessed_str = ctime(&inode.timestamp.accessed);
   char *modified_str = ctime(&inode.timestamp.modified);

   printf("\n%s\n", inode.fileName);
   printf("Created: %s\n", created_str);
   printf("Accessed: %s\n", accessed_str);
   printf("Created: %s\n", modified_str);

   return 0;
}


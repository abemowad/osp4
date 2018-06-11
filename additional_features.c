#include "additional_features.h"

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

   printf("Displaying all file names and sizes currently in disk\n");
   while (i < SUPER_TABLE_SIZE) {

      /* file exists (however may be closed) */
      inode = diskTable[mountedDisk].inodeTable[i];
      if (inode.fileSize > -1) {
         printf("Filename: %s\n", inode.fileName);
         printf("File size: %d bytes\n\n", inode.fileSize);


      }
      i++; 
   }
   return 0;
}

/******************************************************************************
                                  TIMESTAMPS
******************************************************************************/

int tfs_readFileInfo(fileDescriptor FD)
{
   InodeBlock *inode;
   char created_str[26];
   char accessed_str[26];
   char modified_str[26];
   
   if (mountedDisk < 0) {
      return noMountedDiskErr;
   }
   inode = &(diskTable[mountedDisk].inodeTable[FD]);

   char *none = ctime_r(&(inode->timestamp.created), created_str);
   none = ctime_r(&(inode->timestamp.accessed), accessed_str);
   none = ctime_r(&(inode->timestamp.modified), modified_str);

   printf("\n%s\n", inode->fileName);
   printf("Created: %s", created_str);
   printf("Accessed: %s", accessed_str);
   printf("Modified: %s", modified_str);

   return 0;
}


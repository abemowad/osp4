#include "libTinyFS.h"
#include "TinyFS_errno.h"
#include "block.h"
#include "libDisk.h"

/* Disk number of the currently mounted disk */
int mountedDisk = -1;

/* Initializes a free block with the magic number and writes the free block to
each block in the disk */
int clear_block(int diskNum, int blockNum)
{
   char freeBlock[BLOCKSIZE];
   memset(freeBlock, 0, 256);
   freeBlock[0] = 4;
   freeBlock[1] = MAGIC_NUM;
   return writeBlock(diskNum, blockNum, freeBlock);
}

/* Initializes a superblock for the disk and writes the block to the file 
at block 0 of the disk */
void init_superblock(int diskNum, int nBytes)
{
   SuperBlock superblock = {0};
   superblock.details.type = 1;
   superblock.details.magicNum = 45;
   superblock.details.next = -1; 
   superblock.currFileNum = 1; /* root inode is first */
   superblock.isClosed = 1;
   superblock.numBlocks = nBytes / BLOCKSIZE;

   /* root inode will always reside in the block after super block (at block 1) */
   superblock.fileTable[0] = 1;

   fprintf(stderr,"writing superblock at disk %d\n", diskNum);

   writeBlock(diskNum, 0, (void*)(&superblock));
}

/* Initializes a root inode for the disk and writes the block to the file at 
block 1 of the disk*/
void init_root(int diskNum)
{
   char buffa[256] = "hello world";

   InodeBlock inode = {0};
   inode.details.type = 2;
   inode.details.magicNum = 45;
   inode.details.next = -1; /* does root have next block? */
   strcpy(inode.fileName, "/");
   inode.fileSize = 0; /* does root have a file size? */
   inode.lastIndex = 0; /* does root have a file size? */

   /* Sets the first inode in the inode table of disk to be the root inode (fd 0) */
   diskTable[diskNum].inodeTable[0] = inode;

   writeBlock(diskNum, 1, (void*)(&inode));
}

/* Makes a blank TinyFS file system of size nBytes on the file specified by
‘filename’. This function should use the emulated disk library to open the
specified file, and upon success, format the file to be mountable. This
includes initializing all data to 0x00, setting magic numbers, initializing
and writing the superblock and inodes, etc. Must return a specified
success/error code. */
int tfs_mkfs(char *filename, int nBytes)
{
   InodeBlock inode = {0};
   int i, diskNum, blocks = nBytes / BLOCKSIZE;
   char writeBuffer[BLOCKSIZE];
   
   if ((diskNum = openDisk(filename, nBytes)) == -1) {
      return diskOpenError;
   }

   /* loops through each block in disk and sets it to a free block */
   for (i = 0; i < blocks; i++) {
      if ((err = clear_block(mountedDisk, i)) != 0) {
         return err;
   }

   /* initializes the root inode in the disk */
   init_root(diskNum);

   init_superblock(diskNum, nBytes);
   
   closeDisk(diskNum);
   return 0;
}

/* tfs_mount(char *filename) "mounts" a TinyFS file system located within
‘filename'. fs_unmount(void) “unmounts” the currently mounted file system.
As part of the mount operation, tfs_mount should verify the file system is
the correct type. Only one file system may be mounted at a time. Use
tfs_unmount to cleanly unmount the currently mounted file system. Must return
a specified success/error code. */
int tfs_mount(char *filename)
{
   int err;
   if (mountedDisk > -1) {
      if ((err = tfs_unmount()) != 0) {
         return err;
      }
   }

   if ((mountedDisk = openDisk(filename, 0)) == -1) {
      return -1;
   }
   return 0;
}

int tfs_unmount(void)
{
   if (mountedDisk < 0) {
      return noMountedDiskErr;
   }

   closeDisk(mountedDisk);
   mountedDisk = -1;
   return 0;
}

/* Updates the superblock's file table when a file has been either closed
or removed from the disk. */
int update_SB_fileTable(fileDescriptor FD)
{
   int err;
   SuperBlock *sb_ptr;
   char IOBuffer[256];

   if ((err = readBlock(mountedDisk, 0, IOBuffer)) != 0) {
      return err;   
   }

   /* need to change the FD entry in the super table to be -1 for closed */
   sb_ptr = (SuperBlock*)IOBuffer;
   sb_ptr->fileTable[FD] = -1; /* changes fd table so that file is closed */

   if ((err = writeBlock(mountedDisk, 0, IOBuffer)) != 0) {
      return err;   
   }
   return 0;
}

/* Closes the file, de-allocates all system/disk resources, and removes table
entry */
int tfs_closeFile(fileDescriptor FD)
{
   /* ---TO DO:---
    * Still need to update the isClosed variable on the inode actually on disk,
    * Not sure where this inode is on disk
    */
   int err;

   if (mountedDisk < 0) {
      return noMountedDiskErr;
   }

   if ((err = update_SB_fileTable(FD)) != 0) {
      return err;
   }
   
   diskTable[mountedDisk].inodeTable[FD].isClosed = 1;
   return 0;
}

/* Sets all of the file extent block associated with a file (inode) to be free */
int set_freeBlocks(InodeBlock inode)
{
   int i, err;
   FileExtentBlock dataBlock;

   for (i = 0; i < numBlocks; i++) {
      /* Clears each of the file extent blocks associated with the file, assuming
      data is contiguous and that inode's next points to first file ext block */
      if ((err = clear_block(mountedDisk, inode.details.next+i)) != 0) {
         return err;
      }
   }
   return 0;
}

/* deletes a file and marks its blocks as free on disk. */
int tfs_deleteFile(fileDescriptor FD)
{
   int err;
   InodeBlock inode = diskTable[mountedDisk].inodeTable[FD];

   if (mountedDisk < 0) {
      return noMountedDiskErr;
   }

   if ((err = update_SB_fileTable(FD)) != 0) {
      return err;
   }

   /* deletes all of the blocks in the file and sets them to free blocks */
   if ((err = set_freeBlocks(inode)) != 0) {
      return err;
   }
   
   diskTable[mountedDisk].inodeTable[FD].isClosed = 1;
   diskTable[mountedDisk].inodeTable[FD].fileSize = -1; /* signifies freed up inode */

   return 0;
}

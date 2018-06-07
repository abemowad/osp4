#include "libTinyFS.h"
#include "TinyFS_errno.h"

/* Disk number of the currently mounted disk */
int mountedDisk = -1;

/* sets up a buffer than will write free blocks to entire disk */
void clear_disk(disk, blocks)
{
   char freeBlock[BLOCKSIZE];
   memset(freeBlock, 0, 256);
   freeBlock[0] = 4;
   freeBlock[1] = 45;
   for (i = 0; i < blocks, i++) {
      writeBlock(diskNum, i, freeBlock);
   }
   return 0;

}

void init_superblock(int diskNum)
{
   SuperBlock superblock = {0};
   superblock.details.type = 1;
   superblock.details.magicNum = 45;
   superblock.details.next = NULL;
   superblock.currFileNum = 1; /* root inode is first */
   superblock.isClosed = 1;
   superblock.numBlocks = nBytes / BLOCKSIZE;

   /* root inode will always reside in the block after super block */
   superblock.fileTable[0] = 1;

   writeBlock(diskNum, 0, (void*)(&superblock));
}

void init_root(int diskNum)
{
   InodeBlock inode = {0};
   inode.details.type = 2;
   inode.details.magicNum = 45;
   inode.details.next = NULL; /* does root have next block? */
   inode.fileName = "/";
   inode.fileSize = 0; /* does root have a file size? */
   inode.lastIndex = 0; /* does root have a file size? */

   writeBlock(diskNum, 0, (void*)(&inode));
}

void init_inode(const char *filename)
{

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
   int i, blocks = nBytes / BLOCKSIZE;
   char writeBuffer[BLOCKSIZE];
   
   diskNum = openDisk(filename, nBytes);
   clear_disk(diskNum, blocks);

   /* initializes the root inode in the disk */
   init_inode(diskNum);

   init_superblock(diskNum);
   
   
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

/* Opens a file for reading and writing on the currently mounted file system.
Creates a dynamic resource table entry for the file, and returns a file
descriptor (integer) that can be used to reference this file while the
filesystem is mounted. */
fileDescriptor tfs_openFile(char *name)
{

}

/* Closes the file, de-allocates all system/disk resources, and removes table
entry */
int tfs_closeFile(fileDescriptor FD)
{

}

/* Writes buffer ‘buffer’ of size ‘size’, which represents an entire file’s
content, to the file system. Sets the file pointer to 0 (the start of file)
when done. Returns success/error codes. */
int tfs_writeFile(fileDescriptor FD, char *buffer, int size);

/* deletes a file and marks its blocks as free on disk. */
int tfs_deleteFile(fileDescriptor FD)
{

}

/* reads one byte from the file and copies it to buffer, using the current
file pointer location and incrementing it by one upon success. If the file
pointer is already at the end of the file then tfs_readByte() should return
an error and not increment the file pointer. */
int tfs_readByte(fileDescriptor FD, char *buffer);

/* change the file pointer location to
success/error codes.*/
int tfs_seek(fileDescriptor FD, int offset);


#include "libTinyFS.h"
#include "TinyFS_errno.h"
#include "block.h"
#include "libDisk.h"

/* Disk number of the currently mounted disk */
int mountedDisk = -1;

/* Initializes a free block with the magic number and writes the free block to
each block in the disk */
void clear_disk(int diskNum, int blocks)
{
   int i;
   char freeBlock[BLOCKSIZE];
   memset(freeBlock, 0, 256);
   freeBlock[0] = 4;
   freeBlock[1] = MAGIC_NUM;
   for (i = 0; i < blocks; i++) {
      writeBlock(diskNum, i, freeBlock);
   }
}

/* Initializes a superblock for the disk and writes the block to the file */
void init_superblock(int diskNum, int nBytes)
{
   SuperBlock superblock = {0};
   superblock.details.type = 1;
   superblock.details.magicNum = 45;
   superblock.details.next = -1; 
   superblock.currFileNum = 1; /* root inode is first */
   superblock.isClosed = 1;
   superblock.numBlocks = nBytes / BLOCKSIZE;

   /* root inode will always reside in the block after super block */
   superblock.fileTable[0] = 1;

   fprintf(stderr,"writing superblock at disk %d\n", diskNum);

   writeBlock(diskNum, 0, (void*)(&superblock));
}

/* Initializes a root inode for the disk and writes the block to the file */
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

   //fprintf(stderr,"writing %s to disk %d\n", buffa, diskNum);
   writeBlock(diskNum, 1, (void*)(&inode));
   //writeBlock(diskNum, 1, buffa);

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
   int i, diskNum, blocks = nBytes / BLOCKSIZE;
   char writeBuffer[BLOCKSIZE];
   
   if ((diskNum = openDisk(filename, nBytes)) == -1) {
      return diskOpenError;
   }
   clear_disk(diskNum, blocks);

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


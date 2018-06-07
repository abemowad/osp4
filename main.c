#include "libDisk.h"
#include "libTinyFS.h"

int openDisk(char *filename, int nBytes);

int main()
{
   int diskNum, writeBlocks, readBlocks;
   char writeBuf[256] = "Hello there master Luke";
   char readBuf[256];
   
   diskNum = openDisk("too.txt", 1024);


   fprintf(stderr, "Disk 0 mount status: %d\n", diskTable[0].isMounted);
   tfs_unmount();
   fprintf(stderr, "mountedDisk: %d\n", mountedDisk);
   fprintf(stderr, "Disk 0 mount status: %d\n", diskTable[0].isMounted);
   tfs_mount("too.txt");
   fprintf(stderr, "mountedDisk: %d\n", mountedDisk);
   fprintf(stderr, "Disk 0 mount status: %d\n", diskTable[0].isMounted);

   diskNum = openDisk("moo.txt", 1024);

   fprintf(stderr, "mountedDisk: %d\n", mountedDisk);

   fprintf(stderr, "mountedDisk: %d\n", mountedDisk);
   tfs_mount("moo.txt");
   fprintf(stderr, "mountedDisk: %d\n", mountedDisk);

   tfs_mount("too.txt");
   fprintf(stderr,"====================\n");
   writeBlocks = writeBlock(0, 0, writeBuf);
   fprintf(stderr, "Written: %d\n", writeBlocks);
   
   readBlocks = readBlock(0, 0, readBuf);
   fprintf(stderr, "Read: %d\n", readBlocks);
   fprintf(stderr, "Read: %s\n", readBuf);
}

#include <stdio.h>
#include <stdlib.h>

#include "libTinyFS.h"

int main()
{
   printf("tfs_mkfs: %d\n", tfs_mkfs("too.txt", DEFAULT_DISK_SIZE));
   printf("tfs_mount: %d\n", tfs_mount("moo.txt"));
   printf("tfs_openFile: %d\n", tfs_openFile("fake.txt"));
   return 0;
}

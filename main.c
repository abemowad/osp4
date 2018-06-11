#include <stdio.h>
#include <stdlib.h>

#include "block.h"
#include "singleFileDriver.h"
#include "multiFileDriver.h"
#include "multiDiskDriver.h"

int main()
{
   singleFileDriver();
   printf("\n\n\n\n\n\n\n\n\n\n");
   multiFileDriver();
   printf("\n\n\n\n\n\n\n\n\n\n");
   multiDiskDriver();
   return 0;
}

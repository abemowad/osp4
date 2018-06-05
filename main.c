#include <stdio.h>

int main()
{
   int val;

   val = openDisk("too.txt", 100);

   printf("%d\n", val);
   return 0;
}

#ifndef TINYFS_ERRNO_H
#define TINYFS_ERRNO_H

/*
 * A file containing a set of unified eror codes returned by the TinyFS
 * interfaces. In addition, macros for the project are defined here.
 */


#define BLOCKSIZE 256
#define MAGIC_NUM 45

#define diskClosedErr -2
#define outOfDiskBoundsErr -3
#define diskReadErr -4
#define noMountedDiskErr -5
#define diskOpenError -6
#define fileNameTooBigErr -7
#define fileNotFoundErr -8


#endif

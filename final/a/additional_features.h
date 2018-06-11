#ifndef ADDITIONAL_FEATURES_H
#define ADDITIONAL_FEATURES_H

#include "block.h"
#include "TinyFS_errno.h"
#include "libTinyFS.h"
#include "libDisk.h"

/******************************************************************************
                        DIRECTORY LISTING AND RENAMING
******************************************************************************/

/* Renames a file based on a passed in new name. Returns 0 on success
and a negative number signifying an error */
int tfs_rename(const char *oldName, const char *newName);


/* Lists all the files and directories on the disk */
int tfs_readdir();
/******************************************************************************
                                  TIMESTAMPS
******************************************************************************/

int tfs_readFileInfo(fileDescriptor FD);

#endif

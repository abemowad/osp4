# build a program from 2 files and one shared headeri
CC = gcc
CFLAGS = -Wall -pedantic

tinyFSDemo: main.c libDisk.c additional_features.c multiDiskDriver.c multiFileDriver.c singleFileDriver.c libTinyFS_1.c libTinyFS_2.c 

$(CC) $(CFLAGS) main.o libDisk.o additional_features.o multiDiskDriver.o multiFileDriver.o singleFileDriver.o libTinyFS_1.o libTinyFS_2.o

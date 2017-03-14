CC = gcc
CFLAGS = -O3 -Wall 
CFLAGS += -D_LARGEFILE_SOURCE
CFLAGS += -fno-exceptions
CFLAGS += -funroll-loops
CFLAGS += -D_FILE_OFFSET_BITS=64

# Source files
NEW_MERGER = record.c new_merge.c
MERGE_EXTERNAL = record.c merge_external.c
DISK_SORT = merge_external.c record.c disk_sort.c
QUERY1 = record.c new_merge.c merge_external.c query1.c
# Binaries
all: disk_sort merge_external query1

#sequential writing in blocks
disk_sort: ${DISK_SORT}
	$(CC) $(CFLAGS) $^ -o disk_sort
merge_external: ${DISK_SORT}
	$(CC) $(CFLAGS) $^ -o merge_external
query1: ${QUERY1}
	$(CC) $(CFLAGS) $^ -o query1

clean:  
	rm disk_sort merge_external query1

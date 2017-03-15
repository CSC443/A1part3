CC = gcc
CFLAGS = -O3 -Wall 
CFLAGS += -D_LARGEFILE_SOURCE
CFLAGS += -fno-exceptions
CFLAGS += -funroll-loops
CFLAGS += -D_FILE_OFFSET_BITS=64

# Source files
NEW_MERGER = record.c new_merge.c
QUERY1 = disk_sort.c record.c new_merge.c merge_external.c query1.c
READ_BLOCKS_SRC= record.c read_blocks_seq.c

# Binaries
all:query1 read_blocks_seq

#sequential writing in blocks
query1: ${QUERY1}
	$(CC) $(CFLAGS) $^ -o query1
read_blocks_seq:${READ_BLOCKS_SRC}
	$(CC) $(CFLAGS) $^ -o read_blocks_seq

clean:  
	rm disk_sort merge_external query1 read_blocks_seq

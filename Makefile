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
QUERY2 = write_degree_block.c disk_sort.c disk_sort_q2.c q2_merge_external.c record.c new_merge.c merge_external.c query2.c

# Binaries
all:query1 read_blocks_seq query2

#sequential writing in blocks
query1: ${QUERY1}
	$(CC) $(CFLAGS) $^ -o query1
read_blocks_seq:${READ_BLOCKS_SRC}
	$(CC) $(CFLAGS) $^ -o read_blocks_seq
query2: ${QUERY2}
	$(CC) $(CFLAGS) $^ -o query2

clean:  
	rm query1 read_blocks_seq query2

default: build
	

build:
	g++ -Wall main.cpp log.cpp fs.cpp encoding.cpp `pkg-config fuse --cflags --libs` -o main.o -D_FILE_OFFSET_BITS=64

mount:
	mkdir -p mount_dir 
	mkdir -p src_dir
	rm -f bbfs.log
	./main.o src_dir mount_dir ./enc_tables/postgres_encoding_table.txt

unmount:
	fusermount -u mount_dir

test: unmount build mount
	
default:
	gcc -Wall main.c log.c `pkg-config fuse --cflags --libs` -o main.o -D_FILE_OFFSET_BITS=64

build:
	gcc -Wall main.c log.c `pkg-config fuse --cflags --libs` -o main.o -D_FILE_OFFSET_BITS=64

mount:
	mkdir -p mount_dir 
	mkdir -p src_dir
	./main.o src_dir mount_dir

unmount:
	fusermount -u mount_dir

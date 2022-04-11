default:
	gcc -Wall bbfs.c log.c `pkg-config fuse --cflags --libs` -o main.o -D_FILE_OFFSET_BITS=64

build:
	gcc -Wall bbfs.c log.c `pkg-config fuse --cflags --libs` -o main.o -D_FILE_OFFSET_BITS=64

mount:
	mkdir -p tmp_mp
	./main.o -d -s -f ./tmp_mp

unmount:
	fusermount -u tmp_mp

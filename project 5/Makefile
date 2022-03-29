default:
	gcc -Wall main.c `pkg-config fuse3 --cflags --libs` -o main.o -D_FILE_OFFSET_BITS=64

build:
	gcc -Wall main.c `pkg-config fuse3 --cflags --libs` -o main.o -D_FILE_OFFSET_BITS=64

mount:
	mkdir -p tmp_mp
	./main.o -d -s -f ./tmp_mp

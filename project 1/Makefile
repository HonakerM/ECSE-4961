MATRIX_TYPE?=float

default:
	g++  -g *.cpp -o main.o -msse4.1 -mavx -D CACHE_OPTIMIZATION -D MATRIX_TEST_TYPE=$(MATRIX_TYPE)

build:
	g++  -g *.cpp -o main.o -msse4.1 -mavx -D MATRIX_TEST_TYPE=$(MATRIX_TYPE)

build_with_cache_optimization:
	g++  -g *.cpp -o main.o -msse4.1 -mavx -D CACHE_OPTIMIZATION -D MATRIX_TEST_TYPE=$(MATRIX_TYPE)

build_with_compiler_optimization:
	g++  -g *.cpp -o main.o -msse4.1 -mavx -D CACHE_OPTIMIZATION -Ofast -D MATRIX_TEST_TYPE=$(MATRIX_TYPE)


clean:
	@rm -f ./main.o

run:
	./main.o
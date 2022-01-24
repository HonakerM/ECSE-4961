default:
	g++ -Wall -g *.cpp -o main.o -msse4.1 -mavx -D CACHE_OPTIMIZATION

build:
	g++ -Wall -g *.cpp -o main.o -msse4.1 -mavx 

build_with_cache_optimization:
	g++ -Wall -g *.cpp -o main.o -msse4.1 -mavx -D CACHE_OPTIMIZATION

clean:
	@rm -f ./main.o

run:
	./main.o
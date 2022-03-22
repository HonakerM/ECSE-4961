#require bash shell
SHELL := /bin/bash

default:
	mkdir -p files
	g++ -std=c++11 -O3 *.cpp -o ./main.o  -pthread 

debug:
	mkdir -p files
	g++ -std=c++11 -g *.cpp -o ./main.o  -pthread -D debug

create_data:
	mkdir -p files
	python3 generate_column_data.py files/Raw-Column-Data.txt 200000 23574717 5 15

test_encode:
	echo "testing encoding for up to 16 threads"
	num1=1 ; while [[ $$num1 -le 16 ]] ; do \
		echo "Testing $$num1 threads"; \
		num2=1 ; while [[ $$num2 -le 5 ]] ; do \
			./main.o -e -f files/Raw-Column-Data.txt -o /dev/null -t $$num1 ; \
			((num2 = num2 + 1)) ; \
		done ; \
		((num1 = num1 + 1)) ; \
	done


test_decode:
	echo "encoding test file"
	./main.o -e -f files/Raw-Column-Data.txt -o files/encoded_data -tm


	echo "testing decoding for up to 16 threads"
	num1=1 ; while [[ $$num1 -le 16 ]] ; do \
		echo "Testing $$num1 threads"; \
		num2=1 ; while [[ $$num2 -le 5 ]] ; do \
			./main.o -d -f files/encoded_data -o /dev/null -t $$num1 ; \
			((num2 = num2 + 1)) ; \
		done ; \
		((num1 = num1 + 1)) ; \
	done
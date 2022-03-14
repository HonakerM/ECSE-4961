default:
	g++ -std=c++11 -g *.cpp -o ./main.o  -pthread

test_encode:
	g++ -std=c++11 -g *.cpp -o ./main.o  -pthread

	for i in {1..16}
	do
		echo "thread count $i" 
		for i in {1..5}
		do
			echo "run $i" 
			echo "Small"
			./main.o -e -f files/
		done
	done
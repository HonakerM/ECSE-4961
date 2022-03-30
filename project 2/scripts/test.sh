#!/bin/bash

cd ../

make time

for i in {1..15}
do
    echo "worker $i" 
    echo "worker $i" >> scripts/results.txt
    for v in {1..5}
    do
        ./main.o $i ./scripts/test.data ./scripts/test.zstd >> scripts/results.txt
    done
done

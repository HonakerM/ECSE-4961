#!/bin/bash

make time

for i in {1..50}
do
    ./main.o $i
done
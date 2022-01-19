The first project of ECSE-4961 was to multiply two matricies together using SIMD instructions

## Execution
### PreRequisites
This project was created using `g++ v10.2.1-6` with the optional dependency of `GNU Make 4.3`. The [Old Dominion University](https://www.cs.odu.edu/~zeil/cs250PreTest/latest/Public/installingACompiler/) has some simple documentation on how to install `g++` depending on your operating system.

### Build
This project can be built using the `Makefile` by calling `make`. Or by directly using `g++` by calling the following function:

```
g++ -Wall -g *.cpp -o main.o -msse4.1
```

### Running
Once compiled the default tests can be executed by running `./main.o`
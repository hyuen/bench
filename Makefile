GFLAGS = -std=c++11 -O3 # -march=native -funroll-loops
all:
	clang++ $(GFLAGS) -o bench bench.cc

as:
	clang++ $(GFLAGS) -S -o bench.S bench.cc
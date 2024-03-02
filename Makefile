GFLAGS = -std=c++11 -O3 # -march=native -funroll-loops
CLANGPP=/opt/homebrew/opt/llvm/bin/clang++
all:
	$(CLANGPP) $(GFLAGS) -o bench bench.cc -fopenmp

as:
	$(CLANGPP) $(GFLAGS) -S -o bench.S bench.cc
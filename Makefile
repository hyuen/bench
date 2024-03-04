GFLAGS = -g -std=c++11 -O3 -fopenmp # -Rpass=vector
CLANGPP=/opt/homebrew/opt/llvm/bin/clang++
# CLANGPP=clang++
all: matmul_amx matmul_amx_test
	$(CLANGPP) $(GFLAGS) -o bench bench.cc -fopenmp

matmul_amx:
	$(CLANGPP) $(GFLAGS) -c matmul_amx.cc

matmul_amx_test: matmul_amx.o
	$(CLANGPP) $(GFLAGS) matmul_amx.o matmul_amx_test.cc -o matmul_amx_test 

as:
	$(CLANGPP) $(GFLAGS) -S -o bench.S bench.cc


#include "matmul_amx.h"
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

static uint64_t fpcr_init() {
    uint64_t old_fpcr;
    __asm volatile ("mrs %0, fpcr" : "=r"(old_fpcr));
    uint64_t new_fpcr = old_fpcr | (1ull << 25); // DN (Default NaN)
    __asm volatile ("msr fpcr, %0" : : "r"(new_fpcr));
    return old_fpcr;
}

static void fpcr_restore(uint64_t fpcr) {
    __asm volatile ("msr fpcr, %0" : : "r"(fpcr));
}
#define PTR_ROW_FLAGS(ptr, row, flags) (((uint64_t)&*(ptr)) + (((uint64_t)((row) + (flags) * 64)) << 56))



void print_matrix(float* A, int m, int n) {
    cout << "Matrix of " << m << "x" << n << endl;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            cout << A[i * n + j] << " ";
        cout << endl;
    }
}


int main(void) {
    uint32_t AMX_VER = detect_amx_hardware_version();
    cout << "AMX version: " << AMX_VER << endl; 

    int n = 32;
    int times = 1;

    float *A = new float[n*n];
    float *B = new float[n*n];
    float *C = new float[n*n];

    for(auto i=0; i< n * n; i++) {
        A[i] = 1.0;
        B[i] = 1.0;
        C[i] = 0.0;
    }

    cout << "init finised\n";
     print_matrix(A, 32, 32);
 
    steady_clock::time_point start = steady_clock::now();
    for (int i = 0; i < times; i++)
    {
        AMX_SET();
        mm32x32xK(A, B, C, 1);
        AMX_CLR();
        float total = 0.0;
        for (int i = 0; i < n * n; i++)
        {
            total += C[i];
        }
        // cout << total << endl;
    }
    steady_clock::time_point end = steady_clock::now();
    auto dt = duration_cast<microseconds>(end - start);
    print_matrix(C, n, n); //32, 32);
    cout << dt.count() / times / 1000  << " ms" << endl;

    
 
    return 0;
}
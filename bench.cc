#include <iostream>
#include <chrono>
#include <time.h>

using namespace std;
using namespace std::chrono;

// c = a * b
// a is m x k
// b is k x n
// c is m x n
void matmul1(float *c, float *a, float *b, int m, int k, int n)
{
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            for (int l = 0; l < k; l++)
                c[i * n + j] += a[i * k + l] * b[l * n + j];
}

void matmul2(float *c, float *a, float *b, int m, int k, int n, int bs)
{
    for (int i = 0; i < m; i++)
        for (int l = 0; l < k; l++)
            for (int j = 0; j < n; j++)
                c[i * n + j] += a[i * k + l] * b[l * n + j];
}

void matmul21(float* __restrict c, float* __restrict a, float* __restrict b, int m, int k, int n, int bs)
{
    #pragma omp parallel for
    for (int i = 0; i < m; i++)
        for (int l = 0; l < k; l++)
            for (int j = 0; j < n; j++)
                c[i * n + j] += a[i * k + l] * b[l * n + j];
}

void matmul3(float *c, float *a, float *b, int m, int k, int n)
{
    for (int j = 0; j < n; j++)
        for (int i = 0; i < m; i++)
            for (int l = 0; l < k; l++)
                c[i * n + j] += a[i * k + l] * b[l * n + j];
}

void matmul4(float *c, float *a, float *b, int m, int k, int n)
{
    for (int j = 0; j < n; j++)
        for (int l = 0; l < k; l++)
            for (int i = 0; i < m; i++)
                c[i * n + j] += a[i * k + l] * b[l * n + j];
}

void matmul5(float *c, float *a, float *b, int m, int k, int n, int bs)
{
    for (int l = 0; l < k; l++)
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                c[i * n + j] += a[i * k + l] * b[l * n + j];
}

void matmul6(float *c, float *a, float *b, int m, int k, int n)
{
    for (int l = 0; l < k; l++)
        for (int j = 0; j < n; j++)
            for (int i = 0; i < m; i++)
                c[i * n + j] += a[i * k + l] * b[l * n + j];
}

void matmul7(float *c, float *a, float *b, int m, int k, int n, int bs)
{
    const int block_size = bs;
    #pragma omp parallel for
    for (int ii =0; ii <m; ii+= block_size)
        for (int jj =0; jj <n; jj+= block_size)
            for (int ll =0; ll <k; ll+= block_size)
                for (int i = ii; i < min(ii + block_size, m); i++)
                    for (int l = ll; l < min(ll + block_size, k); l++)
                        for (int j = jj; j < min(jj + block_size, n); j++)
                            c[i * n + j] += a[i * k + l] * b[l * n + j];
}

void bench(int m, int n, int k, int times, const string &method_name, int bs, void (*matmul)(float *, float *, float *, int, int, int, int))
{
    auto a = new float[m * k];
    auto b = new float[k * n];
    auto c_out = new float[m * n];

    steady_clock::time_point start = steady_clock::now();
    for (int i = 0; i < times; i++)
    {
        matmul(c_out, a, b, m, k, n, bs);
    }
    steady_clock::time_point end = steady_clock::now();

    // force a read
    float total = 0.0;
    for (int i = 0; i < m * n; i++)
    {
        total += c_out[i];
    }

    auto dt = duration_cast<microseconds>(end - start);
    cout << method_name << " " << dt.count() / times / 1000  << " ms" << endl;

    delete[] a;
    delete[] b;
    delete[] c_out;
}

int main(void)
{
    int m = 1024; // 4096; // * 10000;
    int n = m;
    int k = m;

    auto a = new float[m * k];
    auto b = new float[k * n];
    auto c_out = new float[m * n];
    int times = 10;

    // bench(m, n, k, times, "matmul1", matmul1);
    bench(m, n, k, times, "matmul2", 0, matmul2);
    bench(m, n, k, times, "matmul21", 0, matmul21);
    // bench(m, n, k, times, "matmul3", matmul3);
    // bench(m, n, k, times, "matmul4", matmul4);
    bench(m, n, k, times, "matmul5", 0, matmul5);
    // bench(m, n, k, times, "matmul6", matmul6);
    for (int bs=4; bs<=1024; bs*=2){
        cout << "bs=" << bs << endl;
        bench(m, n, k, times, "matmul7", bs, matmul7);
    }

    return 0;
}

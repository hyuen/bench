import numpy as np
import time

ntimes = 10
n = 1024 * 16
x = np.random.randn(n, n).astype(np.float32)
y = np.random.randn(n, n).astype(np.float32)

start = time.time_ns()
for i in range(ntimes):
    z = np.matmul(x, y)
    z = np.reshape(z, -1)
    z2 = np.sum(z)
    print(z2)

delta = (time.time_ns() - start)/ntimes/1e6

print(delta, "ms", ntimes)
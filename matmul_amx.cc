#include "matmul_amx.h"
#include <iostream>

// Logic for copying between hardware AMX state and emulated AMX state

#define PTR_ROW_FLAGS(ptr, row, flags) (((uint64_t)&*(ptr)) + (((uint64_t)((row) + (flags) * 64)) << 56))

uint32_t detect_amx_hardware_version() {
    __attribute__((aligned(256))) uint8_t buf[256];
    buf[64] = 2;
    buf[128] = 1;
    AMX_SET(); // Set x[0:8] to zero
    AMX_LDX(PTR_ROW_FLAGS(buf, 48, 1)); // On M1: copy buf[0:128] to x[0,1], on M2: copy buf[0:256] to x[0,1,2,3], on M3: copy buf[0:256] to x[0,2,4,6]
    AMX_STX(PTR_ROW_FLAGS(buf,  2, 0)); // Copy x[2] to buf[0:64]
    AMX_CLR();
    return 1 + buf[0];
}

void mm32x32xK(float* A, float* B, float* C, uint64_t K) {
  // flag to load/store 128 bytes
  const uint64_t load_store_2 = 1ull << 62;
  const uint64_t load_store_width = 128; // in bytes

  // AMX_SET();
  // only set for k == 0
  uint64_t reset_z = 1ull << 27;

  for (uint32_t k = 0; k < K; ++k) {
    // std::cout <<  k<< std::endl;
    uint64_t idx = k % 4;
    // load to X, Y (skipping every other index because we're loading 128 bytes)
    AMX_LDX(load_store_2 | (idx * 2) << 56 | (uint64_t)A + k * load_store_width);
    AMX_LDY(load_store_2 | (idx * 2) << 56 | (uint64_t)B + k * load_store_width);

    // // offset into X and Y registers is byte-wise
    const uint64_t offset = idx * load_store_width;

    // // now we do 4 indepedent outer products (avoiding pipeline hazards)
    AMX_FMA32(reset_z | (0ull << 20) | ((offset +  0ull) << 10) | ((offset +  0ull) << 0));
    AMX_FMA32(reset_z | (1ull << 20) | ((offset + 64ull) << 10) | ((offset +  0ull) << 0));
    AMX_FMA32(reset_z | (2ull << 20) | ((offset +  0ull) << 10) | ((offset + 64ull) << 0));
    AMX_FMA32(reset_z | (3ull << 20) | ((offset + 64ull) << 10) | ((offset + 64ull) << 0));
    reset_z = 0;
  }

  for (uint64_t i = 0; i < 16; ++i) {
    // store interleaved
    AMX_STZ(load_store_2 | ((i * 4ull + 0) << 56) | (uint64_t)C + i * load_store_width);
    AMX_STZ(load_store_2 | ((i * 4ull + 2) << 56) | (uint64_t)C + (16 + i) * load_store_width);
  }
}
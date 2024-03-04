#pragma once
#include "aarch64.h"

void mm32x32xK(float* A, float* B, float* C, uint64_t K);
uint32_t detect_amx_hardware_version();

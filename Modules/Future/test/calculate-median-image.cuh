/*
 * Medical Image Registration ToolKit (MIRTK)
 *
 * Copyright 2016 Imperial College London
 * Copyright 2016 Andreas Schuh
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <climits>
#include <cfloat>
#include <cstdio>

#include "cuda_runtime_api.h"


namespace mirtk { namespace cuda {


// =============================================================================
// Types
// =============================================================================

// -----------------------------------------------------------------------------
/// Sorting algorithms implemented for use with CUDA median filter
enum SortingAlgorithm
{
  SA_InsertionSort,
  SA_SelectionSort,
  SA_QuickSelect_RandPivot,
  SA_QuickSelect_MedianOf3,
  SA_MedianOfMedians
};

// =============================================================================
// Auxiliaries
// =============================================================================

// -----------------------------------------------------------------------------
#define CudaSafeCall(ans) ::mirtk::cuda::checkCall((ans), __FILE__, __LINE__, true)
#define CudaWarnCall(ans) ::mirtk::cuda::checkCall((ans), __FILE__, __LINE__, false)
inline void checkCall(cudaError_t code, const char *file, int line, bool abort)
{
  if (code != cudaSuccess) { 
    fprintf(stderr, "CUDA Error: %s %s %d\n", cudaGetErrorString(code), file, line);
    if (abort) exit(code);
  }
}

// -----------------------------------------------------------------------------
template <class T> T DivUp(T a, T b)
{
  return (a + b - 1) / b;
}

// =============================================================================
// Type traits
// =============================================================================

template <class T>
struct type_traits
{
  __host__ __device__ static T max() throw();
};

template <> struct type_traits<int>
{
  __host__ __device__ static int max() throw() { return INT_MAX; }
};

template <> struct type_traits<float>
{
  __host__ __device__ static float max() throw() { return FLT_MAX; }
};

template <> struct type_traits<double>
{
  __host__ __device__ static double max() throw() { return DBL_MAX; }
};

// =============================================================================
// Declaration of host functions
// =============================================================================

// -----------------------------------------------------------------------------
template <class TVoxel, int wx, int wy, int wz>
void RunMedianFilter(TVoxel *d_out, const TVoxel *d_in, const int *d_offsets,
                      int nx, int ny, int nz, SortingAlgorithm algo = SA_QuickSelect_RandPivot);

// -----------------------------------------------------------------------------
template <class TVoxel>
void RunThrustMedianFilter(TVoxel *d_out, const TVoxel *d_in,
                           int nx, int ny, int nz,
                           int wx, int wy, int wz);


} } // namespace mirtk::cuda

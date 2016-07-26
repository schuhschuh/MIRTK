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

#include "calculate-median-image.cuh"

#include "thrust/device_ptr.h"
#include "thrust/device_malloc.h"
#include "thrust/device_free.h"
#include "thrust/device_vector.h"
#include "thrust/sequence.h"
#include "thrust/for_each.h"
#include "thrust/execution_policy.h"

#include "mirtk/Profiling.h"

namespace mirtk { namespace cuda {


// =============================================================================
// 3D median filter
// =============================================================================

// -----------------------------------------------------------------------------
/// Swap values using the provided temporary buffer
template <class T>
__device__ __forceinline__ void swap(T &a, T &b, T &tmp)
{
  tmp = a, a = b, b = tmp;
}

// -----------------------------------------------------------------------------
/// Selection sort items in place
template <class T>
__device__ void SelectionSort(T *items, int begin, int end)
{
  T tmp;
  int i, j, k;
  for (i = begin; i < end; ++i) {
    k = i;
    for (j = k+1; j < end; ++j) {
      if (items[j] < items[k]) {
        k = j;
      }
    }
    if (k != i) {
      swap(items[i], items[k], tmp);
    }
  }
}

// -----------------------------------------------------------------------------
/// Insertion sort items in place
template <class T>
__device__ void InsertionSort(T *items, int begin, int end)
{
  T x;
  int i, j;
  for (i = begin + 1; i < end; ++i) {
    x = items[i];
    j = i - 1;
    while (j >= 0 && items[j] > x) {
      items[j+1] = items[j];
      --j;
    }
    items[j+1] = x;
  }
}

// -----------------------------------------------------------------------------
/// Rearranges the items in [begin, end) such that items below the
/// returned index have value less than the pivot and items above have
/// value greater or equal the pivot
template <class T>
__device__ int Partition(T *items, int begin, int end, int pivot)
{
  T         tmp;
  const T   value = items[pivot];
  const int right = end - 1;
  int       j     = begin;
  swap(items[pivot], items[right], tmp);
  for (int i = begin; i < right; ++i) {
    if (items[i] < value) {
      swap(items[i], items[j], tmp);
      ++j;
    }
  }
  swap(items[right], items[j], tmp);
  return j;
}

// Forward declaration of mutual-recursive median-of-medians function
template <class T> __device__ int MedianOfMedians(T *, int, int, int);

// -----------------------------------------------------------------------------
/// Median of three pivot selection strategy
template <class T>
__device__ int MedianOf3(T *items, int begin, int end)
{
  const int last   = end - 1;
  const int middle = (last - begin) / 2;
  const T a = items[begin];
  const T b = items[middle];
  const T c = items[last];
  if (a < b) {
    if (a < c) return begin;
    else       return last;
  } else if (a < c) {
    if (a < b) return begin;
    else       return middle;
  } else /* a >= b && a >= c */ {
    if (b < c) return middle;
    else       return last;
  }
}

// -----------------------------------------------------------------------------
/// Pivot function of median-of-medians algorithm
template <class T>
__device__ int MedianOf5(T *items, int begin, int end)
{
  int n = end - begin;
  if (n <= 5) {
    InsertionSort(items, begin, end);
    return n / 2;
  }
  T tmp;
  for (int i = begin, j, k = begin; i < end; i += 5, ++k) {
    j = min(i + 5, end);
    InsertionSort(items, i, j);
    j = (j - i) / 2;
    swap(items[j], items[k], tmp);
  }
  n   = begin + (end - begin - 1) / 10;
  end = (end - begin + 3) /  5;
  return MedianOfMedians(items, begin, end, n);
}

// -----------------------------------------------------------------------------
/// Median-of-medians algorithm
template <class T>
__device__ int MedianOfMedians(T *items, int begin, int end, int n)
{
  int pivot;
  while (end - begin > 1) {
    pivot = MedianOf5(items, begin, end);
    pivot = Partition(items, begin, end, pivot);
    if      (n == pivot) return pivot;
    else if (n  < pivot) end   = pivot;
    else                 begin = pivot + 1;
  }
  return begin;
}

// -----------------------------------------------------------------------------
/// Iterative quickselect algorithm with median of three pivot strategy
template <class T>
__device__ int QuickSelect(T *items, int begin, int end, int n)
{
  int pivot;
  while (end - begin > 1) {
    pivot = MedianOf3(items, begin, end);
    pivot = Partition(items, begin, end, pivot);
    if      (n == pivot) return pivot;
    else if (n  < pivot) end   = pivot;
    else                 begin = pivot + 1;
  }
  return begin;
}

// -----------------------------------------------------------------------------
/// Iterative quickselect algorithm with random pivot selection
template <class T>
__device__ int QuickSelect(T *items, int begin, int end, int n, const int *rand_val, int rand_num)
{
  int pivot;
  int rand_idx = 0;
  int len = end - begin;
  while (len > 1) {
    pivot = begin + rand_val[rand_idx] % len;
    pivot = Partition(items, begin, end, pivot);
    if      (n == pivot) return pivot;
    else if (n  < pivot) end   = pivot;
    else                 begin = pivot + 1;
    len = end - begin;
    if (++rand_idx == rand_num) rand_idx = 0;
  }
  return begin;
}

// -----------------------------------------------------------------------------
__device__ __forceinline__ void ind2sub(int id, int &i, int &j, int &k, int nx, int nxy)
{
  k  = id / nxy;
  id = id % nxy;
  j  = id / nx;
  i  = id % nx;
}

// -----------------------------------------------------------------------------
__device__ __forceinline__ int sub2ind(int i, int j, int k, int nx, int nxy)
{
  return i + j * nx + k * nxy;
}

// -----------------------------------------------------------------------------
/// Kernel of ND median filter with neighborhood offsets and one thread per window
template <class T, int tx, int ty, int tz, int wx, int wy, int wz, SortingAlgorithm algo>
__global__ void MedianKernel(T* output, const T *input, const int *offsets,
                             int nx, int ny, int nz, int z0,
                             const int *d_rand = nullptr, int nrand = 0)
{
  enum {
    rx     = wx / 2,
    ry     = wy / 2,
    rz     = wz / 2,
    wn     = wx * wy * wz,
    sx     = tx,// + wx - 1,
    sy     = ty,// + wy - 1,
    sz     = tz,// + wz - 1,
    sxy    = sx * sy,
    sn     = sx * sy * sz,       // no. of subvolume elements incl. halo
    tn     = tx * ty * tz,       // no. of worker threads in block
    stride = (sn + tn - 1) / tn, // stride between local values to load
  };

  const int nxy = nx * ny;
  const int nv  = nxy * nz;

  const int bid = blockIdx .x + gridDim .x * (blockIdx .y + gridDim .y * (blockIdx.z + z0));
  const int tid = threadIdx.x + blockDim.x * (threadIdx.y + blockDim.y * threadIdx.z);
  const int cid = bid * blockDim.x * blockDim.y * blockDim.z + tid;

  if (cid < nv) {
    // Collectively load subvolume with coalescing into shared memory.
    // As the windows of the threads overlap, this should increase bandwidth.
    #if 0
      const int i0 = blockIdx.x;
      const int j0 = blockIdx.y * gridDim.x;
      const int k0 = blockIdx.z * gridDim.x * gridDim.y;

      int vid, vi, vj, vk; // indices into loaded subvolume in shared memory
      int gid, gi, gj, gk; // indices into input volume in global memory
      int wid;             // linear index into local window values array

      __shared__ T volume[sn];
      for (vid = tid; vid < sn; vid += stride) {
        ind2sub(vid, vi, vj, vk, sx, sxy);
        gi = i0 + vi;// - rx;
        gj = j0 + vj;// - ry;
        gk = k0 + vk;// - rz;
        gid = sub2ind(gi, gj, gk, nx, nxy);
        //if (gid < 0 || gid >= nv) volume[vid] = T(0);
        //else                      volume[vid] = input[gid];
      }
      __syncthreads();

      T window[wn];
      for (wid = 0; wid < wn; ++wid) {
        gid = cid + offsets[wid];
        if (gid < 0 || gid >= nv) {
          window[wid] = T(0);
        } else {
          ind2sub(gid, gi, gj, gk, nx, nxy);
          vi = gi - i0;// + rx;
          vj = gj - j0;// + ry;
          vk = gk - k0;// + rz;
          vid = sub2ind(vi, vj, vk, sx, sxy);
          window[wid] = volume[vid];
        }
      }
    #else
      T window[wn];
      for (int wid = 0, gid; wid < wn; ++wid) {
        gid = cid + offsets[wid];
        if (gid < 0 || gid >= nv) window[wid] = T(0);
        else                      window[wid] = input[gid];
      }
    #endif

    switch (algo) {
      case SA_InsertionSort: {
        InsertionSort(window, 0, wn);
        output[cid] = window[wn/2];
      } break;
      case SA_SelectionSort: {
        SelectionSort(window, 0, wn);
        output[cid] = window[wn/2];
      } break;
      case SA_QuickSelect_RandPivot: {
        output[cid] = window[QuickSelect(window, 0, wn, wn/2, d_rand, nrand)];
      } break;
      case SA_QuickSelect_MedianOf3: {
        output[cid] = window[QuickSelect(window, 0, wn, wn/2)];
      } break;
      case SA_MedianOfMedians: {
        output[cid] = window[MedianOfMedians(window, 0, wn, wn/2)];
      } break;
    }
  }
}

// -----------------------------------------------------------------------------
/// Launch median filter kernel for a given total no. of elements in each window
template <class TVoxel, int wx, int wy, int wz>
__host__ void RunMedianFilter(TVoxel *d_out, const TVoxel *d_in, const int *d_offsets,
                              int nx, int ny, int nz, SortingAlgorithm algo)
{
  // launch parameters
  const int nslices  = 8;
  const int nbatches = (nz + nslices - 1) / nslices;
  const int tx = 16, ty = 16, tz = 1;
  dim3 blockDim = dim3(tx, ty, tz);
  dim3 gridDim  = dim3(DivUp<unsigned int>(nx, blockDim.x),
                       DivUp<unsigned int>(ny, blockDim.y),
                       DivUp<unsigned int>(nz, blockDim.z));
  // generate pool of random numbers for quickselect
  int n_rand = 0, *h_rand = nullptr, *d_rand = nullptr;
  if (algo == SA_QuickSelect_RandPivot) {
    n_rand = wx * wy * wz;
    h_rand = new int[n_rand];
    for (int i = 0; i < n_rand; ++i) h_rand[i] = rand();
    CudaSafeCall(cudaMalloc(&d_rand, n_rand * sizeof(int)));
    CudaSafeCall(cudaMemcpy(d_rand, h_rand, n_rand * sizeof(int), cudaMemcpyHostToDevice));
  }
  // run kernel in batches of slices to avoid kernel timeout
  MIRTKCU_START_TIMING();
  for (int batch = 0, z0 = 0; batch < nbatches; ++batch, z0 += nslices) {
    gridDim.z = min(nslices, nz - z0);
    switch (algo) {
      case SA_SelectionSort: {
        MedianKernel<TVoxel, tx, ty, tz, wx, wy, wz, SA_SelectionSort><<<gridDim, blockDim>>>(d_out, d_in, d_offsets, nx, ny, nz, z0);
      } break;
      case SA_InsertionSort: {
        MedianKernel<TVoxel, tx, ty, tz, wx, wy, wz, SA_InsertionSort><<<gridDim, blockDim>>>(d_out, d_in, d_offsets, nx, ny, nz, z0);
      } break;
      case SA_QuickSelect_RandPivot: {
        MedianKernel<TVoxel, tx, ty, tz, wx, wy, wz, SA_QuickSelect_RandPivot><<<gridDim, blockDim>>>(d_out, d_in, d_offsets, nx, ny, nz, z0, d_rand, n_rand);
      } break;
      case SA_QuickSelect_MedianOf3: {
        MedianKernel<TVoxel, tx, ty, tz, wx, wy, wz, SA_QuickSelect_MedianOf3><<<gridDim, blockDim>>>(d_out, d_in, d_offsets, nx, ny, nz, z0);
      } break;
      case SA_MedianOfMedians: {
        MedianKernel<TVoxel, tx, ty, tz, wx, wy, wz, SA_MedianOfMedians><<<gridDim, blockDim>>>(d_out, d_in, d_offsets, nx, ny, nz, z0);
      } break;
    }
    CudaSafeCall(cudaPeekAtLastError());
  }
  MIRTKCU_END_TIMING("median filter kernel");
  if (d_rand) CudaWarnCall(cudaFree(d_rand));
  if (h_rand) delete[] h_rand;
}

#define INSTANTIATE_FOR_VOXEL_TYPE(type) \
  template __host__ void RunMedianFilter<type,  3, 3,  1>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type,  5, 5,  1>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type,  7, 7,  1>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type, 11, 11, 1>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type, 13, 13, 1>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type, 15, 15, 1>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type, 17, 17, 1>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type, 19, 19, 1>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type, 21, 21, 1>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type, 23, 23, 1>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type, 25, 25, 1>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  \
  template __host__ void RunMedianFilter<type,  3,  3,  3>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type,  5,  5,  5>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type,  7,  7,  7>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type, 11, 11, 11>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type, 13, 13, 13>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type, 15, 15, 15>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type, 17, 17, 17>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type, 19, 19, 19>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type, 21, 21, 21>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type, 23, 23, 23>(type *, const type *, const int *, int, int, int, SortingAlgorithm); \
  template __host__ void RunMedianFilter<type, 25, 25, 25>(type *, const type *, const int *, int, int, int, SortingAlgorithm)
INSTANTIATE_FOR_VOXEL_TYPE(short);
INSTANTIATE_FOR_VOXEL_TYPE(unsigned short);
INSTANTIATE_FOR_VOXEL_TYPE(int);
INSTANTIATE_FOR_VOXEL_TYPE(unsigned int);
INSTANTIATE_FOR_VOXEL_TYPE(float);
INSTANTIATE_FOR_VOXEL_TYPE(double);
#undef INSTANTIATE_FOR_VOXEL_TYPE

// =============================================================================
// Median image filter using Thrust
// =============================================================================

// -----------------------------------------------------------------------------
template <class T>
struct ThrustMedianKernel
{
  const T   *_Input;
  T         *_Output;
  const int *_Offsets;
  T         *_Buffer;
  int        _SliceOffset;
  int        _NumVoxels;
  int        _NumOffsets;

  __device__ T operator ()(int i) const
  {
    T *values = _Buffer + i * _NumOffsets;
    for (int j = 0, idx; j < _NumOffsets; ++j) {
      idx = _SliceOffset + i + _Offsets[j];
      values[j] = ((idx < 0 || idx >= _NumVoxels) ? T(0) : _Input[idx]);
    }
    thrust::sort(thrust::device, values, values + _NumOffsets);
    return values[_NumOffsets/2];
  }
};

// -----------------------------------------------------------------------------
template <class T>
__host__ void RunThrustMedianFilter(T *d_out, const T *d_in,
                                    int nx, int ny, int nz,
                                    int wx, int wy, int wz)
{
  const int nslices = 1;

  const int rx = wx / 2;
  const int ry = wy / 2;
  const int rz = wz / 2;
  const int wn = wx * wy * wz;

  // calculate neighborhood offsets
  int idx = 0;
  thrust::device_vector<int> offsets(wx * wy * wz);
  for (int k = -rz; k <= rz; ++k)
  for (int j = -ry; j <= ry; ++j)
  for (int i = -rx; i <= rx; ++i, ++idx) {
    offsets[idx] = i + nx * (j + ny * k);
  }

  // median filter with buffer for unwrapped slice
  thrust::device_vector<T> buffer(nslices * nx * ny * wn);

  ThrustMedianKernel<T> kernel;
  kernel._Input      = d_in;
  kernel._Output     = d_out;
  kernel._Offsets    = thrust::raw_pointer_cast(offsets.data());
  kernel._Buffer     = thrust::raw_pointer_cast(buffer .data());
  kernel._NumVoxels  = nx * ny * nz;
  kernel._NumOffsets = wn;

  // start timer
  clock_t t_start = clock();

  // slice-by-slice due to high memory demands otherwise
  thrust::device_vector<int> seq(nslices * nx * ny);
  thrust::sequence(thrust::device, seq.begin(), seq.end(), 0);
  for (int k = 0; k < nz; k += nslices) {
    kernel._SliceOffset = k * nx * ny;
    if (k + nslices >= nz) {
      seq.resize(nslices - (k + nslices - nz + 1));
    }
    thrust::for_each(seq.begin(), seq.end(), kernel);
  }

  // wait for kernels to finish before measuring the runtime
  CudaSafeCall(cudaDeviceSynchronize());
  const double t_elapsed = static_cast<double>(clock() - t_start) / CLOCKS_PER_SEC;
  PrintElapsedTime("median filter kernel [Thrust]", t_elapsed, TIME_IN_MILLISECONDS);
}

#define INSTANTIATE_FOR_VOXEL_TYPE(type) \
  template __host__ void RunThrustMedianFilter(type *, const type *, int, int, int, int, int, int)
INSTANTIATE_FOR_VOXEL_TYPE(short);
INSTANTIATE_FOR_VOXEL_TYPE(unsigned short);
INSTANTIATE_FOR_VOXEL_TYPE(int);
INSTANTIATE_FOR_VOXEL_TYPE(unsigned int);
INSTANTIATE_FOR_VOXEL_TYPE(float);
INSTANTIATE_FOR_VOXEL_TYPE(double);
#undef INSTANTIATE_FOR_VOXEL_TYPE


} } // namespace mirt::cuda

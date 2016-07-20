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

#include "gtest/gtest.h"

#include "mirtk/future/ArrayFire.h"
#include "af/internal.h"

#include "mirtk/Parallel.h"
#include "mirtk/Profiling.h"

#include <numeric>


using namespace mirtk;
using namespace mirtk::future;


// =============================================================================
// Auxiliaries
// =============================================================================

// -----------------------------------------------------------------------------
af::array clamp(const af::array &in, double min, double max)
{
    return ((in < min) * min + (in > max) * max + (in >= min && in <= max) * in);
}

// -----------------------------------------------------------------------------
template <class T>
struct MedianFilterKernel
{
  const T   *_Input;
  T         *_Output;
  const int *_Offsets;
  int        _NumberOfVoxels;
  int        _NumberOfOffsets;

  MedianFilterKernel()  = default;
  ~MedianFilterKernel() = default;

  // Get median of values within window centered at i-th voxel
  MIRTKCU_API T median(int i, std::vector<T> &values) const
  {
    for (int j = 0, idx; j < _NumberOfOffsets; ++j) {
      idx = i + _Offsets[j];
      values[j] = ((idx < 0 || idx >= _NumberOfVoxels) ? 0. : _Input[idx]);
    }
    std::partial_sort(values.begin(), values.begin() + (_NumberOfOffsets+1)/2, values.end());
    return values[_NumberOfOffsets/2];
  }

  // Unary operator for use by thrust::for_each or custom CUDA kernel
  MIRTKCU_API T operator ()(int i) const
  {
    std::vector<T> values(_NumberOfOffsets);
    return median(i, values);
  }

  // Unary operator for TBB parallel_for
  void operator ()(const blocked_range<int> &range) const
  {
    std::vector<T> values(_NumberOfOffsets);
    for (int i = range.begin(); i != range.end(); ++i) {
      _Output[i] = median(i, values);
    }
  }
};

// =============================================================================
// 3D median filter
// =============================================================================

// -----------------------------------------------------------------------------
TEST(ArrayFire, NonLinearFilterWithTBB)
{
  const int nx = 256, ny = 256, nz = 256;
  const int rx =   3, ry =   3, rz =   3;
  const int nv = nx * ny * nz;
  const int nw = (2*rx+1) * (2*ry+1) * (2*rz+1);

  Array<int> input (nv);
  Array<int> output(input.size());
  std::iota(input.begin(), input.end(), 1);

  MIRTK_START_TIMING();

  int offsets[nw];
  int *offset = offsets;
  for (int k = -rz; k <= rz; ++k)
  for (int j = -ry; j <= ry; ++j)
  for (int i = -rx; i <= rx; ++i, ++offset) {
    *offset = i + nx * (j + k * ny);
  }

  MedianFilterKernel<int> kernel;
  kernel._Input           = input.data();
  kernel._Output          = output.data();
  kernel._NumberOfVoxels  = static_cast<int>(input.size());
  kernel._NumberOfOffsets = nw;
  kernel._Offsets         = offsets;
  parallel_for(blocked_range<int>(0, kernel._NumberOfVoxels), kernel);

  MIRTK_END_TIMING("median filtering");
}

// -----------------------------------------------------------------------------
TEST(ArrayFire, NonLinearFilterWithGfor)
{
  using namespace af;
  // create image of typical brain MR image size, where
  // voxel values correspond to their linear one-based index
  const dim_t nx = 256; // no. of voxels in x dimension
  const dim_t ny = 256; // no. of voxels in y dimension
  const dim_t nz = 256; // no. of voxels in z dimension
  const dim_t rx =   3; // window radius in x dimension
  const dim_t ry =   3; // window radius in y dimension
  const dim_t rz =   3; // window radius in z dimension
  const dim_t nw = (2*rx+1) * (2*ry+1) * (2*rz+1); // no. of voxels in window
  array image = range(dim4(nx*ny*nz), 0, s32) + 1;
  image = moddims(image, nx, ny, nz);
  // BEGIN of non-linear filter execution
  timer start = timer::start();
  {
    // pad image with zeros
    array padded_image = constant(0, nx+2*rx, ny+2*ry, nz+2*rz, image.type());
    padded_image(seq(rx, nx+rx-1), seq(ry, ny+ry-1), seq(rz, nz+rz-1)) = image;
    // compute linear index offsets of local window into padded image
    const dim4 strides = getStrides(padded_image);
    dim_t h_offsets[nw];
    dim_t *offset = h_offsets;
    for (dim_t k = -rz; k <= rz; ++k)
    for (dim_t j = -ry; j <= ry; ++j)
    for (dim_t i = -rx; i <= rx; ++i, ++offset) {
      *offset = i * strides[0] + j * strides[1] + k * strides[2];
    }
    array offsets(nw, h_offsets);
    // flatten the image because we want a single loop over all voxels and
    // therefore use linear indices to reference the voxels within a window
    array flat_image = flat(padded_image);
    // for each voxel, extract grey values within local window
    // and compute non-linear filter response for this voxel
    //
    // note: the following is similar to af::unwarp,
    //       which however only supports 2D images/windows
    //
    // have to split the processing into smaller batches
    // due to device memory restrictions; unfortunately af::getDeviceMemorySize
    // is not part of the public API to split accordingly
    const size_t dbytes    = .8 * 1073414144;         // 80% out of 1GB available (GeForce GT 650M)
    const size_t rbytes    = nw * flat_image.bytes(); // estimated memory requirement for full unwrap
    const dim_t  nbatches  = (rbytes + dbytes - 1) / dbytes;
    const dim_t  batchsize = (flat_image.elements() + nbatches - 1) / nbatches;
    for (dim_t batch = 0; batch < nbatches; ++batch) {
      dim_t begin = batch * batchsize;
      dim_t end   = begin + batchsize - 1;
      if (end >= flat_image.elements()) {
        end = flat_image.elements() - 1;
      }
      gfor(seq i, begin, end) {
        // compute linear indices of voxels within i-th local window
        array indices = i + offsets;
        // when there are any out-of-bounds indices, the center voxel i is within
        // the padded boundary zone and we won't use the result anyway; however,
        // branching is not possible in gfor loop; hence, just compute something
        // using the clamped indices and discard those results afterwards
        indices = clamp(indices, 0, flat_image.elements()-1);
        // get vector of image values within local window
        array values = flat_image(indices);
        // FIXME: the following reshaping of the values array is necessary
        //        because indices array has dims [nw 1 1 nv], but the array
        //        returnd by flat_image(indices) has dims [nw*nv 1 1 1]!
        //        This would not be the case if this were a regular for loop.
        //        It is a side effect of how the gfor loop works. My guess,
        //        flat_image(indices) is doing the wrong thing here. It is
        //        supposed to give me a result array with the same shape as
        //        the indices argument array.
        values = moddims(values, nw, 1, 1, values.elements() / nw /* == i.size */);
        // reduction of local window values
        #if 0 // custom median implementation
          // FIXME: minor issue/comment
          //
          //        a custom implementation of extracting the median would be
          //        to call sort on the vector and then extract the middle
          //        element. this is not possible in gfor loop without adjusting
          //        the array indexing call. all four arguments are required
          //        instead of just one vector index. thus, I must know that
          //        I am dealing with a matrix rather than a vector here and
          //        that the return value should not a scalar but a vector.
          //
          //        in other words, the following
          //          flat_image(i) = values(values.dims(0)/2);
          //        does not produce the same result as
          //          flat_image(i) = values(values.dims(0)/2, span, span, i);
          //        but in a regular for loop I would be writing the first line.
          values = sort(values, 0);
          flat_image(i) = values(values.dims(0)/2, span, span, span /* == i.size */);
        #else
          flat_image(i) = median(values, 0);
        #endif
      }
    }
    // get result of non-linear filter at non-padded voxels
    padded_image = af::moddims(flat_image, nx+2*rx, ny+2*ry, nz+2*rz);
    image = padded_image(seq(rx, nx+rx-1), seq(ry, ny+ry-1), seq(rz, nz+rz-1));
  }
  // END of non-linear filter execution
  printf("elapsed seconds: %g\n", timer::stop(start));
}

// =============================================================================
// Main
// =============================================================================

// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  af::info();
  return RUN_ALL_TESTS();
}

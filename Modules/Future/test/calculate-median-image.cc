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

#include "mirtk/Options.h"
#include "mirtk/GenericImage.h"
#include "mirtk/future/Image.h"
#include "mirtk/Algorithm.h"
#include "mirtk/Parallel.h"
#include "mirtk/Profiling.h"

#include <numeric>

#include "mirtk/FutureConfig.h"
#if MIRTK_Future_WITH_ArrayFire
  #include "mirtk/future/ArrayFire.h"
  using namespace mirtk::future::arrayfire;
#endif
#ifdef HAVE_CUDA
  #include "calculate-median-image.cuh"
#endif

using namespace mirtk;
using namespace mirtk::future;


// =============================================================================
// Help
// =============================================================================

// -----------------------------------------------------------------------------
void PrintHelp(const char *name)
{
  cout << "\n";
  cout << "Usage: " << name << " <input> <output> [options]\n";
  cout << "\n";
  cout << "Options:\n";
  cout << "  -window-size <wx> [<wy> [<wz>]]\n";
  cout << "      Size of local box window in number of voxels. (default: 3)\n";
  cout << "  -platform CPU|CUDA|Thrust|OpenCL\n";
  cout << "      Computing platform/framework. (default: CPU)\n";
  cout << "  -arrayfire\n";
  cout << "      Use ArrayFire implementation for specified platform.\n";
  PrintCommonOptions(cout);
}

// =============================================================================
// CPU (TBB)
// =============================================================================
namespace mirtk { namespace cpu {

// -----------------------------------------------------------------------------
template <class T>
class MedianFilterKernel
{
  const T   *_Input;
  T         *_Output;
  const int *_Offsets;
  int        _NumVoxels;
  int        _NumOffsets;

public:

  MedianFilterKernel(Image &output, const Image &input, const Array<int> &offsets)
  :
    _Input(input.Pointer<T>()),
    _Output(output.Pointer<T>()),
    _Offsets(offsets.data()),
    _NumVoxels(input.X() * input.Y() * input.Z()),
    _NumOffsets(static_cast<int>(offsets.size()))
  {}

  void operator ()(const blocked_range<int> &range) const
  {
    const int median = _NumOffsets/2;
    Array<T> values(_NumOffsets);
    for (int i = range.begin(), idx; i != range.end(); ++i) {
      for (int j = 0; j < _NumOffsets; ++j) {
        idx = i + _Offsets[j];
        values[j] = ((idx < 0 || idx >= _NumVoxels) ? 0. : _Input[idx]);
      }
      NthElement(values, median);
      _Output[i] = values[median];
    }
  }

  void Run()
  {
    MIRTK_START_TIMING();
    parallel_for(blocked_range<int>(0, _NumVoxels), *this);
    MIRTK_DEBUG_TIMING(2, "median filter kernel [CPU]");
  }
};

// -----------------------------------------------------------------------------
Image MedianFilter(const Image &image, int wx, int wy, int wz)
{
  const int rx = wx / 2;
  const int ry = wy / 2;
  const int rz = wz / 2;

  Array<int> offsets(wx * wy * wz);
  int *offset = offsets.data();
  for (int k = -rz; k <= rz; ++k)
  for (int j = -ry; j <= ry; ++j)
  for (int i = -rx; i <= rx; ++i, ++offset) {
    *offset = image.Index(i, j, k);
  }

  Image input(image, Platform_CPU);
  Image output(input.Grid(), input.Type(), input.Platform(), input.Device());
  switch (input.Type()) {
    case T_Short:  MedianFilterKernel<Short >(output, input, offsets).Run(); break;
    case T_UShort: MedianFilterKernel<UShort>(output, input, offsets).Run(); break;
    case T_Int:    MedianFilterKernel<Int   >(output, input, offsets).Run(); break;
    case T_UInt:   MedianFilterKernel<UInt  >(output, input, offsets).Run(); break;
    case T_Float:  MedianFilterKernel<Float >(output, input, offsets).Run(); break;
    case T_Double: MedianFilterKernel<Double>(output, input, offsets).Run(); break;
    default: {
      cerr << __FUNCTION__ << ": Unsupported image type: " << ToString(input.Type()) << endl;
      exit(1);
    }
  }
  return output;
}


} } // namespace mirtk::cpu
// =============================================================================
// CUDA
// =============================================================================
#ifdef HAVE_CUDA
namespace mirtk { namespace cuda {


// -----------------------------------------------------------------------------
template <class TVoxel>
void RunMedianFilter(TVoxel *d_out, const TVoxel *d_in, const int *d_offsets,
                     int nx, int ny, int nz, int wx, int wy, int wz, SortingAlgorithm algo)
{
  #define RunIfWindowSizeIs(WX, WY, WZ) \
    if (wx == WX && wy == WY && wz == WZ) { \
      RunMedianFilter<TVoxel, WX, WY, WZ>(d_out, d_in, d_offsets, nx, ny, nz, algo); \
      return; \
    }
  // 2D window
  RunIfWindowSizeIs(3,3,1);
  RunIfWindowSizeIs(5,5,1);
  RunIfWindowSizeIs(7,7,1);
  RunIfWindowSizeIs(11,11,1);
  RunIfWindowSizeIs(13,13,1);
  RunIfWindowSizeIs(15,15,1);
  RunIfWindowSizeIs(17,17,1);
  RunIfWindowSizeIs(19,19,1);
  RunIfWindowSizeIs(21,21,1);
  RunIfWindowSizeIs(23,23,1);
  RunIfWindowSizeIs(25,25,1);
  // 3D window
  RunIfWindowSizeIs(3,3,3);
  RunIfWindowSizeIs(5,5,5);
  RunIfWindowSizeIs(7,7,7);
  RunIfWindowSizeIs(11,11,11);
  RunIfWindowSizeIs(13,13,13);
  RunIfWindowSizeIs(15,15,15);
  RunIfWindowSizeIs(17,17,17);
  RunIfWindowSizeIs(19,19,19);
  RunIfWindowSizeIs(21,21,21);
  RunIfWindowSizeIs(23,23,23);
  RunIfWindowSizeIs(25,25,25);
  #undef RunIfWindowSizeIs
  FatalError(__FUNCTION__ << ": no instantiation available for window size "
             << wx << "x" << wy << "x" << wz);
  exit(1);
}

// -----------------------------------------------------------------------------
Image MedianFilter(const Image &image, int wx, int wy, int wz,
                   SortingAlgorithm algo = SA_QuickSelect_RandPivot)
{
  // copy input to CUDA device if not done yet, allocate output image
  const Image input(image, Platform_CUDA);
  Image output(input.Grid(), input.Type(), input.Platform(), input.Device());

  // calculate neighborhood offsets and copy to device
  int *d_offsets;
  {
    const int rx = wx / 2;
    const int ry = wy / 2;
    const int rz = wz / 2;
    Array<int> h_offsets(wx * wy * wz);
    int *offset = h_offsets.data();
    for (int k = -rz; k <= rz; ++k)
    for (int j = -ry; j <= ry; ++j)
    for (int i = -rx; i <= rx; ++i, ++offset) {
      *offset = input.Index(i, j, k);
    }
    const size_t s_offsets = h_offsets.size() * sizeof(int);
    CudaSafeCall(cudaMalloc(&d_offsets, s_offsets));
    CudaSafeCall(cudaMemcpy(d_offsets, h_offsets.data(), s_offsets, cudaMemcpyHostToDevice));
  }

  // launch kernel for a given window size
  switch (input.Type()) {
    #define CaseTypeIs(type) \
      case T_##type: { \
        RunMedianFilter<type>(output.Pointer<type>(), input.Pointer<type>(), d_offsets, \
                              image.X(), image.Y(), image.Z(), wx, wy, wz, algo); \
      } break
    CaseTypeIs(Short);
    CaseTypeIs(UShort);
    CaseTypeIs(Int);
    CaseTypeIs(UInt);
    CaseTypeIs(Float);
    CaseTypeIs(Double);
    #undef CaseTypeIs
    default:
      FatalError(__FUNCTION__ << ": no instantiation available for data type: " << ToString(input.Type()));
  }

  // free device memory
  CudaWarnCall(cudaFree(d_offsets));

  CudaSafeCall(cudaDeviceSynchronize());
  return output;
}


} } // namespace mirtk::cuda
#endif // HAVE_CUDA
// =============================================================================
// Thrust
// =============================================================================
#ifdef HAVE_CUDA
namespace thrust  {


// -----------------------------------------------------------------------------
Image MedianFilter(const Image &image, int wx, int wy, int wz)
{
  const Image input(image, Platform_CUDA);
  Image output(input.Grid(), input.Type(), input.Platform(), input.Device());

  switch (image.Type()) {
    #define CaseTypeIs(type) \
      case T_##type: { \
        using namespace mirtk::cuda; \
        RunThrustMedianFilter<type>(output.Pointer<type>(), \
                                    input .Pointer<type>(), \
                                    input.X(), input.Y(), input.Z(), \
                                    wx, wy, wz); \
      } break
    CaseTypeIs(Short);
    CaseTypeIs(UShort);
    CaseTypeIs(Int);
    CaseTypeIs(UInt);
    CaseTypeIs(Float);
    CaseTypeIs(Double);
    #undef CaseTypeIs
    default:
      cerr << __FUNCTION__ << ": Unsupported data type: "
           << ToString(image.Type()) << endl;
      exit(1);
  }

  return output;
}


} // namespace thrust
#endif // HAVE_CUDA
// =============================================================================
// ArrayFire
// =============================================================================
#if MIRTK_Future_WITH_ArrayFire
namespace arrayfire {


// -----------------------------------------------------------------------------
Image MedianFilter(const Image &image, int wx, int wy, int wz)
{
  using namespace af;

  BackendSwitch with(image.Platform());
  Image output(image);
  array arr = ToValuesArray(output);

  if (wz == 1 && wx % 2 == 1 && wx <= 15 && wy % 2 == 1 && wy <= 15) {
    arr = medfilt(arr, wx, wy);
  } else {
    // image size
    const dim_t nx = static_cast<dim_t>(image.X());
    const dim_t ny = static_cast<dim_t>(image.Y());
    const dim_t nz = static_cast<dim_t>(image.Z());
    // window radius
    const int rx = wx / 2;
    const int ry = wy / 2;
    const int rz = wz / 2;
    // size of padded image
    const dim_t mx = nx + wx - 1;
    const dim_t my = ny + wy - 1;
    const dim_t mz = nz + wz - 1;
    // index ranges for original image
    const dim_t x1 = rx, x2 = x1 + nx - 1;
    const dim_t y1 = ry, y2 = y1 + ny - 1;
    const dim_t z1 = rz, z2 = z1 + nz - 1;
    // pad image with zeros
    array input = constant(0, mx, my, mz, arr.type());
    input(seq(x1, x2), seq(y1, y2), seq(z1, z2)) = arr;
    // apply filter for each slab in z of depth wz
    for (dim_t z = z1; z <= z2; ++z) {
      // extract window values and rearrange s.t. dims=[nx*ny wx*wy*wz 1 1]
      array values = unwrap(input(span, span, seq(z-rz, z+rz)), wx, wy, 1, 1);
      values = reorder(values, 1, 0, 2);
      values = moddims(values, values.dims(0), values.dims(1) * values.dims(2));
      // compute non-linear filter response for given window values
      values = sort(values, 1);
      values = values(span, values.dims(1)/2);
      // reshape result for current slice and write to output
      arr(span, span, z-z1) = moddims(values, nx, ny);
    }
  }

  WriteValues(output, arr);
  af::sync();
  return output;
}


} // namespace arrayfire
#endif // MIRTK_Future_WITH_ArrayFire
// =============================================================================
// Main
// =============================================================================

// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  // Parse command arguments
  EXPECTS_POSARGS(2);

  const char *input_name  = POSARG(1);
  const char *output_name = POSARG(2);
  PlatformId  platform    = Platform_CPU;
  bool        arrayfire   = false;
  bool        use_thrust  = false;

  #ifdef HAVE_CUDA
    using namespace mirtk::cuda;
    SortingAlgorithm algo = SA_InsertionSort;
  #endif

  int wx = 3, wy = 3, wz = 3;
  for (ALL_OPTIONS) {
    if (OPTION("-window-size")) {
      PARSE_ARGUMENT(wx);
      if (HAS_ARGUMENT) {
        PARSE_ARGUMENT(wy);
        if (HAS_ARGUMENT) {
          PARSE_ARGUMENT(wz);
        } else wz = 1;
      } else wy = wz = wx;
    }
    else if (OPTION("-platform")) {
      string arg = ToLower(ARGUMENT);
      if (arg == "thrust") {
        #ifdef HAVE_CUDA
          platform   = Platform_CUDA;
          use_thrust = true;
        #else
          FatalError("Future library was built without CUDA!");
        #endif
      } else {
        if (!FromString(arg, platform)) {
          FatalError("Invalid -platform argument: " << arg);
        }
        use_thrust = false;
      }
    }
    else if (OPTION("-arrayfire")) {
      #if MIRTK_Future_WITH_ArrayFire
        arrayfire = true;
      #else
        FatalError("Future library was built without ArrayFire!");
      #endif
    }
    else if (OPTION("-insertion-sort")) {
      #ifdef HAVE_CUDA
        algo = SA_InsertionSort;
      #else
        FatalError("Test build without CUDA");
      #endif
    }
    else if (OPTION("-selection-sort")) {
      #ifdef HAVE_CUDA
        algo = SA_SelectionSort;
      #else
        FatalError("Test build without CUDA");
      #endif
    }
    else if (OPTION("-quick-select") || OPTION("-quick-select-rand")) {
      #ifdef HAVE_CUDA
        algo = SA_QuickSelect_RandPivot;
      #else
        FatalError("Test build without CUDA");
      #endif
    }
    else if (OPTION("-quick-select-median3")) {
      #ifdef HAVE_CUDA
        algo = SA_QuickSelect_MedianOf3;
      #else
        FatalError("Test build without CUDA");
      #endif
    }
    else if (OPTION("-median-of-medians")) {
      #ifdef HAVE_CUDA
        algo = SA_MedianOfMedians;
      #else
        FatalError("Test build without CUDA");
      #endif
    }
    else HANDLE_COMMON_OR_UNKNOWN_OPTION();
  }

  // Read image
  Image image(input_name);

  if (wx > 1 || wy > 1 || wz > 1) {
    // Copy to device
    if (platform != Platform_Default && image.Platform() != platform) {
      MIRTK_START_TIMING();
      image = Image(image, platform);
      MIRTK_DEBUG_TIMING(1, "copy to device");
    }

    // Run median filter
    MIRTK_START_TIMING();
    if (arrayfire) {
      #if MIRTK_Future_WITH_ArrayFire
        image = ::arrayfire::MedianFilter(image, wx, wy, wz);
      #else
        FatalError("Future library was built without ArrayFire!");
      #endif
    } else {
      switch (platform) {
        case Platform_Default:
        case Platform_CPU: {
          image = cpu::MedianFilter(image, wx, wy, wz);
        } break;
        case Platform_CUDA: {
          if (use_thrust) {
            image = thrust::MedianFilter(image, wx, wy, wz);
          } else {
            image = cuda::MedianFilter(image, wx, wy, wz, algo);
          }
        } break;
        case Platform_OpenCL: {
          FatalError("No native OpenCL implementation available, use -arrayfire");
        } break;
      }
    }
    MIRTK_DEBUG_TIMING(1, "median filter");

    // Copy to host
    if (image.Platform() != Platform_CPU) {
      MIRTK_START_TIMING();
      image = Image(image, Platform_CPU);
      MIRTK_DEBUG_TIMING(1, "copy to host");
    }
  }

  // Write image
  image.Write(output_name);
}

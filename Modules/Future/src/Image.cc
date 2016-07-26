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

#include "mirtk/future/Image.h"

#include "mirtk/ImageReader.h"
#include "mirtk/ImageWriter.h"
#include "mirtk/BaseImage.h"
#include "mirtk/GenericImage.h"


namespace mirtk { namespace future {

// =============================================================================
// Construction/Destruction
// =============================================================================

// -----------------------------------------------------------------------------
void Image::CopyAttributes(const Image &other)
{
  _Grid = other._Grid;
}

// -----------------------------------------------------------------------------
void Image::SwapAttributes(Image &other)
{
  swap(_Grid, other._Grid);
}

// -----------------------------------------------------------------------------
void Image::Swap(Image &other)
{
  DataArray::Swap(other);
  SwapAttributes(other);
}

// -----------------------------------------------------------------------------
Image::Image(const RegularGrid &grid, DataType type, PlatformId platform, DeviceId device)
:
  Image(grid, 1, type, platform, device)
{
}

// -----------------------------------------------------------------------------
Image::Image(const RegularGrid &grid, int m, DataType type, PlatformId platform, DeviceId device)
:
  DataArray(grid.Points(), m, type, platform, device), _Grid(grid)
{
}

// -----------------------------------------------------------------------------
Image::Image(const Image &other, PlatformId platform, DeviceId device)
:
  DataArray(other, platform, device), _Grid(other._Grid)
{
}

// -----------------------------------------------------------------------------
Image::Image(const Image &other, PlatformId platform, DeviceId device, bool copy_values, bool copy_status)
:
  DataArray(other, platform, device, copy_values, copy_status)
{
  CopyAttributes(other);
}

// -----------------------------------------------------------------------------
Image::Image(Image &&other)
:
  DataArray(other)
{
  SwapAttributes(other);
}

// =============================================================================
// Assignment operator
// =============================================================================

// -----------------------------------------------------------------------------
Image &Image::operator =(const Image &other)
{
  if (this != &other) {
    DataArray::operator =(other);
    CopyAttributes(other);
  }
  return *this;
}

// -----------------------------------------------------------------------------
Image &Image::operator =(Image &&other)
{
  DataArray::operator =(other);
  SwapAttributes(other);
  return *this;
}

// -----------------------------------------------------------------------------
Image &Image::operator =(double value)
{
  DataArray::operator =(value);
  return *this;
}

// =============================================================================
// Deep copy
// =============================================================================

// -----------------------------------------------------------------------------
SharedPtr<DataArray> Image
::Copy(PlatformId platform, DeviceId device, bool copy_values, bool copy_status)
{
  return SharedPtr<DataArray>(new Image(*this, platform, device, copy_values, copy_status));
}

// =============================================================================
// Data object interface
// =============================================================================

// -----------------------------------------------------------------------------
DataKind Image::Kind() const
{
  return D_Image;
}

// -----------------------------------------------------------------------------
size_t Image::Bytes() const
{
  return DataArray::Bytes() + _Grid.Bytes();
}

// =============================================================================
// I/O
// =============================================================================

// -----------------------------------------------------------------------------
void Image::Read(const char *fname)
{
  // Read image
  UniquePtr<ImageReader> reader(ImageReader::New(fname));
  UniquePtr<BaseImage>   image(reader->Run());
  // Convert to new image type
  *this = Image(*image);
  // Apply rescaling function
  if (reader->Slope() != .0 && reader->Slope() != 1.0) {
    *this *= reader->Slope();
  }
  if (reader->Intercept() != .0) {
    *this += reader->Intercept();
  }
}

// -----------------------------------------------------------------------------
bool Image::Write(const char *fname) const
{
  Image image(*this, Platform_CPU);
  image.AsBaseImage()->Write(fname);
  return true;
}

// -----------------------------------------------------------------------------
void Image::Print(Indent indent) const
{
  cout << indent << "Image grid:\n";
  _Grid.Print(indent + 1);
  cout << "\n";
}

// =============================================================================
// Deprecated
// =============================================================================

// -----------------------------------------------------------------------------
template <class TVoxel>
static void CopyData(TVoxel *ptr, const BaseImage &other)
{
  const int n = other.NumberOfVoxels();
  for (int i = 0; i < n; ++i, ++ptr) {
    *ptr = type_cast<TVoxel>(other.GetAsDouble(i));
  }
}

// -----------------------------------------------------------------------------
template <class TVoxel>
static void CopyData(TVoxel *ptr, const mirtk::GenericImage<TVoxel> &other)
{
  const int n = other.NumberOfVoxels();
  for (int i = 0; i < n; ++i, ++ptr) {
    *ptr = other.Get(i);
  }
}

// -----------------------------------------------------------------------------
Image::Image(const BaseImage &other)
:
  DataArray(other.TSize() > 0. ? other.NumberOfVoxels() : other.NumberOfSpatialVoxels(),
            other.TSize() > 0. ? 1                      : other.T(),
            DataType(other.GetDataType()), Platform_CPU),
  _Grid(other.Attributes())
{
  switch (this->Type()) {
    #define case_(type) \
      case T_##type: { CopyData(this->Pointer<type>(), other); } break
    case_(Binary);
    case_(Char);
    case_(UChar);
    case_(Short);
    case_(UShort);
    case_(Int);
    case_(UInt);
    case_(Float);
    case_(Double);
    #undef case_
    default:
      cerr << __FUNCTION__ << ": Unsupported type: " << ToString(this->Type()) << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
template <class TVoxel>
Image::Image(const mirtk::GenericImage<TVoxel> &other)
:
  DataArray(other.TSize() > 0. ? other.NumberOfVoxels() : other.NumberOfSpatialVoxels(),
            other.TSize() > 0. ? 1                      : other.T(),
            type_traits<TVoxel>::type(), Platform_CPU),
  _Grid(other.Attributes())
{
  CopyData(this->Pointer<TVoxel>(), other);
}

template <> Image::Image(const mirtk::GenericImage<Binary> &);
template <> Image::Image(const mirtk::GenericImage<Grey> &);
template <> Image::Image(const mirtk::GenericImage<Float> &);
template <> Image::Image(const mirtk::GenericImage<Double> &);

// -----------------------------------------------------------------------------
SharedPtr<BaseImage> Image::AsBaseImage()
{
  switch (this->Type()) {
    #define case_(type) \
      case T_##type: return NewShared<mirtk::GenericImage<type> >(_Grid.AsImageAttributes(), this->Pointer<type>())
    case_(Binary);
    case_(Char);
    case_(UChar);
    case_(Short);
    case_(UShort);
    case_(Int);
    case_(UInt);
    case_(Float);
    case_(Double);
    #undef case_
    default:
      cerr << __FUNCTION__ << ": Unsupported type: " << ToString(this->Type()) << endl;
      exit(1);
  }
}

// -----------------------------------------------------------------------------
SharedPtr<const BaseImage> Image::AsBaseImage() const
{
  return const_cast<Image *>(this)->AsBaseImage();
}


} } // namespace mirtk::future

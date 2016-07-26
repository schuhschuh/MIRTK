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

#ifndef MIRTK_Future_Image_H
#define MIRTK_Future_Image_H

#include "mirtk/future/DataArray.h"
#include "mirtk/future/RegularGrid.h"


// Forward declaration of deprecated image classes
namespace mirtk {
class BaseImage;
template <class T> class GenericImage;
} // namespace mirtk


namespace mirtk { namespace future {


/**
 * Multi-dimension array positioned in world space
 */
class Image : public DataArray
{
  mirtkObjectMacro(Image);

  // ---------------------------------------------------------------------------
  // Attributes

  /// Image attributes
  mirtkReadOnlyAttributeMacro(RegularGrid, Grid);

  /// Copy attributes of this class from another instance
  void CopyAttributes(const Image &other);

  /// Swap attributes of this class with another instance
  void SwapAttributes(Image &);

protected:

  /// Swap attributes of this class and superclasses with another instance
  void Swap(Image &);

  /// Copy constructor
  ///
  /// \param[in] other       Instance.
  /// \param[in] platform    Target platform.
  /// \param[in] device      Target device of \p platform.
  /// \param[in] copy_values Force copy of tuple values.
  /// \param[in] copy_status Force copy of tuple status.
  Image(const Image &other, PlatformId platform, DeviceId device,
                            bool copy_values, bool copy_status);

public:

  // ---------------------------------------------------------------------------
  // Construction/Destruction

  /// Default constructor
  Image() = default;

  /// Construct from file
  ///
  /// \param[in] fname Image file name.
  Image(const char *fname);

  /// Construct from file
  ///
  /// \param[in] fname Image file name.
  Image(const string &fname);

  /// Construct new image
  ///
  /// \param[in] grid     Discrete image grid.
  /// \param[in] type     Type of image values.
  /// \param[in] platform Platform on which to create image.
  /// \param[in] device   Platform device on which to allocate image.
  Image(const RegularGrid &grid, DataType   type     = T_Real,
                                 PlatformId platform = Platform_Default,
                                 DeviceId   device   = -1);

  /// Construct new image
  ///
  /// \param[in] grid     Discrete image grid.
  /// \param[in] m        Number of components per voxel.
  /// \param[in] type     Type of image values.
  /// \param[in] platform Platform on which to create image.
  /// \param[in] device   Platform device on which to allocate image.
  Image(const RegularGrid &grid, int m, DataType   type     = T_Real,
                                        PlatformId platform = Platform_Default,
                                        DeviceId   device   = -1);

  /// Shallow copy constructor
  ///
  /// \param[in] other    Other image.
  /// \param[in] platform Target platform. Use platform of \p other object if Platform_Default.
  /// \param[in] device   Target device. Use device of \p other object or
  ///                     active device of \p platform when negative.
  Image(const Image &other, PlatformId platform = Platform_Default,
                            DeviceId   device   = -1);

  /// Move constructor
  Image(Image &&);

  /// Destructor
  virtual ~Image() = default;

  // ---------------------------------------------------------------------------
  // Assignment operator

  /// Shallow copy assignment operator
  Image &operator =(const Image &);

  /// Move assignment operator
  Image &operator =(Image &&);

  /// Assign constant scalar value
  Image &operator =(double);

  // ---------------------------------------------------------------------------
  // Deep copy

  /// Make a copy of this data object
  ///
  /// \param[in] platform    Target platform. Use platform of this object if Platform_Default.
  /// \param[in] device      Target device. Use device of this object or
  ///                        active device of \p platform when negative.
  /// \param[in] copy_values Request copy of the values memory.
  /// \param[in] copy_status Request copy of the status memory.
  virtual SharedPtr<DataArray> Copy(PlatformId platform, DeviceId  device,
                                    bool copy_values, bool copy_status);

  // ---------------------------------------------------------------------------
  // Data object interface

  /// Kind of data object
  DataKind Kind() const;

  /// Number of bytes this data object occupies in memory
  size_t Bytes() const;

  // ---------------------------------------------------------------------------
  // Image grid

  // Image size
  int X() const; ///< Number of voxels in x dimension
  int Y() const; ///< Number of voxels in y dimension
  int Z() const; ///< Number of voxels in z dimension
  int T() const; ///< Number of voxels in t dimension
  int N() const; ///< Number of components per voxel

  // Voxel size
  double dX() const; ///< Size of voxels in x dimension
  double dY() const; ///< Size of voxels in y dimension
  double dZ() const; ///< Size of voxels in z dimension
  double dT() const; ///< Size of voxels in t dimension

  /// Convert voxel subscripts to linear voxel index
  Id Index(int, int) const;

  /// Convert voxel subscripts to linear voxel index
  Id Index(int, int, int) const;

  /// Convert voxel subscripts to linear voxel index
  Id Index(int, int, int, int) const;

  /// Convert linear voxel index to 2D voxel subscripts
  void Subscripts(Id, int &, int &) const;
 
  /// Convert linear voxel index to 3D voxel subscripts
  void Subscripts(Id, int &, int &, int &) const;
 
  /// Convert linear voxel index to 4D voxel subscripts
  void Subscripts(Id, int &, int &, int &, int &) const;
  
  // ---------------------------------------------------------------------------
  // Scalar operations

  /// Add constant value to all elements
  Image &operator +=(double);

  /// Subtract constant value from all elements
  Image &operator -=(double);

  /// Multiply all elements with a constant value
  Image &operator *=(double);

  /// Divide all elements by a constant value
  Image &operator /=(double);


  /// Add constant value to all elements
  Image operator +(double) const;

  /// Subtract constant value from all elements
  Image operator -(double) const;

  /// Multiply all elements with a constant value
  Image operator *(double) const;

  /// Divide all elements by a constant value
  Image operator /(double) const;

  // ---------------------------------------------------------------------------
  // Element-wise operations

  /// Add constant value to all elements
  Image &operator +=(const Image &);

  /// Subtract constant value from all elements
  Image &operator -=(const Image &);

  /// Multiply all elements with a constant value
  Image &operator *=(const Image &);

  /// Divide all elements by a constant value
  Image &operator /=(const Image &);


  /// Add constant value to all elements
  Image operator +(const Image &) const;

  /// Subtract constant value from all elements
  Image operator -(const Image &) const;

  /// Multiply all elements with a constant value
  Image operator *(const Image &) const;

  /// Divide all elements by a constant value
  Image operator /(const Image &) const;

  // ---------------------------------------------------------------------------
  // I/O

  /// Read image from file
  virtual void Read(const char *);

  /// Write image to file
  virtual bool Write(const char *) const;

  /// Print image information
  virtual void Print(Indent = 0) const;

  // ---------------------------------------------------------------------------
  // Deprecated

  /// Copy construct from deprecated image
  Image(const BaseImage &);

  /// Copy construct from deprecated image
  template <class T>
  Image(const GenericImage<T> &);

  /// Move construct from deprecated image
  template <class T>
  Image(GenericImage<T> &&);

  /// Wrap this image in deprecated image type
  SharedPtr<BaseImage> AsBaseImage();

  /// Wrap this image in deprecated image type
  SharedPtr<const BaseImage> AsBaseImage() const;
};

////////////////////////////////////////////////////////////////////////////////
// Inline definitions
////////////////////////////////////////////////////////////////////////////////

// =============================================================================
// Construction
// =============================================================================

// -----------------------------------------------------------------------------
inline Image::Image(const char *fname)
{
  Read(fname);
}

// -----------------------------------------------------------------------------
inline Image::Image(const string &fname)
:
  Image(fname.c_str())
{
}

// =============================================================================
// Image grid
// =============================================================================

// -----------------------------------------------------------------------------
inline int Image::X() const
{
  return _Grid.nx;
}

// -----------------------------------------------------------------------------
inline int Image::Y() const
{
  return _Grid.ny;
}

// -----------------------------------------------------------------------------
inline int Image::Z() const
{
  return _Grid.nz;
}

// -----------------------------------------------------------------------------
inline int Image::T() const
{
  return _Grid.nt;
}

// -----------------------------------------------------------------------------
inline int Image::N() const
{
  return _Components;
}

// -----------------------------------------------------------------------------
inline double Image::dX() const
{
  return _Grid.dx;
}

// -----------------------------------------------------------------------------
inline double Image::dY() const
{
  return _Grid.dy;
}

// -----------------------------------------------------------------------------
inline double Image::dZ() const
{
  return _Grid.dz;
}

// -----------------------------------------------------------------------------
inline double Image::dT() const
{
  return _Grid.dt;
}

// -----------------------------------------------------------------------------
inline Id Image::Index(int i, int j) const
{
  return _Grid.Index(i, j);
}

// -----------------------------------------------------------------------------
inline Id Image::Index(int i, int j, int k) const
{
  return _Grid.Index(i, j, k);
}

// -----------------------------------------------------------------------------
inline Id Image::Index(int i, int j, int k, int l) const
{
  return _Grid.Index(i, j, k, l);
}

// -----------------------------------------------------------------------------
inline void Image::Subscripts(Id idx, int &i, int &j) const
{
  _Grid.Subscripts(idx, i, j);
}

// -----------------------------------------------------------------------------
inline void Image::Subscripts(Id idx, int &i, int &j, int &k) const
{
  _Grid.Subscripts(idx, i, j, k);
}

// -----------------------------------------------------------------------------
inline void Image::Subscripts(Id idx, int &i, int &j, int &k, int &l) const
{
  _Grid.Subscripts(idx, i, j, k, l);
}

// =============================================================================
// Scalar operations
// =============================================================================

// -----------------------------------------------------------------------------
inline Image &Image::operator +=(double value)
{
  DataArray::operator +=(value);
  return *this;
}

// -----------------------------------------------------------------------------
inline Image &Image::operator -=(double value)
{
  DataArray::operator -=(value);
  return *this;
}

// -----------------------------------------------------------------------------
inline Image &Image::operator *=(double value)
{
  DataArray::operator *=(value);
  return *this;
}

// -----------------------------------------------------------------------------
inline Image &Image::operator /=(double value)
{
  DataArray::operator /=(value);
  return *this;
}

// -----------------------------------------------------------------------------
inline Image Image::operator +(double value) const
{
  Image copy(*this);
  return copy += value;
}

// -----------------------------------------------------------------------------
inline Image Image::operator -(double value) const
{
  Image copy(*this);
  return copy -= value;
}

// -----------------------------------------------------------------------------
inline Image Image::operator *(double value) const
{
  Image copy(*this);
  return copy *= value;
}

// -----------------------------------------------------------------------------
inline Image Image::operator /(double value) const
{
  Image copy(*this);
  return copy /= value;
}

// =============================================================================
// Element-wise operations
// =============================================================================

// -----------------------------------------------------------------------------
inline Image &Image::operator +=(const Image &rhs)
{
  DataArray::operator +=(rhs);
  return *this;
}

// -----------------------------------------------------------------------------
inline Image &Image::operator -=(const Image &rhs)
{
  DataArray::operator -=(rhs);
  return *this;
}

// -----------------------------------------------------------------------------
inline Image &Image::operator *=(const Image &rhs)
{
  DataArray::operator *=(rhs);
  return *this;
}

// -----------------------------------------------------------------------------
inline Image &Image::operator /=(const Image &rhs)
{
  DataArray::operator /=(rhs);
  return *this;
}

// -----------------------------------------------------------------------------
inline Image Image::operator +(const Image &rhs) const
{
  Image copy(*this);
  return copy += rhs;
}

// -----------------------------------------------------------------------------
inline Image Image::operator -(const Image &rhs) const
{
  Image copy(*this);
  return copy -= rhs;
}

// -----------------------------------------------------------------------------
inline Image Image::operator *(const Image &rhs) const
{
  Image copy(*this);
  return copy *= rhs;
}

// -----------------------------------------------------------------------------
inline Image Image::operator /(const Image &rhs) const
{
  Image copy(*this);
  return copy /= rhs;
}


} } // namespace mirtk::future

#endif // MIRTK_Future_Image_H

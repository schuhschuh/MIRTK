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
#include "mirtk/ImageAttributes.h"


namespace mirtk { namespace future {

// Forward declaration of deprecated image class
class BaseImage;


/**
 * Multi-dimension array positioned in world space
 */
class Image : public DataArray
{
  mirtkObjectMacro(Image);

  // ---------------------------------------------------------------------------
  // Attributes

  /// Image attributes
  mirtkReadOnlyAttributeMacro(ImageAttributes, Attributes);

  /// Copy attributes of this class from another instance
  void CopyAttributes(const Image &other);

  /// Swap attributes of this class with another instance
  void SwapAttributes(Image &);

protected:

  /// Swap attributes of this class and superclasses with another instance
  void Swap(Image &);

public:

  // ---------------------------------------------------------------------------
  // Construction/Destruction

  /// Default constructor
  Image() = default;

  /// Copy construct from non-const reference to other object
  ///
  /// \param[in] other     Non-const reference to other object.
  /// \param[in] platform  Target platform. Use \p other platform if Platform_Default.
  /// \param[in] device    Target device. Use device where \p values memory is
  ///                      located or active device of \p platform when negative.
  /// \param[in] copy_data When \c false, a copy of the data memory is only
  ///                      performed when the target \p platform and/or \p device
  ///                      differs from where the data memory of the \p other
  ///                      instance is located. When \c true, the data memory
  ///                      of the \p other instance is always copied.
  Image(Image &other, PlatformId platform  = Platform_Default,
                      DeviceId   device    = -1,
                      bool       copy_data = false);

  /// Copy construct from non-const reference to other object
  ///
  /// \param[in] other       Non-const reference to other object.
  /// \param[in] platform    Target platform. Use \p other platform if Platform_Default.
  /// \param[in] device      Target device. Use device where \p values memory is
  ///                        located or active device of \p platform when negative.
  /// \param[in] copy_values When \c false, a copy of the values memory is only
  ///                        performed when the target \p platform and/or \p device
  ///                        differs from where the values memory of the \p other
  ///                        instance is located. When \c true, the values memory
  ///                        of the \p other instance is always copied.
  /// \param[in] copy_status When \c false, a copy of the status memory is only
  ///                        performed when the target \p platform and/or \p device
  ///                        differs from where the status memory of the \p other
  ///                        instance is located. When \c true, the status memory
  ///                        of the \p other instance is always copied.
  Image(Image &other, PlatformId platform,
                      DeviceId   device,
                      bool       copy_values,
                      bool       copy_status);

  /// Copy construct from const reference to other object
  Image(const Image &);

  /// Move constructor
  Image(Image &&);

  /// Destructor
  virtual ~Image();

  // ---------------------------------------------------------------------------
  // Assignment operator

  /// Copy assignment operator
  Image &operator =(const Image &);

  /// Move assignment operator
  Image &operator =(Image &&);

  /// Assign constant scalar value
  Image &operator =(double);

  // ---------------------------------------------------------------------------
  // Deprecated

  /// Copy construct from deprecated image
  Image(const BaseImage &);

  /// Move construct from deprecated image
  Image(BaseImage &&);

  /// Copy assignment from deprecated image
  Image &operator =(const BaseImage &);

  /// Move assignment from deprecated image
  Image &operator =(BaseImage &&);

  // ---------------------------------------------------------------------------
  // Make copy

  // Import other overload
  using DataArray::Copy;

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

  /// Whether this array is empty
  bool IsEmpty() const;

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

};

////////////////////////////////////////////////////////////////////////////////
// Inline definitions
////////////////////////////////////////////////////////////////////////////////

// =============================================================================
// Assignment operator
// =============================================================================

// -----------------------------------------------------------------------------
inline Image &Image::operator =(double c)
{
  DataArray::operator =(c);
  return *this;
}

// =============================================================================
// Data object interface
// =============================================================================

// -----------------------------------------------------------------------------
inline size_t Image::Bytes() const
{
  return DataArray::Bytes() + sizeof(ImageAttributes);
}

// =============================================================================
// Scalar operations
// =============================================================================

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

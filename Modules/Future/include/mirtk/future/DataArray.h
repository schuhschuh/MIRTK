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

#ifndef MIRTK_Future_DataArray_H
#define MIRTK_Future_DataArray_H

#include "mirtk/future/DataObject.h"

#include "mirtk/SmartPtr.h"
#include "mirtk/DataTypes.h"
#include "mirtk/future/DataMemory.h"
#include "mirtk/future/GenericMemory.h"


namespace mirtk { namespace future {


/**
 * Data array interface
 *
 * A data array stores a variable number of tuple elements with a fixed number
 * of components per element for a variable number of data points. The number of
 * element components is usually known at compile time and depends on the specific
 * element type. For example, basic data types such as unsigned int, int, float,
 * and double are scalar element types and therefore each element has exactly one
 * component. Vector element types such as int2, double3, array, Point, Vector3, and
 * Vector3D may have more than one, but a fixed number of components.
 *
 * This non-templated base class performs all access operations knowing only the number
 * of bytes per component and the number of components per element. This requires
 * additional arithmetic operations for each data access. The TypedArray subclass
 * is therefore templated over the element type and provides faster element access.
 *
 * Each tuple has a status which can be either Active or Passive. While a single
 * data memory can be shared by different data arrays, the status array is unique
 * to one such instance. It can be used to mark data tuples as Passive
 * (e.g., image background, outside region-of-interest, invalid measurement, outlier)
 * and thus to exclude these from a data filter operation. A shallow copy of an
 * existing data array can be made to use the same data tuples, but different
 * status arrays for different filter operations.
 *
 * - Tuple:     Ordered list of components
 * - Component: Tuple element whose type is given by the array data type
 * - Scalar:    Single scalar component of a tuple element, when the data type
 *              is a scalar type, each component has only one scalar.
 *              Otherwise, a tuple element consists of more than one, but fixed
 *              number of scalar components. For example, a double has 1 scalar,
 *              whereas a double3 has 3 scalars.
 *
 * How to make a copy of DataArray \c a:
 *
 * \code
 * DataArray b(a.Copy());
 * DataArray c = a.Copy();
 * DataArray d = const_cast<const DataArray>(a); // makes copy of const DataArray
 * DataArray e(const_cast<const DataArray>(a));  // makes copy of const DataArray
 * \endcode
 *
 * How to only make a copy of the mask, but share the data:
 *
 * \code
 * DataArray b(a.Data()); // initializes new mask for b
 * DataArray c = a.CopyMask();
 * DataArray d = DataArray(a.Data());
 * \endcode
 *
 * How to avoid any copy and share both data and mask:
 *
 * \code
 * DataArray b(a);   // no copy when other     array is non-const DataArray
 * DataArray c = a;  // no copy when right-hand side is non-const DataArray
 * DataArray d(a.Data(), a.Mask());
 * \endcode
 *
 * How to move the data and mask, leaving the DataArray \c a empty:
 *
 * \code
 * DataArray b;
 * move(b, a);
 * DataArray c = static_cast<DataArray &&>(a);
 * \endcode
 *
 * How to switch platforms and transfer a copy of the data between devices:
 *
 * \code
 * DataArray b(a, Platform_CUDA);
 * // Get CUDA device memory and lock it
 * real_t *p = static_cast<real_t *>(b.RawPointer());
 * // Run CUDA kernel, then unlock memory
 * b.Unlock();
 * // Result is in b
 * \endcode
 *
 * How to ensure a certain platform with copying and transferring between devices
 * only performed when necessary, i.e., when the platform of the DataArray \c a
 * differs from the platform used to process the data:
 *
 * \code
 * DataArray b(a.Data(), a.Mask(), Platform_CUDA);
 * // Get CUDA device memory and lock it
 * real_t *p = static_cast<real_t *>(b.RawPointer());
 * // Run CUDA kernel, then unlock memory
 * b.Unlock();
 * // Result is in both a and b
 * \endcode
 */
class DataArray : public DataObject
{
  mirtkObjectMacro(DataArray);

  // ---------------------------------------------------------------------------
  // Attributes

private:

  /// Number of tuples
  mirtkReadOnlyAttributeMacro(Id, Tuples);

  /// Number of elements per tuple
  mirtkReadOnlyAttributeMacro(int, Components);

  /// Memory where the data values are stored in column-major order
  /// - rows: tuple elements / components
  /// - cols: tuples
  /// Note that the tuples are linearly arranged in memory, but subclasses may
  /// reshape this linear memory in any desired layout for a multi-dimensional
  /// access by mapping the multi-dimensional index to the respective linear
  /// memory index. The only requirement is that all elements of one tuple
  /// are in consecutive memory addresses, where each element occupies the
  /// number of bytes corresponding to the data type. This is so that low-level
  /// filters that process any data tuple-wise can access all components through
  /// a single pointer to the first component of the current tuple.
  SharedPtr<DataMemory> _Values;

  /// Linear memory where the tuple status is stored
  SharedPtr<StatusMemory> _Status;

  /// Copy attributes of this class from another instance
  ///
  /// \param[in] other       Instance.
  /// \param[in] platform    Target platform.
  /// \param[in] device      Target device of \p platform.
  /// \param[in] copy_values Force copy of tuple values.
  /// \param[in] copy_status Force copy of tuple status.
  void CopyAttributes(const DataArray &other,
                      PlatformId platform, DeviceId device,
                      bool copy_values, bool copy_status);

  /// Swap attributes of this class with those of another instance
  void SwapAttributes(DataArray &);

protected:

  /// Swap attributes of this class and superclasses with those of another instance
  void Swap(DataArray &);

public:

  // ---------------------------------------------------------------------------
  // Construction/Destruction

  /// Default constructor
  DataArray();

  /// Construct from shared values memory
  ///
  /// When the platform which manages the memory of the given device \p data
  /// differs from the target \p platform, a copy of the array. When the
  /// device of the target \p platform differs from the device on which the
  /// source \p data array is stored, the copy includes a transfer between
  /// the two devices. When the source and target platforms are identical or
  /// when the target \p platform is Platform_Default, no copy is made and
  /// both data arrays share the \p data array. A copy of the mask array
  /// is still made such that both arrays can have different data masks.
  ///
  /// \param[in] tuples   Number of tuples. The number of components is set to
  ///                     the number of elements divided by this number.
  /// \param[in] values   Shared pointer to device data.
  /// \param[in] platform Target platform. Use \p values platform if Platform_Default.
  /// \param[in] device   Target device. Use \p values device or active device
  ///                     of \p platform when argument is negative.
  explicit DataArray(Id tuples, SharedPtr<DataMemory> values,
                     PlatformId platform = Platform_Default,
                     DeviceId   device  = -1);

  /// Construct from shared values memory
  ///
  /// When the platform which manages the memory of the given device \p data
  /// differs from the target \p platform, a copy of the array. When the
  /// device of the target \p platform differs from the device on which the
  /// source \p data array is stored, the copy includes a transfer between
  /// the two devices. When the source and target platforms are identical or
  /// when the target \p platform is Platform_Default, no copy is made and
  /// both data arrays share the \p data array. A copy of the mask array
  /// is still made such that both arrays can have different data masks.
  ///
  /// The number of components is set to 1 by this constructor. Each element
  /// of the values memory is considered one tuple of the data.
  ///
  /// \param[in] values   Shared pointer to device data.
  /// \param[in] platform Target platform. Use \p values platform if Platform_Default.
  /// \param[in] device   Target device. Use \p values device or active device
  ///                     of \p platform when argument is negative.
  explicit DataArray(SharedPtr<DataMemory> values,
                     PlatformId platform = Platform_Default,
                     DeviceId   device  = -1);

  /// Construct from shared memory
  ///
  /// When the platform which manages the memory of the given device \p data
  /// differs from the target \p platform, a copy of the array. When the
  /// device of the target \p platform differs from the device on which the
  /// source \p data array is stored, the copy includes a transfer between
  /// the two devices. When the source and target platforms are identical or
  /// when the target \p platform is Platform_Default, no copy is made and
  /// both data arrays share the \p data array. The same applies to the
  /// \p mask array.
  ///
  /// \param[in] values   Shared data array.
  /// \param[in] status   Shared tuple status array.
  /// \param[in] platform Target platform. Use \p values platform if Platform_Default.
  /// \param[in] device   Target device. Use \p values device or active device
  ///                     of \p platform when argument is negative.
  explicit DataArray(SharedPtr<DataMemory> values,
                     SharedPtr<StatusMemory> status,
                     PlatformId platform = Platform_Default,
                     DeviceId   device  = -1);

  /// Copy construct from non-const reference to other instance
  ///
  /// \param[in] other     Non-const reference to other instance.
  /// \param[in] platform  Target platform. Use \p other platform if Platform_Default.
  /// \param[in] device    Target device. Use device where \p values memory is
  ///                      located or active device of \p platform when negative.
  /// \param[in] copy_data When \c false, a copy of the data memory is only
  ///                      performed when the target \p platform and/or \p device
  ///                      differs from where the data memory of the \p other
  ///                      instance is located. When \c true, the data memory
  ///                      of the \p other instance is always copied.
  DataArray(DataArray &other, PlatformId platform  = Platform_Default,
                              DeviceId   device    = -1,
                              bool       copy_data = false);

  /// Copy construct from non-const reference to other instance
  ///
  /// \param[in] other       Non-const reference to other instance.
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
  DataArray(DataArray &other, PlatformId platform,
                              DeviceId   device,
                              bool       copy_values,
                              bool       copy_status);

  /// Copy construct from const reference to other instance
  ///
  /// \param[in] other    Const reference to other instance.
  /// \param[in] platform Target platform. Use \p other platform if Platform_Default.
  /// \param[in] device   Target device. Use device where \p values memory is
  ///                     located or active device of \p platform when negative.
  DataArray(const DataArray &other, PlatformId platform = Platform_Default,
                                    DeviceId   device  = -1);

  /// Move constructor
  DataArray(DataArray &&);

  /// Destructor
  virtual ~DataArray();

  // ---------------------------------------------------------------------------
  // Assignment operator

  /// Copy assignment operator
  DataArray &operator =(const DataArray &);

  /// Move assignment operator
  DataArray &operator =(DataArray &&);

  /// Assign constant scalar value
  DataArray &operator =(double);

  // ---------------------------------------------------------------------------
  // Make copy

  /// Make a copy of this data object
  ///
  /// \param[in] platform    Target platform. Use platform of this object if Platform_Default.
  /// \param[in] device      Target device. Use device of this object or
  ///                        active device of \p platform when negative.
  /// \param[in] copy_values Request copy of the values memory.
  /// \param[in] copy_status Request copy of the status memory.
  virtual SharedPtr<DataArray> Copy(PlatformId platform, DeviceId  device,
                                    bool copy_values, bool copy_status);

  /// Make a copy of this data object
  ///
  /// \param[in] platform Target platform. Use platform of this object if Platform_Default.
  /// \param[in] device   Target device. Use device of this object or
  ///                     active device of \p platform when negative.
  SharedPtr<DataArray> Copy(PlatformId platform = Platform_Default,
                            DeviceId   device  = -1) const;

  /// Make a copy of the values memory, but not the status memory
  SharedPtr<DataArray> CopyValues();

  /// Make a copy of the status memory, but not the values memory
  SharedPtr<DataArray> CopyStatus();

  // ---------------------------------------------------------------------------
  // Data object interface

  /// Platform which manages the memory of both tuple values and status arrays
  PlatformId Platform() const;

  /// ID of device on which both tuple values and status arrays are stored
  DeviceId Device() const;

  /// Kind of data object
  DataKind Kind() const;

  /// Data type of array elements
  DataType Type() const;

  /// Number of bytes this data object occupies in memory
  size_t Bytes() const;

  /// Whether this array is empty
  bool IsEmpty() const;

  // ---------------------------------------------------------------------------
  // Attributes

  /// Cast to bool
  ///
  /// \retval true  When array is non-empty.
  /// \retval false When array is empty, i.e., Size() returns zero.
  operator bool() const;

  /// Total number of elements
  Id Size() const;

  /// Reset status of all tuples to Active
  void ResetStatus();

  /// Change status of Active tuples to Passive and vice versa
  void NegateStatus();

  // ---------------------------------------------------------------------------
  // Scalar operations

  /// Add constant value to all elements
  DataArray &operator +=(double);

  /// Subtract constant value from all elements
  DataArray &operator -=(double);

  /// Multiply all elements with a constant value
  DataArray &operator *=(double);

  /// Divide all elements by a constant value
  DataArray &operator /=(double);


  /// Add constant value to all elements
  DataArray operator +(double) const;

  /// Subtract constant value from all elements
  DataArray operator -(double) const;

  /// Multiply all elements with a constant value
  DataArray operator *(double) const;

  /// Divide all elements by a constant value
  DataArray operator /(double) const;

  // ---------------------------------------------------------------------------
  // Element-wise operations

  /// Add constant value to all elements
  DataArray &operator +=(const DataArray &);

  /// Subtract constant value from all elements
  DataArray &operator -=(const DataArray &);

  /// Multiply all elements with a constant value
  DataArray &operator *=(const DataArray &);

  /// Divide all elements by a constant value
  DataArray &operator /=(const DataArray &);


  /// Add constant value to all elements
  DataArray operator +(const DataArray &) const;

  /// Subtract constant value from all elements
  DataArray operator -(const DataArray &) const;

  /// Multiply all elements with a constant value
  DataArray operator *(const DataArray &) const;

  /// Divide all elements by a constant value
  DataArray operator /(const DataArray &) const;

  // ---------------------------------------------------------------------------
  // Memory access
  //
  // Attention: The memory can only be read/written by the device on which the
  //            memory was allocated. Therefore ensure that the platform processing
  //            the data is using the same device as where the data is stored.

  /// Get pointer to memory storing either the data values or tuple status
  ///
  /// When the template argument is StatusValue, the pointer to the tuple status
  /// array is returned, otherwise, the pointer to memory storing the data values
  /// is returned after casting it to the requested type. It is the callers
  /// responsibility to use the appropriate type for reading/writing the data
  /// values. Note that the template argument can also be void.
  ///
  /// \returns Pointer to device memory.
  template <class T> T *Pointer();

  /// Get const pointer to memory storing either the data values or tuple status
  ///
  /// When the template argument is StatusValue, the pointer to the tuple status
  /// array is returned, otherwise, the pointer to memory storing the data values
  /// is returned after casting it to the requested type. It is the callers
  /// responsibility to use the appropriate type for reading/writing the data
  /// values. Note that the template argument can also be void.
  ///
  /// \returns Pointer to device memory.
  template <class T> const T *Pointer() const;

  /// Set pointer to device data
  void Values(SharedPtr<DataMemory>);

  /// Get pointer to device data
  SharedPtr<DataMemory> Values();

  /// Get const pointer to device data
  SharedPtr<const DataMemory> Values() const;

  /// Get pointer to device mask
  SharedPtr<StatusMemory> Status();

  /// Get const pointer to device mask
  SharedPtr<const StatusMemory> Status() const;
};

////////////////////////////////////////////////////////////////////////////////
// Inline definitions
////////////////////////////////////////////////////////////////////////////////

// =============================================================================
// Make copy
// =============================================================================

// -----------------------------------------------------------------------------
inline SharedPtr<DataArray> DataArray::Copy(PlatformId platform, DeviceId device) const
{
  return const_cast<DataArray *>(this)->Copy(platform, device, true, true);
}

// -----------------------------------------------------------------------------
inline SharedPtr<DataArray> DataArray::CopyValues()
{
  return this->Copy(Platform_Default, -1, true, false);
}

// -----------------------------------------------------------------------------
inline SharedPtr<DataArray> DataArray::CopyStatus()
{
  return this->Copy(Platform_Default, -1, false, true);
}

// -----------------------------------------------------------------------------
inline DataArray &DataArray::operator =(double c)
{
  if (_Values) *_Values = c;
  return *this;
}

// =============================================================================
// Attributes
// =============================================================================

// -----------------------------------------------------------------------------
inline Id DataArray::Size() const
{
  return _Tuples * static_cast<Id>(_Components);
}

// -----------------------------------------------------------------------------
inline DataArray::operator bool() const
{
  return Size() != 0;
}

// =============================================================================
// Scalar operations
// =============================================================================

// -----------------------------------------------------------------------------
inline DataArray DataArray::operator +(double value) const
{
  DataArray copy(*this);
  return copy += value;
}

// -----------------------------------------------------------------------------
inline DataArray DataArray::operator -(double value) const
{
  DataArray copy(*this);
  return copy -= value;
}

// -----------------------------------------------------------------------------
inline DataArray DataArray::operator *(double value) const
{
  DataArray copy(*this);
  return copy *= value;
}

// -----------------------------------------------------------------------------
inline DataArray DataArray::operator /(double value) const
{
  DataArray copy(*this);
  return copy /= value;
}

// =============================================================================
// Element-wise operations
// =============================================================================

// -----------------------------------------------------------------------------
inline DataArray DataArray::operator +(const DataArray &rhs) const
{
  DataArray copy(*this);
  return copy += rhs;
}

// -----------------------------------------------------------------------------
inline DataArray DataArray::operator -(const DataArray &rhs) const
{
  DataArray copy(*this);
  return copy -= rhs;
}

// -----------------------------------------------------------------------------
inline DataArray DataArray::operator *(const DataArray &rhs) const
{
  DataArray copy(*this);
  return copy *= rhs;
}

// -----------------------------------------------------------------------------
inline DataArray DataArray::operator /(const DataArray &rhs) const
{
  DataArray copy(*this);
  return copy /= rhs;
}

// =============================================================================
// Memory access
// =============================================================================

// -----------------------------------------------------------------------------
template <class T>
inline T *DataArray::Pointer()
{
  return _Values ? _Values->Pointer<T>() : nullptr;
}

// -----------------------------------------------------------------------------
template <class T>
inline const T *DataArray::Pointer() const
{
  return const_cast<DataArray *>(this)->Pointer<T>();
}

// -----------------------------------------------------------------------------
template <>
inline StatusValue *DataArray::Pointer<StatusValue>()
{
  return _Status ? _Status->Pointer() : nullptr;
}

// -----------------------------------------------------------------------------
template <>
inline const StatusValue *DataArray::Pointer<StatusValue>() const
{
  return const_cast<DataArray *>(this)->Pointer<StatusValue>();
}

// -----------------------------------------------------------------------------
inline void DataArray::Values(SharedPtr<DataMemory> values)
{
  _Values = values;
}

// -----------------------------------------------------------------------------
inline SharedPtr<DataMemory> DataArray::Values()
{
  return _Values;
}

// -----------------------------------------------------------------------------
inline SharedPtr<const DataMemory> DataArray::Values() const
{
  return _Values;
}

// -----------------------------------------------------------------------------
inline SharedPtr<StatusMemory> DataArray::Status()
{
  return _Status;
}

// -----------------------------------------------------------------------------
inline SharedPtr<const StatusMemory> DataArray::Status() const
{
  return _Status;
}


} } // namespace mirtk::future

#endif // MIRTK_Future_DataArray_H
